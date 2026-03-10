#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

using namespace KamataEngine;

// Windowsアプリのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"LC1B_29_マノ_ショウタ_AL2");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// シーン生成
	GameScene* gameScene = new GameScene();

	// ImGui初期化済みマネージャ
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	gameScene->Initialize();

	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		// ImGui更新開始
		imguiManager->Begin();

		// ゲーム更新
		gameScene->Update();

		// ImGui更新終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		// ゲーム描画
		gameScene->Draw();

		// ImGui描画
		imguiManager->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// シーン解放
	delete gameScene;
	gameScene = nullptr;

	KamataEngine::Finalize();
	return 0;
}