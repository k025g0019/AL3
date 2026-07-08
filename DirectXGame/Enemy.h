#pragma once
#include  <list>
#include "EnemyBullet.h"
#include "KamataEngine.h"
#include "worldTransform.h"
class EnemyBullet;

enum class Phase {
	Approach, // 接近
	Leave, // 攻撃
};

class Enemy {
public:
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle);

	void Update();
	void Draw(const KamataEngine::Camera& camera);
	const KamataEngine::Vector3& GetWorldPosition() const { return worldTransform_.translation_; }
	Phase phase_ = Phase::Approach;
	std::list<EnemyBullet*> bullets_;
	EnemyBullet* bullet_ = nullptr;
	~Enemy();

	void approachPhase();
	static constexpr int kFireInterval = 60; // 弾発射間隔（フレーム）
private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	uint32_t textureHandle_ = 0u;
	// 移動ベクトル
	KamataEngine::Vector3 move_ = {0.0f, 0.0f, 0.0f};
	// 定数
	static constexpr float kMoveSpeed = 0.1f;
	//弾発射
	void Fire();

	int32_t fireTimer = 0;
};
