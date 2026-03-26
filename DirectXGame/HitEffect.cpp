#include "HitEffect.h"

#include <algorithm>
#include <cassert>
#include <numbers>
#include <random>

#include "Matrix4x4.h"

using namespace KamataEngine;

Model* HitEffect::model_ = nullptr;
Camera* HitEffect::camera_ = nullptr;

HitEffect* HitEffect::Create(const KamataEngine::Vector3& position) {
	HitEffect* instance = new HitEffect();
	assert(instance);
	instance->Initialize(position);
	return instance;
}

void HitEffect::Initialize(const KamataEngine::Vector3& position) {
	objectColor_.Initialize();
	color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	objectColor_.SetColor(color_);

	state_ = State::kSpread;
	counter_ = 0.0f;

	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.scale_ = {0.2f, 0.2f, 1.0f};
	circleWorldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	static std::random_device seedGenerator;
	static std::mt19937_64 randomEngine(seedGenerator());
	std::uniform_real_distribution<float> rotationDistribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
		worldTransform.scale_ = {0.15f, 1.2f, 1.0f};
		worldTransform.rotation_ = {0.0f, 0.0f, rotationDistribution(randomEngine)};
	}
}

void HitEffect::Update() {
	if (state_ == State::kDead) {
		return;
	}

	const float kDeltaTime = 1.0f / 60.0f;
	counter_ += kDeltaTime;

	switch (state_) {
	case State::kSpread: {
		const float t = std::clamp(counter_ / spreadDuration_, 0.0f, 1.0f);
		circleWorldTransform_.scale_.x = std::lerp(0.2f, 1.6f, t);
		circleWorldTransform_.scale_.y = std::lerp(0.2f, 1.6f, t);
		for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
			worldTransform.scale_.x = std::lerp(0.15f, 0.05f, t);
			worldTransform.scale_.y = std::lerp(1.2f, 2.0f, t);
		}

		if (counter_ >= spreadDuration_) {
			state_ = State::kFade;
			counter_ = 0.0f;
		}
		break;
	}
	case State::kFade: {
		const float t = std::clamp(counter_ / fadeDuration_, 0.0f, 1.0f);
		color_.w = std::clamp(1.0f - t, 0.0f, 1.0f);
		objectColor_.SetColor(color_);
		if (counter_ >= fadeDuration_) {
			state_ = State::kDead;
		}
		break;
	}
	case State::kDead:
	default:
		break;
	}

	circleWorldTransform_.matWorld_ = MakeAffineMatrix(circleWorldTransform_.scale_, circleWorldTransform_.rotation_, circleWorldTransform_.translation_);
	circleWorldTransform_.TransferMatrix();

	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
		worldTransform.TransferMatrix();
	}
}

void HitEffect::Draw() {
	if (!model_ || !camera_ || state_ == State::kDead) {
		return;
	}

	model_->Draw(circleWorldTransform_, *camera_, &objectColor_);
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		model_->Draw(worldTransform, *camera_, &objectColor_);
	}
}
