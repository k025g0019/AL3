#include "GameScene.h"

using namespace KamataEngine;
void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("mario.jpg");

	// スプライトインスタンスの生成
	model_ = Model::Create();

	worldTransform_.Initialize();
	camera_.Initialize();

	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	debugCamera_ = new DebugCamera(1280, 720);

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);

	// 軸方向表示が参照するビュープロジェクションを指定する
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	player_ = new Player();
	player_->Initialize(model_,textureHandle_,&camera_);
}

void GameScene::Update() {

	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::End();

	debugCamera_->Update();
	//// スプライトの座標を取得
	// Vector2 position = sprite_->GetPosition();

	//// 座標を{2.0f, 1.0f}移動
	// position.x += 2.0f;
	// position.y += 1.0f;

	//// 移動した座標をスプライトに反映
	// sprite_->SetPosition(position);

	player_->Update();
}

void GameScene::Draw() {

	//PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});

	Model::PreDraw();
	player_->Draw();
	Model::PostDraw();

	
}

GameScene::GameScene() { Initialize(); }

GameScene::~GameScene() {
	delete debugCamera_;
	delete model_;
	sprite_ = nullptr;
	delete player_;
}