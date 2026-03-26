#pragma once

#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "HitEffect.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Matrix4x4.h"
#include "Player.h"

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
	KamataEngine::Model* hitEffectModel_ = nullptr;
	KamataEngine::Sprite* sprite_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	Player* player_ = nullptr;
	std::vector<Enemy*> enemies_;
	std::vector<HitEffect*> hitEffects_;
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

	void GenerateBlocks();
	void CheckAllCollisions();
	void CreateHitEffect(const KamataEngine::Vector3& position);

private:
	void UpdatePlayPhase();
	void UpdateDeathPhase();
	void ChangePhase();
	void UpdateBlockMatrices();
	void RemoveDeadEnemies();
	void RemoveDeadHitEffects();

private:
	static inline const float kFadeDuration = 1.0f;
	Phase phase_ = Phase::kFadeIn;
	bool finished_ = false;
};
