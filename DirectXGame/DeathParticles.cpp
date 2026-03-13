//====================
// デスパーティクル 実装
//====================
#include "DeathParticles.h"

#include <algorithm>
#include <cmath>

#include "Matrix4x4.h"

using namespace KamataEngine;

//====================
// 初期化
//====================
/*
モデル・カメラ・初期位置を設定し、色とワールド変換を初期化する
*/
void DeathParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// モデルとカメラ参照を保持
	model_ = model;
	camera_ = camera;

	// フェード用の初期化
	objectColor_.Initialize();
	color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	objectColor_.SetColor(color_);

	// 終了状態を初期化
	isFinished_ = false;
	counter_ = 0.0f;

	// 全パーティクルのワールド変換を初期化
	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.scale_ = {kStartScale, kStartScale, kStartScale};
		worldTransform.rotation_ = {0.0f, 0.0f, 0.0f};
		worldTransform.translation_ = position;
	}
}

//====================
// 更新
//====================
/*
8方向移動とフェードアウトを更新し、一定時間後に終了する
*/
void DeathParticles::Update() {
	// 終了済みなら更新しない
	if (isFinished_) {
		return;
	}

	// 8方向へ移動
	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// 基本となる右方向ベクトル
		Vector3 velocity = {kSpeed, 0.0f, 0.0f};

		// i番目の角度
		float angle = kAngleUnit * static_cast<float>(i);

		// Z軸回転で速度ベクトルを回転
		float c = std::cos(angle);
		float s = std::sin(angle);
		Vector3 rotatedVelocity{};
		rotatedVelocity.x = velocity.x * c - velocity.y * s;
		rotatedVelocity.y = velocity.x * s + velocity.y * c;
		rotatedVelocity.z = 0.0f;

		// 座標へ速度を加算
		worldTransforms_[i].translation_.x += rotatedVelocity.x;
		worldTransforms_[i].translation_.y += rotatedVelocity.y;
		worldTransforms_[i].translation_.z += rotatedVelocity.z;

		// 行列更新
		worldTransforms_[i].matWorld_ = MakeAffineMatrix(worldTransforms_[i].scale_, worldTransforms_[i].rotation_, worldTransforms_[i].translation_);
		worldTransforms_[i].TransferMatrix();
	}

	// カウンタを1フレーム分進める
	counter_ += 1.0f / 60.0f;

	// アルファ値を 1.0 -> 0.0 へ減衰
	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);
	objectColor_.SetColor(color_);

	// 存続時間に達したら終了
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}
}

//====================
// 描画
//====================
/*
全パーティクルを描画する（色変更オブジェクトを適用）
*/
void DeathParticles::Draw() {
	// 終了済みなら描画しない
	if (isFinished_) {
		return;
	}

	// 参照が無効なら描画しない
	if (!model_ || !camera_) {
		return;
	}

	// 全パーティクルを描画
	for (WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_, &objectColor_);
	}
}
