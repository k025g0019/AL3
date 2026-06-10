#pragma once

#include <3d/WorldTransform.h>
#include "AABB.h"
#include "HitEffect.h"
#include "KamataEngine.h"
#include "Matrix4x4.h"

class Player;
class GameScene;

class ShieldEnemy {
public:
	enum class Behavior {
		kUnknown,
		kWalk,
		kDead,
	};


	static constexpr float kWalSpeed = 0.1f;
	KamataEngine::Vector3 velocity_ = {};

	static constexpr float kWalkMotionAngleStart = 0.0f;
	static constexpr float kWalkMotionAngleEnd = 30.0f;
	static constexpr float kWalkMotionTime = 0.5f;
	float walkTimer_ = 0.0f;

	static constexpr float kWidth = 2.0f;
	static constexpr float kHeight = 2.0f;


	ShieldEnemy();
	~ShieldEnemy();

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	KamataEngine::Vector3 GetWorldPosition() const;
	AABB GetAABB() const;
	void OnCollision(Player* player);
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	bool IsDead() const { return isDead_; }
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

private:
	void BehaviorWalkInitialize();
	void BehaviorWalkUpdate();
	void BehaviorDeadInitialize();
	void BehaviorDeadUpdate();

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	GameScene* gameScene_ = nullptr;
	HitEffect* HitEffect_;
	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnknown;
	bool isDead_ = false;
	bool isCollisionDisabled_ = false;
	float deathTimer_ = 0.0f;

	static constexpr float kDeathDuration = 0.6f;
};
