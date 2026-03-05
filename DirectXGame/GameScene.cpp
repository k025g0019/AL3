#include "GameScene.h"

using namespace KamataEngine;

void GameScene::GenerateBlocks() {
	uint32_t kNumBlockVertical = 20;
	uint32_t kNumBlockHorizontal = 100;

	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		for (uint32_t j = i % 2; j < kNumBlockHorizontal; j += 2) {

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
	// �X�v���C�g�C���X�^���X�̐���
	model_ = Model::Create();
	modelBlock_ = Model::CreateFromOBJ("block", true);

	worldTransform_.Initialize();
	camera_.Initialize();

	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	debugCamera_ = new DebugCamera(1280, 720);
	mapChipField_ = new MapChipField;

	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	// �������\���̕\����L���ɂ���
	AxisIndicator::GetInstance()->SetVisible(true);

	// �������\�����Q�Ƃ���r���[�v���W�F�N�V������w�肷��
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
	GenerateBlocks();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2*2, 5*2);

	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, playerPosition);

	player_->SetMapChipField(mapChipField_);
	//==================================
	// CameraController
	//===================================

	// 生成
	cameraController_ = new CameraController();
	cameraController_->SetCamera(&camera_);
	// 初期化
	cameraController_->Initialize();

	// 目標プレイヤーのセット
	cameraController_->SetTarget(player_);

	// カメラのリセット
	cameraController_->Reset();
}

void GameScene::Update() {

	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::End();

	debugCamera_->Update();
	//// �X�v���C�g�̍��W��擾
	// Vector2 position = sprite_->GetPosition();
	for (const std::vector<KamataEngine::WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			worldTransformBlock->TransferMatrix();
		}
	}
	//// ���W��{2.0f, 1.0f}�ړ�
	// position.x += 2.0f;
	// position.y += 1.0f;

	//// �ړ��������W��X�v���C�g�ɔ��f
	// sprite_->SetPosition(position);

	player_->Update();

	cameraController_->Update();
}

void GameScene::Draw() {

	// PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});

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