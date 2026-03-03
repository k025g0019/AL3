#pragma once

#include <3d/Camera.h>

class Player;

class CameraController {

public:
	// <summary>
	// 初期化
	// </summary>
	void Initialize();

	// <summary>
	// 更新
	// </summary>
	void Update();

	// 目標プレイヤー
	Player* target_ = nullptr;

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	// 追従対象とカメラの座標の差
	KamataEngine::Vector3 targetOffset_ = {0, 0, -30.0f};
	void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	struct Rect {
		float left = 0.0f;
		float top = 1.0f;
		float right = 10.0f;
		float bottom = 0.0f;
	};

	Rect movableArea_ = {0, 100, 0, 100};

	void SetMovableArea(const Rect& area) { movableArea_ = area; }

	// カメラの目標座標
	KamataEngine::Vector3 targetPosition_ = {};

	static inline const float kInterpolationRate = 0.1f;

	static inline const float kVelocityBias = 0.5f;

	static inline const Rect margin = {
		.left = -10.0f,
		.top = 10.0f,
		.right = 10.0f,
		.bottom = -10.0f,
	};

private:
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
};
