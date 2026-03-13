//====================
// 敵 実装
//====================
#include "Enemy.h"

#include <cassert>
#include <cmath>

#include "Matrix4x4.h"

#define PI 3.141592654f

using namespace KamataEngine;

//====================
// 生成破棄
//====================
Enemy::Enemy() {}
Enemy::~Enemy() {}

//====================
// 初期化
//====================
/*
モデル・カメラ・初期座標を設定する
*/
void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// ポインタの有効性を確認
	assert(model);
	assert(camera);

	// 受け取った参照を保存
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期値を設定
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.rotation_.y = PI * 3.0f / 2.0f; // プレイヤーと向きを合わせる

	// 移動パラメータを初期化
	velocity_.x = -kWalSpeed;
	walkTimer_ = 0.0f;
}

//====================
// 更新
//====================
/*
移動と歩行アニメーションを更新する
*/
void Enemy::Update() {
	// 速度から位置を更新
	worldTransform_.translation_.x += velocity_.x;

	// 歩行アニメーション用タイマー
	walkTimer_ += (1.0f / 60.0f);

	// サイン波で首振りアニメーション
	float param = std::sin(PI * 2.0f * walkTimer_ / kWalkMotionTime);
	float t = (param + 1.0f) / 2.0f;
	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * t;
	float rad = degree * (PI / 180.0f);

	// 基準向き + 首振りを適用
	worldTransform_.rotation_.x = rad;
	worldTransform_.rotation_.y = PI * 3.0f / 2.0f;
	worldTransform_.rotation_.z = 0.0f;

	// SRT からワールド行列を作成
	Matrix4x4 s = MakeScaleMatrix(worldTransform_.scale_);
	Matrix4x4 rx = MakeRotateX(worldTransform_.rotation_.x);
	Matrix4x4 ry = MakeRotateY(worldTransform_.rotation_.y);
	Matrix4x4 tMat = MakeTranslateMatrix(worldTransform_.translation_);
	Matrix4x4 r = Multiply(rx, ry);
	worldTransform_.matWorld_ = Multiply(Multiply(s, r), tMat);
	worldTransform_.TransferMatrix();
}

//====================
// 描画
//====================
/*
敵モデルを描画する
*/
void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }

//====================
// 座標取得
//====================
/*
敵のワールド座標を取得する
*/
KamataEngine::Vector3 Enemy::GetWorldPosition() const { return worldTransform_.translation_; }

//====================
// AABB取得
//====================
/*
敵の当たり判定AABBを返す
*/
AABB Enemy::GetAABB() const {
	// 中心座標と半径を計算
	const Vector3 center = GetWorldPosition();
	const Vector3 half = {kWidth * 0.5f, kHeight * 0.5f, kWidth * 0.5f};

	// 最小座標・最大座標を設定
	AABB aabb;
	aabb.min = {center.x - half.x, center.y - half.y, center.z - half.z};
	aabb.max = {center.x + half.x, center.y + half.y, center.z + half.z};
	return aabb;
}

//====================
// 衝突時処理
//====================
/*
プレイヤーと衝突した時の処理
*/
void Enemy::OnCollision(const Player* player) { (void)player; }
