#include "GameScene.h"
#include "KamataEngine.h"

#include <Windows.h>

using namespace KamataEngine;

// Windows アプリのエントリポイント
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	KamataEngine::Initialize(L"LC1B_29_Mario_Showta_AL2");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// シーンを生成
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
