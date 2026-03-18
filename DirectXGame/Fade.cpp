#include "Fade.h"

#include <algorithm>

#include <base/WinApp.h>

using namespace KamataEngine;

Fade::~Fade() {
	delete sprite_;
	sprite_ = nullptr;
}

void Fade::Initialize() {
	const uint32_t whiteTexture = TextureManager::Load("white1x1.png");
	sprite_ = Sprite::Create(whiteTexture, {0.0f, 0.0f}, {0, 0, 0, 1});
	sprite_->SetSize(Vector2(static_cast<float>(WinApp::kWindowWidth), static_cast<float>(WinApp::kWindowHeight)));
	sprite_->SetColor(Vector4(0, 0, 0, 1));

	status_ = Status::None;
	duration_ = 0.0f;
	counter_ = 0.0f;
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = (std::max)(duration, 0.0001f);
	counter_ = 0.0f;
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinished() const {
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		return counter_ >= duration_;
	default:
		return true;
	}
}

void Fade::Update() {
	if (!sprite_) {
		return;
	}

	switch (status_) {
	case Status::None:
		return;
	case Status::FadeIn: {
		counter_ += 1.0f / 60.0f;
		counter_ = (std::min)(counter_, duration_);
		const float alpha = std::clamp(1.0f - counter_ / duration_, 0.0f, 1.0f);
		sprite_->SetColor(Vector4(0, 0, 0, alpha));
		if (counter_ >= duration_) {
			status_ = Status::None;
		}
		break;
	}
	case Status::FadeOut: {
		counter_ += 1.0f / 60.0f;
		counter_ = (std::min)(counter_, duration_);
		const float alpha = std::clamp(counter_ / duration_, 0.0f, 1.0f);
		sprite_->SetColor(Vector4(0, 0, 0, alpha));
		if (counter_ >= duration_) {
			status_ = Status::None;
		}
		break;
	}
	}
}

void Fade::Draw() {
	if (!sprite_ || status_ == Status::None) {
		return;
	}

	Sprite::PreDraw();
	sprite_->Draw();
	Sprite::PostDraw();
}
