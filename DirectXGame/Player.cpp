//====================
// Player
//====================
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

Vector3 Add(const Vector3& a, const Vector3& b) {
	Vector3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

Player::Player() {
}

Player::~Player() {
}

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	camera_ = camera;
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.translation_ = position;

	isDead_ = false;
	behavior_ = Behavior::kRoot;
	behaviorRequest_ = Behavior::kUnknown;
	BehaviorRootInitialize();
}

void Player::BehaviorRootInitialize() {
	attackParameter_ = 0;
	attackPhase_ = AttackPhase::kCharge;
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
}

void Player::BehaviorRootUpdate() {
	constexpr float deltaTime = kDeltaTime;
	bool laning = false;

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

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
			if (turnTimer_ > 0.0f) {
				turnTimer_ -= deltaTime;
				turnTimer_ = (std::max)(turnTimer_, 0.0f);
				float t = 1.0f - (turnTimer_ / kTimeTurn);

				float destinationRotationYTable[]{
					std::numbers::pi_v<float> / 2.0f,
					std::numbers::pi_v<float> * 3.0f / 2.0f,
				};

				float destinationRotationY = destinationRotationYTable[static_cast<int>(lrDirection)];
				worldTransform_.rotation_.y = std::lerp(turnFirstRotationY, destinationRotationY, t);
			}

			velocity_.x += acceleration.x * deltaTime;
			velocity_.y += acceleration.y * deltaTime;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		}
		else {
			velocity_.x *= (1.0f - kAttenu);
		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y += kJumpAcceleration;
		}
	}
	else {
		if (laning) {
			worldTransform_.translation_.y = 2.0f;
			velocity_.x *= (1.0f - kAttenu);
			velocity_.y = 0.0f;
			onGround_ = true;
		}

		velocity_.y += -kGravity;
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}

	CollisionMapInfo collisionMapInfo{};
	collisionMapInfo.movement.x = velocity_.x * deltaTime;
	collisionMapInfo.movement.y = velocity_.y * deltaTime;
	collisionMapInfo.movement.z = velocity_.z * deltaTime;

	MapCollisionDetection(collisionMapInfo);
	ApplyCollision(collisionMapInfo);
	ProcessCeilingHit(collisionMapInfo);
	groundStateSwiching(collisionMapInfo);

	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		behaviorRequest_ = Behavior::kAttack;
	}
}

void Player::BehaviorAttackInitialize() {
	attackPhase_ = AttackPhase::kCharge;
	attackParameter_ = 0;
	velocity_.x = 0.0f;
}

void Player::BehaviorAttackUpdate() {
	Vector3 attackVelocity{};

	if (!onGround_) {
		velocity_.y += -kGravity;
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}

	switch (attackPhase_) {
	case AttackPhase::kCharge:
	default: {
		const float t = std::clamp(static_cast<float>(attackParameter_) / static_cast<float>(kAttackChargeTime), 0.0f,
		                           1.0f);
		worldTransform_.scale_.z = std::lerp(2.0f, 0.6f, t);
		worldTransform_.scale_.y = std::lerp(2.0f, 1.6f, t);
		if (attackParameter_++ >= kAttackChargeTime) {
			attackPhase_ = AttackPhase::kDash;
			attackParameter_ = 0;
		}
		break;
	}
	case AttackPhase::kDash: {
		const float t = std::clamp(static_cast<float>(attackParameter_) / static_cast<float>(kAttackDashTime), 0.0f,
		                           1.0f);
		worldTransform_.scale_.z = std::lerp(0.6f, 2.2f, t);
		worldTransform_.scale_.y = std::lerp(1.6f, 1.8f, t);
		attackVelocity.x = (lrDirection == LRDirection::kRigh ? +1.0f : -1.0f) * kAttackSpeed;
		if (attackParameter_++ >= kAttackDashTime) {
			attackPhase_ = AttackPhase::kRecovery;
			attackParameter_ = 0;
		}
		break;
	}
	case AttackPhase::kRecovery: {
		const float t = std::clamp(static_cast<float>(attackParameter_) / static_cast<float>(kAttackRecoveryTime), 0.0f,
		                           1.0f);
		worldTransform_.scale_.z = std::lerp(2.2f, 2.0f, t);
		worldTransform_.scale_.y = std::lerp(1.8f, 2.0f, t);
		if (attackParameter_++ >= kAttackRecoveryTime) {
			worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
			behaviorRequest_ = Behavior::kRoot;
		}
		break;
	}
	}

	CollisionMapInfo collisionMapInfo{};
	collisionMapInfo.movement.x = attackVelocity.x * kDeltaTime;
	collisionMapInfo.movement.y = velocity_.y * kDeltaTime;
	collisionMapInfo.movement.z = 0.0f;

	MapCollisionDetection(collisionMapInfo);
	ApplyCollision(collisionMapInfo);
	ProcessCeilingHit(collisionMapInfo);
	groundStateSwiching(collisionMapInfo);
}

void Player::Update() {
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_,
	                                             worldTransform_.translation_);
	if (isKnockBackRequested_) {
		behaviorRequest_ = Behavior::kKnockBack;
		isKnockBackRequested_ = false;
	}
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kKnockBack:
			BehaviorKnockBackInitialize();
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kKnockBack:
		BehaviorKnockBackUpdate();
		break;
	}

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_,
	                                             worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

const WorldTransform& Player::GetWorldTransform() const { return worldTransform_; }

void Player::MapCollisionDetection(CollisionMapInfo& info) {
	MapCollisionUp(info);
	MapCollisionDown(info);
	MapCollisionRight(info);
	MapCollisionLeft(info);
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorners] = {
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

void Player::MapCollisionUp(CollisionMapInfo& info) {
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	if (info.movement.y <= 0) {
		return;
	}

	bool hit = false;
	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopRight]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float playerY = worldTransform_.translation_.y;
		info.movement.y = rect.bottom - (playerY + kHeight / 2.0f) - kBlank;
		info.ceiling = true;
	}
}

void Player::MapCollisionDown(CollisionMapInfo& info) {
	if (info.movement.y >= 0) {
		return;
	}

	std::array<Vector3, 4> positionsNow;
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNow.size(); i++) {
		positionsNow[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	float yMoveToLand = info.movement.y;
	const float playerBottom = worldTransform_.translation_.y - kHeight / 2.0f;
	bool hit = false;

	auto TryLandingCandidate = [&](int32_t xIndex, uint32_t yIndex, const Vector3& currentCorner,
	                               const Vector3& nextCorner) {
		if (xIndex < 0) {
			return false;
		}

		const uint32_t candidateX = static_cast<uint32_t>(xIndex);
		if (mapChipField_->GetMapChipTypeByIndex(candidateX, yIndex) != MapChipType::kBlock) {
			return false;
		}

		const MapChipField::Rect rect = mapChipField_->GetRectByIndex(candidateX, yIndex);
		if (currentCorner.y < rect.top || rect.top < nextCorner.y) {
			return false;
		}

		yMoveToLand = (std::max)(yMoveToLand, rect.top - playerBottom + kBlank);
		hit = true;
		return true;
	};

	auto CheckLanding = [&](Corner corner, int32_t adjacentXOffset) {
		const Vector3& currentCorner = positionsNow[corner];
		const Vector3& nextCorner = positionsNew[corner];

		const MapChipField::IndexSet currentIndex = mapChipField_->GetMapChipIndexSetByPosition(currentCorner);
		const MapChipField::IndexSet nextIndex = mapChipField_->GetMapChipIndexSetByPosition(nextCorner);

		// 下方向にセル境界をまたいだときだけ着地を判定する。
		if (currentIndex.yIndex == nextIndex.yIndex) {
			return;
		}

		if (TryLandingCandidate(static_cast<int32_t>(nextIndex.xIndex), nextIndex.yIndex, currentCorner, nextCorner)) {
			return;
		}
		if (TryLandingCandidate(static_cast<int32_t>(nextIndex.xIndex) + adjacentXOffset, nextIndex.yIndex,
		                        currentCorner, nextCorner)) {
			return;
		}
		if (TryLandingCandidate(static_cast<int32_t>(nextIndex.xIndex), nextIndex.yIndex + 1, currentCorner,
		                        nextCorner)) {
			return;
		}
		TryLandingCandidate(static_cast<int32_t>(nextIndex.xIndex) + adjacentXOffset, nextIndex.yIndex + 1,
		                    currentCorner, nextCorner);
	};

	CheckLanding(kBottomLeft, +1);
	CheckLanding(kBottomRight, -1);

	if (!hit) {
		return;
	}

	info.movement.y = yMoveToLand;
	info.Landing = true;
}

void Player::MapCollisionRight(CollisionMapInfo& info) {
	if (info.movement.x <= 0) {
		return;
	}

	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	MapChipField::IndexSet topIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopRight]);
	MapChipField::IndexSet bottomIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kBottomRight]);

	bool hitTop = mapChipField_->GetMapChipTypeByIndex(topIndex.xIndex, topIndex.yIndex) == MapChipType::kBlock;
	bool hitBottom = mapChipField_->GetMapChipTypeByIndex(bottomIndex.xIndex, bottomIndex.yIndex) ==
		MapChipType::kBlock;
	if (!hitTop && !hitBottom) {
		return;
	}

	float playerRight = worldTransform_.translation_.x + kWidth / 2.0f;
	float xMoveToWall = info.movement.x;

	if (hitTop) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(topIndex.xIndex, topIndex.yIndex);
		xMoveToWall = (std::min)(xMoveToWall, rect.left - playerRight - kBlank);
	}
	if (hitBottom) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(bottomIndex.xIndex, bottomIndex.yIndex);
		xMoveToWall = (std::min)(xMoveToWall, rect.left - playerRight - kBlank);
	}

	info.movement.x = xMoveToWall;
	info.wall = true;
	velocity_.x = 0.0f;
}

void Player::MapCollisionLeft(CollisionMapInfo& info) {
	if (info.movement.x >= 0) {
		return;
	}

	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	MapChipField::IndexSet topIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);
	MapChipField::IndexSet bottomIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kBottomLeft]);

	bool hitTop = mapChipField_->GetMapChipTypeByIndex(topIndex.xIndex, topIndex.yIndex) == MapChipType::kBlock;
	bool hitBottom = mapChipField_->GetMapChipTypeByIndex(bottomIndex.xIndex, bottomIndex.yIndex) ==
		MapChipType::kBlock;
	if (!hitTop && !hitBottom) {
		return;
	}

	float playerLeft = worldTransform_.translation_.x - kWidth / 2.0f;
	float xMoveToWall = info.movement.x;

	if (hitTop) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(topIndex.xIndex, topIndex.yIndex);
		xMoveToWall = (std::max)(xMoveToWall, rect.right - playerLeft + kBlank);
	}
	if (hitBottom) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(bottomIndex.xIndex, bottomIndex.yIndex);
		xMoveToWall = (std::max)(xMoveToWall, rect.right - playerLeft + kBlank);
	}

	info.movement.x = xMoveToWall;
	info.wall = true;
	velocity_.x = 0.0f;
}

void Player::groundStateSwiching(const CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
			return;
		}

		Vector3 footOffset = {0.0f, -kGroundProbeDepth, 0.0f};
		Vector3 leftBottom = Add(CornerPosition(worldTransform_.translation_, kBottomLeft), footOffset);
		Vector3 rightBottom = Add(CornerPosition(worldTransform_.translation_, kBottomRight), footOffset);

		MapChipField::IndexSet leftIndex = mapChipField_->GetMapChipIndexSetByPosition(leftBottom);
		MapChipField::IndexSet rightIndex = mapChipField_->GetMapChipIndexSetByPosition(rightBottom);
		bool hitLeft = mapChipField_->GetMapChipTypeByIndex(leftIndex.xIndex, leftIndex.yIndex) == MapChipType::kBlock;
		bool hitRight = mapChipField_->GetMapChipTypeByIndex(rightIndex.xIndex, rightIndex.yIndex) ==
			MapChipType::kBlock;

		if (!hitLeft && !hitRight) {
			onGround_ = false;
		}
		else {
			velocity_.y = 0.0f;
		}
	}
	else {
		if (info.Landing) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}

Vector3 Player::GetWorldPosition() const { return worldTransform_.translation_; }

AABB Player::GetAABB() const {
	const Vector3 center = GetWorldPosition();
	constexpr Vector3 half = {kWidth * 0.5f, kHeight * 0.5f, kWidth * 0.5f};

	AABB aabb;
	aabb.min = {center.x - half.x, center.y - half.y, center.z - half.z};
	aabb.max = {center.x + half.x, center.y + half.y, center.z + half.z};
	return aabb;
}

void Player::EnemyOnCollision(const Enemy* enemy) {
	(void)enemy;
	if (IsAttack()) {
		return;
	}
	isDead_ = true;
}

void Player::ShieldEnemyOnCollision(const ShieldEnemy* shieldEnemy) {
	(void)shieldEnemy;
	if (IsAttack()) {
		return;
	}
	isDead_ = true;
}

void Player::RequestKnockBack() { isKnockBackRequested_ = true; }

void Player::BehaviorKnockBackInitialize() {
	attackParameter_ = 0;
	velocity_.x = 0.0f;
}

void Player::BehaviorKnockBackUpdate() {
	attackParameter_++;

	Vector3 knockBackVelocity{};

	knockBackVelocity.x = (lrDirection == LRDirection::kRigh ? -1.0f : 1.0f) * kAttackSpeed;

	CollisionMapInfo collisionMapInfo{};
	collisionMapInfo.movement.x = knockBackVelocity.x * kDeltaTime;
	collisionMapInfo.movement.y = velocity_.y * kDeltaTime;
	collisionMapInfo.movement.z = 0.0f;

	MapCollisionDetection(collisionMapInfo);
	ApplyCollision(collisionMapInfo);
	ProcessCeilingHit(collisionMapInfo);
	groundStateSwiching(collisionMapInfo);

	if (attackParameter_ >= kAttackRecoveryTime) {
		behaviorRequest_ = Behavior::kRoot;
	}
}
