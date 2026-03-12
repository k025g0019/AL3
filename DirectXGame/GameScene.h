#pragma once

#include "CameraController.h"
#include "Enemy.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Matrix4x4.h"
#include "Player.h"

// ゲーム全体の進行を管理するシーン
class GameScene {
public:
	uint32_t textureHandle_ = 0;

	// 描画モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;

	KamataEngine::Sprite* sprite_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	CameraController* cameraController_ = nullptr;
	MapChipField* mapChipField_ = nullptr;

	// マップチップからブロックを生成
	void GenerateBlocks();

	GameScene();
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	~GameScene();

	void Initialize();
	void Update();
	void Draw();
};
