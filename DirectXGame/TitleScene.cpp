#include "TitleScene.h"

#include <dinput.h>

using namespace KamataEngine;

TitleScene::~TitleScene() {
	delete fade_;
	fade_ = nullptr;
}

void TitleScene::Initialize() {
	finished_ = false;
	phase_ = Phase::kFadeIn;

	delete fade_;
	fade_ = nullptr;
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, kFadeDuration);
}

void TitleScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, kFadeDuration);
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}

void TitleScene::Draw() {
	if (fade_) {
		fade_->Draw();
	}
}
