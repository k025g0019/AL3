//====================
// カメラ制御 実装
//====================
#include "CameraController.h"

#include <algorithm>

#include "Player.h"

using namespace KamataEngine;

//====================
// 初期化
//====================
/*
カメラの初期状態と可動範囲を設定する
*/
void CameraController::Initialize() {
	// カメラの内部状態を初期化
	camera_->Initialize();

	// ステージに合わせた可動範囲を設定
	movableArea_ = {
	    .left = 20.0f,
	    .top = 100.0f,
	    .right = 90.0f,
	    .bottom = 0.0f,
	};
}

//====================
// 更新
//====================
/*
ターゲット位置に追従するようカメラを更新する
*/
void CameraController::Update() {
	// ターゲットの座標と速度を取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	// 速度先読みつきの追従目標を計算
	targetPosition_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	targetPosition_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	targetPosition_.z = targetWorldTransform.translation_.z + targetOffset_.z;

	// 線形補間で滑らかに移動
	camera_->translation_.x = std::lerp(camera_->translation_.x, targetPosition_.x, kInterpolationRate);
	camera_->translation_.y = std::lerp(camera_->translation_.y, targetPosition_.y, kInterpolationRate);
	camera_->translation_.z = std::lerp(camera_->translation_.z, targetPosition_.z, kInterpolationRate);

	// マージン範囲内にクランプ
	camera_->translation_.x = std::clamp(camera_->translation_.x, targetPosition_.x + margin.left, targetPosition_.x + margin.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, targetPosition_.y + margin.bottom, targetPosition_.y + margin.top);

	// ステージ可動範囲内にクランプ
	camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, movableArea_.bottom, movableArea_.top);

	// ビュー行列を更新
	camera_->UpdateMatrix();
}

//====================
// リセット
//====================
/*
ターゲット位置にカメラを即時スナップする
*/
void CameraController::Reset() {
	// ターゲットの現在位置を取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// オフセット込みでカメラ位置を再設定
	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}
