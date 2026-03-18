//====================
// GameScene
//====================
#include "GameScene.h"

using namespace KamataEngine;

void GameScene::GenerateBlocks() {
	const uint32_t kNumBlockVertical = 20;
	const uint32_t kNumBlockHorizontal = 100;

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

	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	GenerateBlocks();

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 18);
	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	const int32_t kEnemyCount = 3;
	for (int32_t i = 0; i < kEnemyCount; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(12 + static_cast<uint32_t>(i) * 8, 18);
		newEnemy->Initialize(playerModel_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	cameraController_ = new CameraController();
	cameraController_->SetCamera(&camera_);
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();

	delete fade_;
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, kFadeDuration);

	phase_ = Phase::kFadeIn;
	finished_ = false;
	deathParticles_ = nullptr;
}

void GameScene::UpdateBlockMatrices() {
	for (const std::vector<KamataEngine::WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			worldTransformBlock->TransferMatrix();
		}
	}
}

void GameScene::UpdatePlayPhase() {
	player_->Update();
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}
	CheckAllCollisions();
	cameraController_->Update();

	ChangePhase();
}

void GameScene::UpdateDeathPhase() {
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	if (deathParticles_) {
		deathParticles_->Update();
		if (deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, kFadeDuration);
		}
	}
}

void GameScene::ChangePhase() {
	if (phase_ == Phase::kPlay) {
		if (player_ && player_->IsDead()) {
			phase_ = Phase::kDeath;
			const Vector3 deathParticlesPosition = player_->GetWorldPosition();
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(playerModel_, &camera_, deathParticlesPosition);
		}
	}
}

void GameScene::Update() {
	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::End();

	debugCamera_->Update();
	UpdateBlockMatrices();

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kPlay:
		UpdatePlayPhase();
		break;
	case Phase::kDeath:
		UpdateDeathPhase();
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}

void GameScene::Draw() {
	Model::PreDraw();

	if ((phase_ == Phase::kFadeIn || phase_ == Phase::kPlay) && player_) {
		player_->Draw();
	}

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	for (const std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			if (!worldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	Model::PostDraw();

	if (fade_) {
		fade_->Draw();
	}
}

GameScene::GameScene() = default;

GameScene::~GameScene() {
	for (std::vector<KamataEngine::WorldTransform*>& row : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform*& block : row) {
			delete block;
			block = nullptr;
		}
	}
	worldTransformBlocks_.clear();

	delete cameraController_;
	cameraController_ = nullptr;

	delete player_;
	player_ = nullptr;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	delete deathParticles_;
	deathParticles_ = nullptr;

	delete mapChipField_;
	mapChipField_ = nullptr;

	delete debugCamera_;
	debugCamera_ = nullptr;

	delete sprite_;
	sprite_ = nullptr;

	delete model_;
	model_ = nullptr;

	delete modelBlock_;
	modelBlock_ = nullptr;

	delete playerModel_;
	playerModel_ = nullptr;

	delete fade_;
	fade_ = nullptr;
}

void GameScene::CheckAllCollisions() {
	const AABB playerAABB = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		const AABB enemyAABB = enemy->GetAABB();
		const bool isHit = IsAABBCollision(playerAABB, enemyAABB);
		if (isHit) {
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
}
