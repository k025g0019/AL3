#pragma once

#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "GuardEffect.h"
#include "HitEffect.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Matrix4x4.h"
#include "Player.h"
#include "ShieldEnemy.h"

class GameScene {
public:
	enum class Phase {
		kFadeIn,
		kPlay,
		kDeath,
		kFadeOut,
	};

	uint32_t textureHandle_ = 0;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::Model* HitEffectModel_ = nullptr;
	KamataEngine::Sprite* sprite_ = nullptr;
	KamataEngine::Model* modelEnemy_ = nullptr;
	KamataEngine::Model* modelShieldEnemy_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	KamataEngine::Model* modelBlocks_ = nullptr;
	KamataEngine::Model* guardEffectModel_ = nullptr;

	Player* player_ = nullptr;
	std::vector<Enemy*> enemies_;
	std::vector<ShieldEnemy*> shieldEnemies_;
	std::vector<HitEffect*> HitEffects_;
	std::vector<GuardEffect*> guardEffects_;
	DeathParticles* deathParticles_ = nullptr;
	CameraController* cameraController_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	Fade* fade_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	GameScene();
	~GameScene();

	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }
	bool IsReloadRequested() const { return reloadRequested_; }
	void CreateGunEffect(const KamataEngine::Vector3& position);

	void GenerateFieldObjects();
	void CheckAllCollisions();
	void CreateHitEffect(const KamataEngine::Vector3& position);

private:
	void UpdatePlayPhase();
	void UpdateDeathPhase();
	void ChangePhase();
	void UpdateBlockMatrices();
	void RemoveDeadEnemies();
	void RemoveDeadHitEffects();
	void RemoveDeadShieldEnemies();
	static constexpr float kFadeDuration = 1.0f;
	Phase phase_ = Phase::kFadeIn;
	bool finished_ = false;
	bool reloadRequested_ = false;
};
