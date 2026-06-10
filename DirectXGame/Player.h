#pragma once

#include "AABB.h"
#include "KamataEngine.h"
#include "Matrix4x4.h"

#include <3d/WorldTransform.h>

#include  "Enemy.h"
#include "ShieldEnemy.h"
class Enemy;
class MapChipField;

class Player {
public:
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Vector3 velocity_ = {};

	enum class LRDirection {
		kRigh,
		KLeft,
	};

	enum class Behavior {
		kUnknown,
		kRoot,
		kAttack,
		kKnockBack,
	};

	enum class AttackPhase {
		kCharge,
		kDash,
		kRecovery,
	};

	enum Corner {
		kTopLeft,
		kTopRight,
		kBottomLeft,
		kBottomRight,
		kNumCorners,
	};

	struct CollisionMapInfo {
		bool ceiling = false;
		bool Landing = false;
		bool wall = false;
		KamataEngine::Vector3 movement;
	};

	LRDirection lrDirection = LRDirection::kRigh;
	float turnFirstRotationY = 0.0f;
	float turnTimer_ = 0.0f;
	bool onGround_ = true;

	static constexpr float kGravity = 9.8f;
	static constexpr float kLimitFallSpeed = 10.0f;
	static constexpr float kJumpAcceleration = 105.0f;
	static constexpr float kTimeTurn = 0.3f;
	static constexpr float kAttenu = 0.1f;
	static constexpr float kLimitRunSpeed = 80.0f;
	static constexpr float kAttenuationLanding = 0.5f;

	static constexpr float kWidth = 2.0f;
	static constexpr float kHeight = 2.0f;
	static constexpr float kBlank = 0.01f;
	static constexpr float kGroundProbeDepth = 0.05f;

	Player();
	~Player();

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
	void BehaviorKnockBackInitialize();
	void BehaviorKnockBackUpdate();
	void BehaviorRootInitialize();
	void BehaviorRootUpdate();
	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();

	const KamataEngine::WorldTransform& GetWorldTransform() const;
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	bool IsDead() const { return isDead_; }
	bool IsAttack() const { return behavior_ == Behavior::kAttack; }

	void MapCollisionUp(CollisionMapInfo& info);
	void MapCollisionDown(CollisionMapInfo& info);
	void MapCollisionRight(CollisionMapInfo& info);
	void MapCollisionLeft(CollisionMapInfo& info);
	void MapCollisionDetection(CollisionMapInfo& info);
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);
	void ApplyCollision(const CollisionMapInfo& info);
	void ProcessCeilingHit(const CollisionMapInfo& info);
	void groundStateSwiching(const CollisionMapInfo& info);
	void RequestKnockBack();

	KamataEngine::Vector3 GetWorldPosition() const;
	AABB GetAABB() const;
	void EnemyOnCollision(const Enemy* enemy);
	void ShieldEnemyOnCollision(const ShieldEnemy* shieldEnemy);
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
	MapChipField* mapChipField_ = nullptr;
	bool isDead_ = false;

	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;
	AttackPhase attackPhase_ = AttackPhase::kCharge;
	uint32_t attackParameter_ = 0;

	LRDirection GettrLR() const { return lrDirection; }

private:
	static constexpr float kDeltaTime = 1.0f / 60.0f;
	static constexpr uint32_t kAttackChargeTime = 10;
	static constexpr uint32_t kAttackDashTime = 8;
	static constexpr uint32_t kAttackRecoveryTime = 12;
	static constexpr float kAttackSpeed = 120.0f;
	bool isKnockBackRequested_ = false;
};
