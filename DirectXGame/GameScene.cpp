//====================
// ゲームシーン 実装
//====================
#include "GameScene.h"

using namespace KamataEngine;

//====================
// ブロック生成
//====================
/*
マップ情報をもとにブロックのワールド変換を生成する
*/
void GameScene::GenerateBlocks() {
	// マップサイズを定義
	uint32_t kNumBlockVertical = 20;
	uint32_t kNumBlockHorizontal = 100;

	// 2次元配列のサイズを確保
	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// ブロックチップの位置だけWorldTransformを生成
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

//====================
// 初期化
//====================
/*
シーンで使うリソースとオブジェクトを初期化する
*/
void GameScene::Initialize() {
	// テクスチャとモデルを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");
	playerModel_ = Model::CreateFromOBJ("player", true);
	model_ = Model::Create();
	modelBlock_ = Model::CreateFromOBJ("block", true);

	// ワールド変換とカメラを初期化
	worldTransform_.Initialize();
	camera_.Initialize();
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	// デバッグカメラとマップを初期化
	debugCamera_ = new DebugCamera(1280, 720);
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// 軸表示を設定
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	// マップブロックを生成
	GenerateBlocks();

	// プレイヤーを生成
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 18);
	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// 敵を複数生成
	const int32_t kEnemyCount = 3;
	for (int32_t i = 0; i < kEnemyCount; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(12 + static_cast<uint32_t>(i) * 8, 18);
		newEnemy->Initialize(playerModel_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	// カメラ制御を初期化
	cameraController_ = new CameraController();
	cameraController_->SetCamera(&camera_);
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();
}

//====================
// 更新
//====================
/*
毎フレームの更新処理を実行する
*/
void GameScene::Update() {
	// デバッグUIを表示
	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::End();

	// デバッグカメラを更新
	debugCamera_->Update();

	// ブロック行列を更新
	for (const std::vector<KamataEngine::WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			worldTransformBlock->TransferMatrix();
		}
	}

	// プレイヤー・敵・当たり判定を更新
	player_->Update();
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}
	CheckAllCollisions();

	// 追従カメラを更新
	cameraController_->Update();
}

//====================
// 描画
//====================
/*
シーン内オブジェクトを描画する
*/
void GameScene::Draw() {
	// モデル描画開始
	Model::PreDraw();

	// プレイヤーと敵を描画
	player_->Draw();
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// マップブロックを描画
	for (const std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			if (!worldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	// モデル描画終了
	Model::PostDraw();
}

//====================
// 生成破棄
//====================
GameScene::GameScene() { Initialize(); }

GameScene::~GameScene() {
	// 生成したオブジェクトを解放
	delete debugCamera_;
	delete model_;
	sprite_ = nullptr;
	delete player_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
	delete mapChipField_;
}

//====================
// 当たり判定
//====================
/*
プレイヤーと全敵のAABB交差判定を行う
*/
void GameScene::CheckAllCollisions() {
	// プレイヤーのAABBを取得
	const AABB playerAABB = player_->GetAABB();

	// 各敵との交差判定
	for (Enemy* enemy : enemies_) {
		const AABB enemyAABB = enemy->GetAABB();
		const bool isHit = IsAABBCollision(playerAABB, enemyAABB);
		if (isHit) {
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
}
