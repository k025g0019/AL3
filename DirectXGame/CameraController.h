#pragma once

#include <3d/Camera.h>

class Player;

class CameraController {

	
public:
	enum class Mode {
		kFollow,
		kForcedScroll,
	};

	void SetMode(Mode mode) { mode_ = mode; }

	Mode GetMode() const { return mode_; }

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
	void SetForcedScrollSpeed(float speed) { forcedScrollSpeed_ = speed; }
	float GetForcedScrollSpeed() const { return forcedScrollSpeed_; }
	void SetForcedScrollStopX(float stopX) { forcedScrollStopX_ = stopX; }
	float GetForcedScrollStopX() const { return forcedScrollStopX_; }

	float GetViewHalfHeight(float worldZ = 0.0f) const;
	float GetViewHalfWidth(float worldZ = 0.0f) const;
	float GetLeftEdge(float worldZ = 0.0f) const;
	float GetRightEdge(float worldZ = 0.0f) const;

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
	Mode mode_ = Mode::kFollow;
	float forcedScrollSpeed_ = 0.0f;
	float forcedScrollStopX_ = 100000.0f;
};
