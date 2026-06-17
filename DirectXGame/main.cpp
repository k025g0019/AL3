#include <Windows.h>
#include "GameScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Initialize(L"LC1B_29_マノ_ショウタ_AL2");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	auto gameScene = new GameScene();
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	gameScene->Initialize();

	while (true) {
		if (Update()) {
			break;
		}

		imguiManager->Begin();

		// ゲームシーンの更新
		gameScene->Update();

		imguiManager->End();

		dxCommon->PreDraw();

		// ゲームシーンの描画
		gameScene->Draw();
		AxisIndicator::GetInstance()->Draw();
		imguiManager->Draw();

		dxCommon->PostDraw();
	}

	delete gameScene;
	gameScene = nullptr;

	Finalize();

	return 0;
}
