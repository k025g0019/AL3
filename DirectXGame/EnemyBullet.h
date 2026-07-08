#pragma once
#include "KamataEngine.h"
#include <algorithm>
#include <math/MathUtility.h>

class EnemyBullet {
public:
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity);

	void Update();

	void Draw(const KamataEngine::Camera& camera);

	// 速度
	KamataEngine::Vector3 velocity_;

	static constexpr int32_t kLifeTime = 60 * 5;

	// タイマー
	int32_t deathTimer_ = kLifeTime;

	bool isDead_ = false;

	bool IsDead() const { return isDead_; }

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	uint32_t textureHandle_ = 0u;
};
