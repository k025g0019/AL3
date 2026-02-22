#pragma once
#include "KamataEngine.h"
#include <vector>

class GameScene {

public:
	uint32_t textureHandle_ = 0; // テクスチャハンドル

	// 3Dモデルのハンドル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;

	KamataEngine::WorldTransform worldTransform;

	KamataEngine::Camera camera_;

	KamataEngine::DebugCamera* debugCamera_ = nullptr;
		
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	
	bool isDebugCameraActive = false;
	GameScene();

	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();
};