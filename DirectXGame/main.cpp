#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

using namespace KamataEngine;
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize(L"LC1B_29_マノ_ショウタ_AL2");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ゲームシーンのインスタンス生成
	GameScene* gameScene = new GameScene();

	// ゲームシーンの初期化

	gameScene->Initialize();

	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		//
		//
		//

		//================================================
		// ゲームシーンの更新
		//================================================
		gameScene->Update();

		//================================================
		// 描画開始
		//================================================

		dxCommon->PreDraw();

		//================================================
		// ゲームシーンの描画 ^^
		//==============================================

		gameScene->Draw();

		//================================================
		// 描画終了
		//================================================
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;

	// nullptrの代入
	gameScene = nullptr;

	KamataEngine::Finalize();

	return 0;
}
