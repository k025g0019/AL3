#pragma once

#include "AABB.h"
#include "KamataEngine.h"

#include <3d/WorldTransform.h>

class Player;

//====================
// 敵
//====================
/*
敵キャラクター（簡易版）
*/
class Enemy {
public:
	//====================
	// 変数
	//====================
	static inline const float kWalSpeed = 0.1f; // 敵の移動速度
	KamataEngine::Vector3 velocity_ = {};        // 現在速度

	//====================
	// 定数
	//====================
	/* 歩行アニメーション */
	static inline const float kWalkMotionAngleStart = 0.0f;  // 首振り開始角度[度]
	static inline const float kWalkMotionAngleEnd = 30.0f;   // 首振り終了角度[度]
	static inline const float kWalkMotionTime = 0.5f;        // 首振り1周期の時間[秒]
	float walkTimer_ = 0.0f;                                  // 歩行アニメーション進行時間

	/* 当たり判定サイズ */
	static inline const float kWidth = 2.0f;                 // 敵AABBの幅
	static inline const float kHeight = 2.0f;                // 敵AABBの高さ

	//====================
	// 関数
	//====================
	/* 生成 */ Enemy();
	/* 破棄 */ ~Enemy();

	/* 初期化 */ void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	/* 更新 */ void Update();
	/* 描画 */ void Draw();

	/* 座標取得 */ KamataEngine::Vector3 GetWorldPosition() const;
	/* AABB取得 */ AABB GetAABB() const;
	/* 衝突時処理 */ void OnCollision(const Player* player);

private:
	KamataEngine::WorldTransform worldTransform_; // 敵のワールド変換
	KamataEngine::Model* model_ = nullptr;        // 描画モデル参照
	KamataEngine::Camera* camera_ = nullptr;      // 描画カメラ参照
};
