#pragma once
#include <3d/WorldTransform.h>
#include "KamataEngine.h"
class Player {

public:
	KamataEngine::Camera* camera_ = nullptr;
	Player();
	~Player();
	// 初期化
	void Initialize(KamataEngine::Model* model,uint32_t textureHandle,KamataEngine::Camera* camera);
	// 更新
	void Update();
	// 描画
	void Draw();
private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
};
