//====================
// entry point
//====================
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"

#include <Windows.h>

using namespace KamataEngine;

namespace {
enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

Scene scene = Scene::kUnknown;
TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene && titleScene->IsFinished()) {
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;
			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene && gameScene->IsFinished()) {
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	default:
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene) {
			titleScene->Update();
		}
		break;
	case Scene::kGame:
		if (gameScene) {
			gameScene->Update();
		}
		break;
	default:
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene) {
			titleScene->Draw();
		}
		break;
	case Scene::kGame:
		if (gameScene) {
			gameScene->Draw();
		}
		break;
	default:
		break;
	}
}
} // namespace

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	KamataEngine::Initialize(L"LC1B_29_Mario_Showta_AL2");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	scene = Scene::kTitle;
	titleScene = new TitleScene();
	titleScene->Initialize();

	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		imguiManager->Begin();
		ChangeScene();
		UpdateScene();
		imguiManager->End();

		dxCommon->PreDraw();
		DrawScene();
		imguiManager->Draw();
		dxCommon->PostDraw();
	}

	delete titleScene;
	titleScene = nullptr;
	delete gameScene;
	gameScene = nullptr;

	KamataEngine::Finalize();
	return 0;
}
