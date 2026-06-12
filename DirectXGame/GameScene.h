#pragma once

#include "Enemy.h"
#include "KamataEngine.h"
#include "Player.h"

class GameScene {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	GameScene();
	~GameScene();

private:
	// テクスチャハンドル
	uint32_t playertextureHandle_ = 0u;
	uint32_t enemytextureHandle_ = 0u;
	// メンバ変数
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::Model* enemyModel_ = nullptr;
	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;

	// デバッグカメラ切り替え用フラグ
	bool isDebugCameraActive_ = false;
	bool isInitialized_ = false;
};
