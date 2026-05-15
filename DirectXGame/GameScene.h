#pragma once
#include "KamataEngine.h"

class GameScene {
public:
	uint32_t textureHandle_ = 0; // テクスチャハンドル
	uint32_t soundDataHandle_ = 0;

	// 3Dモデルのハンドル
	KamataEngine::Model* model_ = nullptr;

	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Camera camera_;

	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	KamataEngine::Vector2 spriteVelocity_ = {2.0f, 1.0f};
	bool isInitialized_ = false;

	GameScene();

	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();
};
