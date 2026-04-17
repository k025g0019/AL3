#include "Player.h"

#include <algorithm>
#include <array>
#include <numbers>

#include "MapChipField.h"
#define NOMINMAX
namespace KamataEngine {
class Model;
}

using namespace KamataEngine;

namespace {
Vector3 Add(const Vector3& a, const Vector3& b) {
	Vector3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}
}

Player::Player() {}
Player::~Player() {}

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	camera_ = camera;
	model_ = model;
	velocity_ = {};
	isDead_ = false;
	useScreenLeftLimit_ = false;
	onGround_ = true;

	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.translation_ = position;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Player::Update() {
	const float deltaTime = 1.0f / 60.0f;

	if (isDead_) {
		worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
		worldTransform_.TransferMatrix();
		return;
	}

	bool laning = false;
	if (velocity_.y < 0.0f) {
		if (worldTransform_.translation_.y <= 2.0f) {
			laning = true;
			worldTransform_.translation_.y = 2.0f;
			velocity_.y = 0.0f;
		}
	}

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
	}

	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};

			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenu);
				}
				if (lrDirection != LRDirection::kRigh) {
					lrDirection = LRDirection::kRigh;
					turnFirstRotationY = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x += 50.0f;
			}

			if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenu);
				}
				if (lrDirection != LRDirection::KLeft) {
					lrDirection = LRDirection::KLeft;
					turnFirstRotationY = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x -= 50.0f;
			}

			velocity_.x += acceleration.x * deltaTime;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenu);
		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y = kJumpAcceleration;
			onGround_ = false;
		}
	} else {
		if (laning) {
			worldTransform_.translation_.y = 2.0f;
			velocity_.x *= (1.0f - kAttenu);
			velocity_.y = 0.0f;
			onGround_ = true;
		}

		velocity_.y += -kGravity;
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}

	if (turnTimer_ > 0.0f) {
		turnTimer_ -= deltaTime;
		turnTimer_ = (std::max)(turnTimer_, 0.0f);

		const float t = 1.0f - (turnTimer_ / kTimeTurn);
		const float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,
		    std::numbers::pi_v<float> * 3.0f / 2.0f,
		};
		const float destinationRotationY = destinationRotationYTable[static_cast<int>(lrDirection)];
		worldTransform_.rotation_.y = std::lerp(turnFirstRotationY, destinationRotationY, t);
	}

	CollisionMapInfo collisionMapInfo{};
	collisionMapInfo.movement.x = velocity_.x * deltaTime;
	collisionMapInfo.movement.y = velocity_.y * deltaTime;
	collisionMapInfo.movement.z = velocity_.z * deltaTime;

	float requiredScreenPush = 0.0f;
	if (useScreenLeftLimit_) {
		const float requiredCenterX = screenLeftLimit_ + kWidth / 2.0f + kBlank;
		if (worldTransform_.translation_.x + collisionMapInfo.movement.x < requiredCenterX) {
			requiredScreenPush = requiredCenterX - worldTransform_.translation_.x;
			collisionMapInfo.movement.x = (std::max)(collisionMapInfo.movement.x, requiredScreenPush);
		}
	}

	MapCollisionDetection(collisionMapInfo);

	if (requiredScreenPush > 0.0f && collisionMapInfo.wallRight && collisionMapInfo.movement.x + kBlank < requiredScreenPush) {
		Kill();
	}

	ApplyCollision(collisionMapInfo);
	ProcessCeilingHit(collisionMapInfo);
	ProcessWallHit(collisionMapInfo);
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

const WorldTransform& Player::GetWorldTransform() const { return worldTransform_; }

void Player::MapCollisionDetection(CollisionMapInfo& info) {
	if (info.movement.y > 0.0f) {
		const std::array<Vector3, 2> checkPoints = {
		    Add(worldTransform_.translation_, {+kWidth / 2.0f, +kHeight / 2.0f + info.movement.y, 0.0f}),
		    Add(worldTransform_.translation_, {-kWidth / 2.0f, +kHeight / 2.0f + info.movement.y, 0.0f}),
		};

		float resolvedMovementY = info.movement.y;
		bool hit = false;

		for (const Vector3& point : checkPoints) {
			const MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(point);
			if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) != MapChipType::kBlock) {
				continue;
			}

			const MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			resolvedMovementY = (std::min)(resolvedMovementY, rect.bottom - (worldTransform_.translation_.y + kHeight / 2.0f) - kBlank);
			hit = true;
		}

		if (hit) {
			info.movement.y = resolvedMovementY;
			info.ceiling = true;
		}
	}

	const Vector3 verticalResolvedCenter = Add(worldTransform_.translation_, {0.0f, info.movement.y, 0.0f});

	if (info.movement.x > 0.0f) {
		const std::array<Vector3, 2> checkPoints = {
		    Add(verticalResolvedCenter, {+kWidth / 2.0f + info.movement.x, +kHeight / 2.0f - kBlank, 0.0f}),
		    Add(verticalResolvedCenter, {+kWidth / 2.0f + info.movement.x, -kHeight / 2.0f + kBlank, 0.0f}),
		};

		float resolvedMovementX = info.movement.x;
		bool hit = false;

		for (const Vector3& point : checkPoints) {
			const MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(point);
			if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) != MapChipType::kBlock) {
				continue;
			}

			const MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			resolvedMovementX = (std::min)(resolvedMovementX, rect.left - (worldTransform_.translation_.x + kWidth / 2.0f) - kBlank);
			hit = true;
		}

		if (hit) {
			info.movement.x = resolvedMovementX;
			info.wall = true;
			info.wallRight = true;
		}
	} else if (info.movement.x < 0.0f) {
		const std::array<Vector3, 2> checkPoints = {
		    Add(verticalResolvedCenter, {-kWidth / 2.0f + info.movement.x, +kHeight / 2.0f - kBlank, 0.0f}),
		    Add(verticalResolvedCenter, {-kWidth / 2.0f + info.movement.x, -kHeight / 2.0f + kBlank, 0.0f}),
		};

		float resolvedMovementX = info.movement.x;
		bool hit = false;

		for (const Vector3& point : checkPoints) {
			const MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(point);
			if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) != MapChipType::kBlock) {
				continue;
			}

			const MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			resolvedMovementX = (std::max)(resolvedMovementX, rect.right - (worldTransform_.translation_.x - kWidth / 2.0f) + kBlank);
			hit = true;
		}

		if (hit) {
			info.movement.x = resolvedMovementX;
			info.wall = true;
			info.wallLeft = true;
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	const Vector3 offsetTable[kNumCorners] = {
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
	};

	Vector3 result;
	result.x = center.x + offsetTable[static_cast<int>(corner)].x;
	result.y = center.y + offsetTable[static_cast<int>(corner)].y;
	result.z = center.z + offsetTable[static_cast<int>(corner)].z;
	return result;
}

void Player::ApplyCollision(const CollisionMapInfo& info) {
	worldTransform_.translation_.x += info.movement.x;
	worldTransform_.translation_.y += info.movement.y;
	worldTransform_.translation_.z += info.movement.z;
}

void Player::ProcessCeilingHit(const CollisionMapInfo& info) {
	if (info.ceiling) {
		velocity_.y = 0.0f;
	}
}

void Player::ProcessWallHit(const CollisionMapInfo& info) {
	if (info.wall) {
		velocity_.x = 0.0f;
	}
}

void Player::Kill() {
	if (isDead_) {
		return;
	}

	isDead_ = true;
	velocity_ = {};
	onGround_ = false;
	worldTransform_.scale_.x = 2.8f;
	worldTransform_.scale_.y = 1.0f;
}
