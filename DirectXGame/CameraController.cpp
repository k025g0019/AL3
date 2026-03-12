#include "CameraController.h"

#include <algorithm>

#include "Player.h"

using namespace KamataEngine;

void CameraController::Initialize() {
	camera_->Initialize();

	// ステージに合わせた可動範囲
	movableArea_ = {
	    .left = 20.0f,
	    .top = 100.0f,
	    .right = 90.0f,
	    .bottom = 0.0f,
	};
}

void CameraController::Update() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	// 速度先読みつきの追従目標
	targetPosition_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	targetPosition_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	targetPosition_.z = targetWorldTransform.translation_.z + targetOffset_.z;

	// 線形補間で滑らかに移動
	camera_->translation_.x = std::lerp(camera_->translation_.x, targetPosition_.x, kInterpolationRate);
	camera_->translation_.y = std::lerp(camera_->translation_.y, targetPosition_.y, kInterpolationRate);
	camera_->translation_.z = std::lerp(camera_->translation_.z, targetPosition_.z, kInterpolationRate);

	// ターゲット周囲のマージン内に制限
	camera_->translation_.x = std::clamp(camera_->translation_.x, targetPosition_.x + margin.left, targetPosition_.x + margin.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, targetPosition_.y + margin.bottom, targetPosition_.y + margin.top);

	// ステージ全体の可動範囲内に制限
	camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, movableArea_.bottom, movableArea_.top);

	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}
