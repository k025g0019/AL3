//====================
// entry point
//====================
#include <Windows.h>
#include "GameScene.h"
#include "KamataEngine.h"
#include "StageManager.h"
#include "TitleScene.h"


using namespace KamataEngine;

namespace {
	enum class Scene {
		kUnknown = 0,
		kTitle,
		kGame,
	};

	auto scene = Scene::kUnknown;
	TitleScene* titleScene = nullptr;
	GameScene* gameScene = nullptr;
	StageManager* stageManager = nullptr;
	uint32_t currentStageNo = 1;

	void ChangeScene() {
		switch (scene) {
		case Scene::kTitle:
			if (titleScene && titleScene->IsFinished()) {
				scene = Scene::kGame;
				delete titleScene;
				titleScene = nullptr;
				gameScene = new GameScene();
				gameScene->SetStageNo(currentStageNo);
				gameScene->SetStageManager(stageManager);
				gameScene->Initialize();
			}
			break;
		case Scene::kGame:
			if (gameScene && gameScene->IsFinished()) {
				// ステージクリア：次のステージへ
				currentStageNo++;
				if (currentStageNo > stageManager->GetNumStages()) {
					// 全ステージクリア：タイトルへ戻る
					currentStageNo = 1;
					scene = Scene::kTitle;
					delete gameScene;
					gameScene = nullptr;
					titleScene = new TitleScene();
					titleScene->Initialize();
				} else {
					// 次のステージを読み込み
					delete gameScene;
					gameScene = new GameScene();
					gameScene->SetStageNo(currentStageNo);
					gameScene->SetStageManager(stageManager);
					gameScene->Initialize();
				}
			} else if (gameScene && gameScene->IsReloadRequested()) {
				delete gameScene;
				gameScene = new GameScene();
				gameScene->SetStageNo(currentStageNo);
				gameScene->SetStageManager(stageManager);
				gameScene->Initialize();
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
	Initialize(L"LC1B_29_Mario_SQChowta_AL2");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	scene = Scene::kTitle;
	stageManager = new StageManager();
	stageManager->AddStage(1, "Resources/stage_01.csv");
	stageManager->AddStage(2, "Resources/stage_02.csv");
	stageManager->AddStage(3, "Resources/stage_03.csv");

	titleScene = new TitleScene();
	titleScene->Initialize();

	while (true) {
		if (Update()) {
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
	delete stageManager;
	stageManager = nullptr;

	Finalize();
	return 0;
}
