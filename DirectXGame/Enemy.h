#pragma once

#include "AABB.h"
#include "KamataEngine.h"

#include <3d/WorldTransform.h>

class Player;
class GameScene;

class Enemy {
public:
	enum class Behavior {
		kUnknown,
		kWalk,
		kDead,
	};

	static inline const float kWalSpeed = 0.1f;
	KamataEngine::Vector3 velocity_ = {};

	static inline const float kWalkMotionAngleStart = 0.0f;
	static inline const float kWalkMotionAngleEnd = 30.0f;
	static inline const float kWalkMotionTime = 0.5f;
	float walkTimer_ = 0.0f;

	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;

	Enemy();
	~Enemy();

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	KamataEngine::Vector3 GetWorldPosition() const;
	AABB GetAABB() const;
	void OnCollision(const Player* player);
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	bool IsDead() const { return isDead_; }
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

private:
	void BehaviorWalkInitialize();
	void BehaviorWalkUpdate();
	void BehaviorDeadInitialize();
	void BehaviorDeadUpdate();

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	GameScene* gameScene_ = nullptr;

	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnknown;
	bool isDead_ = false;
	bool isCollisionDisabled_ = false;
	float deathTimer_ = 0.0f;

	static inline const float kDeathDuration = 0.6f;
};
