#pragma once

#include <3d/Camera.h>

class Player;

// プレイヤーを追従する 2D 横スクロール向けカメラ制御
class CameraController {
public:
	// 初期状態を設定する
	void Initialize();

	// ターゲット位置へ補間しながら更新する
	void Update();

	// 追従対象
	Player* target_ = nullptr;
	void SetTarget(Player* target) { target_ = target; }

	// カメラをターゲット位置へ即座に合わせる
	void Reset();

	// ターゲットからの相対オフセット
	KamataEngine::Vector3 targetOffset_ = {0, 0, -25.0f};
	void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	struct Rect {
		float left = 0.0f;
		float top = 1.0f;
		float right = 10.0f;
		float bottom = 0.0f;
	};

	// カメラ移動の絶対制限領域
	Rect movableArea_ = {0, 100, 0, 100};
	void SetMovableArea(const Rect& area) { movableArea_ = area; }

	// 実際に追いかける目標位置
	KamataEngine::Vector3 targetPosition_ = {};

	// 補間率
	static inline const float kInterpolationRate = 0.1f;
	// プレイヤー速度を先読みする係数
	static inline const float kVelocityBias = 0.5f;

	// ターゲット周囲で許可する追従マージン
	static inline const Rect margin = {
		.left = -10.0f,
		.top = 10.0f,
		.right = 10.0f,
		.bottom = -10.0f,
	};

private:
	KamataEngine::Camera* camera_ = nullptr;
};
