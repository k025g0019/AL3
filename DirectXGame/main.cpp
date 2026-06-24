#include <Windows.h>

#include "GameScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	KamataEngine::Initialize(L"AL2_Skydome");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	GameScene* gameScene = new GameScene();
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	gameScene->Initialize();

	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		imguiManager->Begin();
		gameScene->Update();
		imguiManager->End();

		dxCommon->PreDraw();
		gameScene->Draw();
		imguiManager->Draw();
		dxCommon->PostDraw();
	}

	delete gameScene;
	gameScene = nullptr;

	KamataEngine::Finalize();
	return 0;
}
