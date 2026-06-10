#include "ShieldEnemy.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

#include "GameScene.h"
#include "Player.h"
Player::LRDirection PlayerLR;
using namespace KamataEngine;

ShieldEnemy::ShieldEnemy() {
}

ShieldEnemy::~ShieldEnemy() {
}

void ShieldEnemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
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

void ShieldEnemy::BehaviorWalkInitialize() {
	velocity_.x = -kWalSpeed;
	walkTimer_ = 0.0f;
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
}

void ShieldEnemy::BehaviorWalkUpdate() {
	worldTransform_.translation_.x += velocity_.x;
	walkTimer_ += (1.0f / 60.0f);

	float param = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);
	float t = (param + 1.0f) * 0.5f;
	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * t;
	float rad = degree * (std::numbers::pi_v<float> / 180.0f);
	(void)rad;
	//worldTransform_.rotation_.x = std::numbers::pi_v<float> * 3.0f / 2.0f;
	worldTransform_.rotation_.x = 0;
	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f + rad;
	worldTransform_.rotation_.z = 0.0f;
}

void ShieldEnemy::BehaviorDeadInitialize() {
	isCollisionDisabled_ = true;
	deathTimer_ = 0.0f;
}

void ShieldEnemy::BehaviorDeadUpdate() {
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

void ShieldEnemy::Update() {
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

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_,
	                                             worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void ShieldEnemy::Draw() { model_->Draw(worldTransform_, *camera_); }

Vector3 ShieldEnemy::GetWorldPosition() const { return worldTransform_.translation_; }

AABB ShieldEnemy::GetAABB() const {
	const Vector3 center = GetWorldPosition();
	constexpr Vector3 half = {kWidth * 0.5f, kHeight * 0.5f, kWidth * 0.5f};

	AABB aabb;
	aabb.min = {center.x - half.x, center.y - half.y, center.z - half.z};
	aabb.max = {center.x + half.x, center.y + half.y, center.z + half.z};
	return aabb;
}

void ShieldEnemy::OnCollision(Player* player) {
	if (behavior_ == Behavior::kDead) {
		return;
	}

	if (player && player->IsAttack()) {
		PlayerLR = player->GettrLR();
		if ((PlayerLR == Player::LRDirection::kRigh && worldTransform_.rotation_.y <= 0) || (PlayerLR ==
			Player::LRDirection::KLeft && worldTransform_.rotation_.y >= 0)) {
			if (gameScene_) {
				const Vector3 ShieldEnemyPos = GetWorldPosition();
				const Vector3 playerPos = player->GetWorldPosition();
				Vector3 effectPos = {
					(ShieldEnemyPos.x + playerPos.x) * 0.5f,
					(ShieldEnemyPos.y + playerPos.y) * 0.5f,
					(ShieldEnemyPos.z + playerPos.z) * 0.5f,
				};
				Vector3 guneffectPos = {
					(worldTransform_.translation_.x + player->GetWorldPosition().x) * 0.5f,
					(worldTransform_.translation_.y + player->GetWorldPosition().y) * 0.5f,
					(worldTransform_.translation_.z + player->GetWorldPosition().z) * 0.5f,
				};
				gameScene_->CreateGunEffect(guneffectPos);
				player->RequestKnockBack();
			}
			return;
		}
		behaviorRequest_ = Behavior::kDead;
	}
}
