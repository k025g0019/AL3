#pragma once

#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Matrix4x4.h"
#include "Player.h"

//====================
// ゲームシーン
//====================
/*
ゲーム全体の進行を管理する
*/
class GameScene {
public:
	enum class Phase {
		kPlay,
		kDeath,
	};

	//====================
	// 変数
	//====================
	// 使用テクスチャID
	uint32_t textureHandle_ = 0;

	/* 描画モデル */
	KamataEngine::Model* model_ = nullptr;       // 汎用モデル
	KamataEngine::Model* modelBlock_ = nullptr;  // ブロック描画モデル
	KamataEngine::Model* playerModel_ = nullptr; // プレイヤー/敵で使うモデル
	KamataEngine::Sprite* sprite_ = nullptr;     // 2D描画用（未使用）

	KamataEngine::WorldTransform worldTransform_;     // シーン基準変換
	KamataEngine::Camera camera_;                     // 本番カメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr; // デバッグカメラ

	Player* player_ = nullptr;                     // プレイヤー本体
	std::vector<Enemy*> enemies_;                  // 敵配列
	DeathParticles* deathParticles_ = nullptr;     // デスパーティクル
	CameraController* cameraController_ = nullptr; // カメラ追従制御
	MapChipField* mapChipField_ = nullptr;         // マップチップ情報

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_; // ブロック用変換配列

	//====================
	// 関数
	//====================
	/* 生成 */ GameScene();
	/* 破棄 */ ~GameScene();

	/* 初期化 */ void Initialize();
	/* 更新 */ void Update();
	/* 描画 */ void Draw();
	bool IsFinished() const { return finished_; }

	/* ブロック生成 */ void GenerateBlocks();
	/* 当たり判定 */ void CheckAllCollisions();

private:
	void UpdatePlayPhase();
	void UpdateDeathPhase();
	void ChangePhase();
	void UpdateBlockMatrices();

private:
	Phase phase_ = Phase::kPlay;
	bool finished_ = false;
};
