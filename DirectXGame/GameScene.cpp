//====================
// GameScene
//====================
#include "GameScene.h"

#include <algorithm>

using namespace KamataEngine;

void GameScene::GenerateFieldObjects() {
	constexpr uint32_t kNumBlockVertical = 20;
	constexpr uint32_t kNumBlockHorizontal = 100;

	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			MapChipType type = mapChipField_->GetMapChipTypeByIndex(j, i);
			if (type == MapChipType::kBlock) {
				worldTransformBlocks_[i][j] = new WorldTransform();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->scale_ = {2.0f, 2.0f, 2.0f};
				worldTransformBlocks_[i][j]->rotation_ = {0.0f, 0.0f, 0.0f};
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			} else if (type == MapChipType::kPlayer) {
				Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(j, i);
				player_ = new Player();
				player_->Initialize(playerModel_, &camera_, playerPosition);
				player_->SetMapChipField(mapChipField_);
			} else if (type == MapChipType::kEnemy) {
				Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(j, i);
				auto newEnemy = new Enemy();
				newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
				newEnemy->SetGameScene(this);
				enemies_.push_back(newEnemy);
			} else if (type == MapChipType::kShieldEnemy) {
				Vector3 shieldEnemyPosition = mapChipField_->GetMapChipPositionByIndex(j, i);
				auto newShieldEnemy = new ShieldEnemy();
				newShieldEnemy->Initialize(modelShieldEnemy_, &camera_, shieldEnemyPosition);
				newShieldEnemy->SetGameScene(this);
				shieldEnemies_.push_back(newShieldEnemy);
			}
		}
	}
}

void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("mario.jpg");
	playerModel_ = Model::CreateFromOBJ("player", true);
	model_ = Model::Create();
	modelBlock_ = Model::CreateFromOBJ("block", true);
	HitEffectModel_ = Model::CreateFromOBJ("hit_effect", true);
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	modelShieldEnemy_ = Model::CreateFromOBJ("shieldEnemy", true);
	guardEffectModel_ = Model::CreateFromOBJ("ring", true);
	worldTransform_.Initialize();
	camera_.Initialize();
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);
	HitEffect::SetModel(HitEffectModel_);
	HitEffect::SetCamera(&camera_);
	GuardEffect::SetModel(guardEffectModel_);
	GuardEffect::SetCamera(&camera_);
	debugCamera_ = new DebugCamera(1280, 720);
	mapChipField_ = new MapChipField;

	// StageManagerからCSVファイルパスを取得
	if (stageManager_) {
		mapChipField_->LoadMapChipCsv(stageManager_->GetStageCsvPath(stageNo_));
	}

	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	GenerateFieldObjects();

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
	for (const std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_,
			                                                  worldTransformBlock->rotation_,
			                                                  worldTransformBlock->translation_);
			worldTransformBlock->TransferMatrix();
		}
	}
}

void GameScene::UpdatePlayPhase() {
	player_->Update();
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}
	for (HitEffect* HitEffect : HitEffects_) {
		HitEffect->Update();
	}
	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		shieldEnemy->Update();
	}
	for (GuardEffect* guardEffect : guardEffects_) {
		guardEffect->Update();
	}
	RemoveDeadEnemies();
	RemoveDeadHitEffects();
	RemoveDeadShieldEnemies();
	CheckAllCollisions();
	cameraController_->Update();

	ChangePhase();
}

void GameScene::UpdateDeathPhase() {
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		shieldEnemy->Update();
	}
	for (HitEffect* HitEffect : HitEffects_) {
		HitEffect->Update();
	}
	for (GuardEffect* guardEffect : guardEffects_) {
		guardEffect->Update();
	}
	RemoveDeadEnemies();
	RemoveDeadHitEffects();

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
	ImGui::Text("Stage: %u / %u", stageNo_, stageManager_ ? stageManager_->GetNumStages() : 0);
	if (ImGui::Button("Reload")) {
		reloadRequested_ = true;
	}
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

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		shieldEnemy->Draw();
	}

	for (HitEffect* HitEffect : HitEffects_) {
		HitEffect->Draw();
	}

	for (GuardEffect* guardEffect : guardEffects_) {
		guardEffect->Draw();
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
	for (std::vector<WorldTransform*>& row : worldTransformBlocks_) {
		for (WorldTransform*& block : row) {
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

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		delete shieldEnemy;
	}
	shieldEnemies_.clear();

	for (HitEffect* HitEffect : HitEffects_) {
		delete HitEffect;
	}
	HitEffects_.clear();
	for (GuardEffect* guardEffect : guardEffects_) {
		delete guardEffect;
	}
	guardEffects_.clear();
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
	delete HitEffectModel_;
	HitEffectModel_ = nullptr;

	delete fade_;
	fade_ = nullptr;
}

void GameScene::CheckAllCollisions() {
	const AABB playerAABB = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		if (enemy->IsCollisionDisabled()) {
			continue;
		}
		const AABB enemyAABB = enemy->GetAABB();
		const bool isHit = IsAABBCollision(playerAABB, enemyAABB);
		if (isHit) {
			player_->EnemyOnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		if (shieldEnemy->IsCollisionDisabled()) {
			continue;
		}
		const AABB shieldEnemyAABB = shieldEnemy->GetAABB();
		const bool isHit = IsAABBCollision(playerAABB, shieldEnemyAABB);
		if (isHit) {
			player_->ShieldEnemyOnCollision(shieldEnemy);
			shieldEnemy->OnCollision(player_);
		}
	}
}

void GameScene::RemoveDeadEnemies() {
	auto erasedBegin = std::remove_if(enemies_.begin(), enemies_.end(), [](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
	enemies_.erase(erasedBegin, enemies_.end());
}

void GameScene::RemoveDeadShieldEnemies() {
	auto erasedBegin = std::remove_if(shieldEnemies_.begin(), shieldEnemies_.end(), [](ShieldEnemy* shieldEnemy) {
		if (shieldEnemy->IsDead()) {
			delete shieldEnemy;
			return true;
		}
		return false;
	});
	shieldEnemies_.erase(erasedBegin, shieldEnemies_.end());
}

void GameScene::CreateHitEffect(const Vector3& position) {
	HitEffect* newHitEffect = HitEffect::Create(position);
	HitEffects_.push_back(newHitEffect);
}

void GameScene::RemoveDeadHitEffects() {
	auto erasedBegin = std::remove_if(HitEffects_.begin(), HitEffects_.end(), [](HitEffect* HitEffect) {
		if (HitEffect->IsDead()) {
			delete HitEffect;
			return true;
		}
		return false;
	});
	HitEffects_.erase(erasedBegin, HitEffects_.end());
}

void GameScene::CreateGunEffect(const Vector3& position) {
	GuardEffect* newGuardEffect = GuardEffect::Create(position);
	guardEffects_.push_back(newGuardEffect);
}
