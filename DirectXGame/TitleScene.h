#pragma once

#include "Fade.h"
#include "KamataEngine.h"

class TitleScene {
public:
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

	TitleScene() = default;
	~TitleScene();

	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }

private:
	static inline const float kFadeDuration = 1.0f;
	bool finished_ = false;
	Phase phase_ = Phase::kFadeIn;
	Fade* fade_ = nullptr;
};
