#pragma once

#include "KamataEngine.h"

#include <3d/WorldTransform.h>
#include <array>

class Player;

//====================
// デスパーティクル
//====================
/*
プレイヤー撃破時の破片エフェクト
*/
class DeathParticles {
public:
	//====================
	// 関数
	//====================
	/* 生成 */
	DeathParticles() = default;

	/* 破棄 */
	~DeathParticles() = default;

	/* 初期化（Player と同じ引数構成） */
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	/* 更新 */
	void Update();

	/* 描画 */
	void Draw();

	/* 終了状態取得 */
	bool IsFinished() const { return isFinished_; }

private:
	//====================
	// 定数
	//====================
	static inline const uint32_t kNumParticles = 8;                                                    // パーティクル個数
	static inline const float kDuration = 1.0f;                                                        // 存続時間[秒]
	static inline const float kSpeed = 0.2f;                                                          // 移動速度
	static inline const float kAngleUnit = 2.0f * 3.1415926535f / static_cast<float>(kNumParticles); // 1分割の角度
	static inline const float kStartScale = 0.8f;                                                      // 初期スケール

	//====================
	// 変数
	//====================
	KamataEngine::Model* model_ = nullptr;    // 描画モデル
	KamataEngine::Camera* camera_ = nullptr;  // 描画カメラ
	KamataEngine::ObjectColor objectColor_;   // 色変更オブジェクト
	KamataEngine::Vector4 color_ = {};        // 現在色(RGBA)
	bool isFinished_ = false;                 // 終了フラグ
	float counter_ = 0.0f;                    // 経過時間

	//====================
	// 配列
	//====================
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_; // 各パーティクルの変換
};
