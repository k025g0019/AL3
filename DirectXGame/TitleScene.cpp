#include "TitleScene.h"

#include <dinput.h>

using namespace KamataEngine;

void TitleScene::Initialize() { finished_ = false; }

void TitleScene::Update() {
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}
}

void TitleScene::Draw() {}
