//====================
// エントリーポイント 実装
//====================
#include "GameScene.h"
#include "KamataEngine.h"

#include <Windows.h>

using namespace KamataEngine;

//====================
// エントリポイント
//====================
/*
アプリ全体の初期化・更新・描画・終了を管理する
*/
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// エンジン初期化
	KamataEngine::Initialize(L"LC1B_29_Mario_Showta_AL2");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// シーン生成
	GameScene* gameScene = new GameScene();

	// ImGui取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// シーン初期化
	gameScene->Initialize();

	// メインループ
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

	// 後始末
	delete gameScene;
	gameScene = nullptr;

	KamataEngine::Finalize();
	return 0;
}
