#pragma once

#include "KamataEngine.h"

class TitleScene {
public:
	TitleScene() = default;
	~TitleScene() = default;

	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }

private:
	bool finished_ = false;
};
