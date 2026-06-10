#pragma once

#include "KamataEngine.h"

#include <array>

class GuardEffect {
public:
	enum class State {
		kSpread,
		kFade,
		kDead,
	};

	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }
	static GuardEffect* Create(const KamataEngine::Vector3& position);

	GuardEffect() = default;
	~GuardEffect() = default;

	void Initialize(const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	bool IsDead() const { return state_ == State::kDead; }

private:
	static KamataEngine::Model* model_;
	static KamataEngine::Camera* camera_;

	KamataEngine::ObjectColor objectColor_;
	KamataEngine::Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

	KamataEngine::WorldTransform circleWorldTransform_;
	std::array<KamataEngine::WorldTransform, 2> ellipseWorldTransforms_;

	State state_ = State::kSpread;
	float counter_ = 0.0f;
	float spreadDuration_ = 0.1f;
	float fadeDuration_ = 0.2f;
};
