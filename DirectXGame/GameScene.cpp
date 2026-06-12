#include "GameScene.h"

#include <imgui.h>
#include <string>

using namespace KamataEngine;

namespace {
	const Camera& GetActiveCamera(const Camera& camera, DebugCamera* debugCamera, bool isDebugCameraActive) {
		if (isDebugCameraActive) {
			assert(debugCamera != nullptr);
			return debugCamera->GetCamera();
		}

		return camera;
	}
} // namespace

void GameScene::Initialize() {
	if (isInitialized_) {
		return;
	}

	// テクスチャとモデルの読み込み
	playertextureHandle_ = TextureManager::Load("mario.jpg");
	enemytextureHandle_ = TextureManager::Load("mario.jpg");
	playerModel_ = Model::Create();
	enemyModel_ = Model::Create();
	// カメラの初期化
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -15.0f};
	camera_.TransferMatrix();

	// 描画系のカメラ設定
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 自キャラの生成
	player_ = new Player();
	enemy_ = new Enemy();
	player_->Initialize(playerModel_, playertextureHandle_);
	enemy_->Initialize(enemyModel_, enemytextureHandle_);

	// 軸方向表示を有効化
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&camera_);

	isInitialized_ = true;
}

void GameScene::Update() {
	assert(player_ != nullptr);
	assert(debugCamera_ != nullptr);

	// 自キャラの更新
	player_->Update();
	enemy_->Update();
	// 座標の画面表示
	const Vector3& playerPosition = player_->GetWorldPosition();
	const Vector3& enemyPosition = enemy_->GetWorldPosition();
	ImGui::Begin("Player");
	ImGui::Text("x = %.2f", playerPosition.x);
	ImGui::Text("y = %.2f", playerPosition.y);
	ImGui::Text("z = %.2f", playerPosition.z);
	ImGui::End();
	ImGui::Begin("Enemy");
	ImGui::Text("x = %.2f", enemyPosition.x);
	ImGui::Text("y = %.2f", enemyPosition.y);
	ImGui::Text("z = %.2f", enemyPosition.z);
	ImGui::End();

#ifdef _DEBUG
	// デバッグカメラの切り替え
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	// 有効中のカメラだけを更新
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
	}
	else {
		camera_.TransferMatrix();
		AxisIndicator::GetInstance()->SetTargetCamera(&camera_);
	}
}

void GameScene::Draw() {
	assert(player_ != nullptr);

	const Camera& activeCamera = GetActiveCamera(camera_, debugCamera_, isDebugCameraActive_);

	Model::PreDraw();
	player_->Draw(activeCamera);
	enemy_->Draw(activeCamera);
	Model::PostDraw();
}

GameScene::GameScene() {
}

GameScene::~GameScene() {
	delete player_;
	delete debugCamera_;
	delete playerModel_;
}
