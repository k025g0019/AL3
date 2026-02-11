#pragma once
#include "KamataEngine.h"

class GameScene {

public:
	uint32_t textureHandle_ = 0; // テクスチャハンドル

	// 3Dモデルのハンドル
	KamataEngine::Model* model_ = nullptr;

	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Camera camera_;

	KamataEngine::DebugCamera* debugCamera_ = nullptr;



	

	GameScene();

	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();
};