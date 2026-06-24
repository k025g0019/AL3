#pragma once

#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"

#include <vector>

class GameScene {
public:
	uint32_t textureHandle_ = 0u;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Sprite* sprite_ = nullptr;

	KamataEngine::WorldTransform worldTransform;
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	Skydome* skydome = nullptr;
	bool isDebugCameraActive = false;
	Player* player_ = nullptr;

	GameScene();
	~GameScene();

	void Initialize();
	void Update();
	void Draw();
};
