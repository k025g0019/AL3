#include "GameScene.h"

using namespace KamataEngine;

// CSV縺ｮ繝槭ャ繝玲ュ蝣ｱ繧貞・縺ｫ繝悶Ο繝・け縺ｮ繝ｯ繝ｼ繝ｫ繝芽｡悟・繧堤函謌・
void GameScene::GenerateBlocks() {
	uint32_t kNumBlockVertical = 20;
	uint32_t kNumBlockHorizontal = 100;

	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				worldTransformBlocks_[i][j] = new WorldTransform();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->scale_ = {2.0f, 2.0f, 2.0f};
				worldTransformBlocks_[i][j]->rotation_ = {0.0f, 0.0f, 0.0f};
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

// 繧ｷ繝ｼ繝ｳ蛻晄悄蛹・
void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("mario.jpg");
	playerModel_ = Model::CreateFromOBJ("player", true);
	model_ = Model::Create();
	modelBlock_ = Model::CreateFromOBJ("block", true);

	worldTransform_.Initialize();
	camera_.Initialize();
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	debugCamera_ = new DebugCamera(1280, 720);
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// 繝・ヰ繝・げ逕ｨ霆ｸ繧定｡ｨ遉ｺ
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	GenerateBlocks();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 5);

	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// 繧ｫ繝｡繝ｩ霑ｽ蠕薙さ繝ｳ繝医Ο繝ｼ繝ｩ
	cameraController_ = new CameraController();
	cameraController_->SetCamera(&camera_);
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();
}

// 繧ｷ繝ｼ繝ｳ譖ｴ譁ｰ
void GameScene::Update() {
	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::End();

	debugCamera_->Update();

	// 繝悶Ο繝・け陦悟・譖ｴ譁ｰ
	for (const std::vector<KamataEngine::WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			worldTransformBlock->TransferMatrix();
		}
	}

	player_->Update();
	cameraController_->Update();
}

// 繧ｷ繝ｼ繝ｳ謠冗判
void GameScene::Draw() {
	Model::PreDraw();
	player_->Draw();

	for (const std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			if (!worldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	Model::PostDraw();
}

GameScene::GameScene() { Initialize(); }

GameScene::~GameScene() {
	delete debugCamera_;
	delete model_;
	sprite_ = nullptr;
	delete player_;
	delete mapChipField_;
}
