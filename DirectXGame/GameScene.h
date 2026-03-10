#pragma once
#include "CameraController.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Matrix4x4.h"
#include "Player.h"

class GameScene {

public:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;

	// 2Dスプライト（現在は未使用）
	KamataEngine::Sprite* sprite_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	Player* player_ = nullptr;
	CameraController* cameraController_;
	MapChipField* mapChipField_;

	// ブロック配置を生成
	void GenerateBlocks();

	GameScene();
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();
};