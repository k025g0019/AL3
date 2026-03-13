#pragma once

#include <3d/Camera.h>

class Player;

//====================
// カメラ制御
//====================
/*
プレイヤーを追従するカメラ制御クラス
*/
class CameraController {
public:
	//====================
	// 関数
	//====================
	/* 初期化 */
	void Initialize(); // 初期値設定

	/* 更新 */
	void Update(); // 毎フレーム更新

	/* リセット */
	void Reset(); // 即時リセット

	//====================
	// 変数
	//====================
	/* 追従対象 */
	Player* target_ = nullptr;

	/* カメラ参照設定 */
	void SetTarget(Player* target) { target_ = target; }                 // 追従対象を設定
	void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }   // 制御対象カメラを設定

	/* 追従オフセット */
	KamataEngine::Vector3 targetOffset_ = {0.0f, 0.0f, -25.0f};

	/* 可動範囲構造体 */
	struct Rect {
		float left = 0.0f;
		float top = 1.0f;
		float right = 10.0f;
		float bottom = 0.0f;
	};

	/* カメラ可動範囲 */
	Rect movableArea_ = {0.0f, 100.0f, 0.0f, 100.0f};
	void SetMovableArea(const Rect& area) { movableArea_ = area; } // カメラ可動範囲を設定

	/* 現在の追従目標位置 */
	KamataEngine::Vector3 targetPosition_ = {};

	//====================
	// 定数
	//====================
	/* 補間率 */
	static inline const float kInterpolationRate = 0.1f; // カメラ追従の補間率

	/* 速度先読み係数 */
	static inline const float kVelocityBias = 0.5f; // プレイヤー速度の先読み係数

	/* 追従許容マージン */
	static inline const Rect margin = {
		.left = -10.0f,
		.top = 10.0f,
		.right = 10.0f,
		.bottom = -10.0f,
	};

private:
	KamataEngine::Camera* camera_ = nullptr; // 制御するカメラ
};
