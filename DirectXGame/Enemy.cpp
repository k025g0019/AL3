#include "Enemy.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

#include "Player.h"

using namespace KamataEngine;

Enemy::Enemy() {}
Enemy::~Enemy() {}

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	velocity_.x = -kWalSpeed;
	walkTimer_ = 0.0f;
	isDead_ = false;
	isCollisionDisabled_ = false;
	deathTimer_ = 0.0f;
	behavior_ = Behavior::kWalk;
	behaviorRequest_ = Behavior::kUnknown;
}

void Enemy::BehaviorWalkInitialize() {
	velocity_.x = -kWalSpeed;
	walkTimer_ = 0.0f;
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
}

void Enemy::BehaviorWalkUpdate() {
	worldTransform_.translation_.x += velocity_.x;
	walkTimer_ += (1.0f / 60.0f);

	float param = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);
	float t = (param + 1.0f) * 0.5f;
	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * t;
	float rad = degree * (std::numbers::pi_v<float> / 180.0f);

	worldTransform_.rotation_.x = rad;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	worldTransform_.rotation_.z = 0.0f;
}

void Enemy::BehaviorDeadInitialize() {
	isCollisionDisabled_ = true;
	deathTimer_ = 0.0f;
}

void Enemy::BehaviorDeadUpdate() {
	deathTimer_ += (1.0f / 60.0f);
	const float t = std::clamp(deathTimer_ / kDeathDuration, 0.0f, 1.0f);

	worldTransform_.rotation_.y += 0.8f;
	worldTransform_.rotation_.x += 0.2f;
	worldTransform_.translation_.y -= 0.05f;
	worldTransform_.scale_ = {
		std::lerp(2.0f, 0.0f, t),
		std::lerp(2.0f, 0.0f, t),
		std::lerp(2.0f, 0.0f, t),
	};

	if (deathTimer_ >= kDeathDuration) {
		isDead_ = true;
	}
}

void Enemy::Update() {
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kWalk:
		default:
			BehaviorWalkInitialize();
			break;
		case Behavior::kDead:
			BehaviorDeadInitialize();
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kWalk:
	default:
		BehaviorWalkUpdate();
		break;
	case Behavior::kDead:
		BehaviorDeadUpdate();
		break;
	}

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }

KamataEngine::Vector3 Enemy::GetWorldPosition() const { return worldTransform_.translation_; }

AABB Enemy::GetAABB() const {
	const Vector3 center = GetWorldPosition();
	const Vector3 half = {kWidth * 0.5f, kHeight * 0.5f, kWidth * 0.5f};

	AABB aabb;
	aabb.min = {center.x - half.x, center.y - half.y, center.z - half.z};
	aabb.max = {center.x + half.x, center.y + half.y, center.z + half.z};
	return aabb;
}

void Enemy::OnCollision(const Player* player) {
	if (behavior_ == Behavior::kDead) {
		return;
	}

	if (player && player->IsAttack()) {
		behaviorRequest_ = Behavior::kDead;
	}
}