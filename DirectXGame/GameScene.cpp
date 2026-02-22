#include "GameScene.h"
#include <cmath>

// KamataEngine 名前空間を使用
using namespace KamataEngine;

//
// スケール行列を作成
//
static Matrix4x4 MakeScaleMatrix(const Vector3& s) {
	Matrix4x4 m{};

	// 対角成分にスケール値を入れる
	m.m[0][0] = s.x;
	m.m[1][1] = s.y;
	m.m[2][2] = s.z;
	m.m[3][3] = 1.0f;

	return m;
}

//
// X軸回転行列
//
static Matrix4x4 MakeRotateX(float x) {
	const float c = std::cos(x);
	const float s = std::sin(x);

	Matrix4x4 m{};
	m.m[0][0] = 1.0f;
	m.m[1][1] = c;
	m.m[1][2] = s;
	m.m[2][1] = -s;
	m.m[2][2] = c;
	m.m[3][3] = 1.0f;

	return m;
}

//
// Y軸回転行列
//
static Matrix4x4 MakeRotateY(float y) {
	const float c = std::cos(y);
	const float s = std::sin(y);

	Matrix4x4 m{};
	m.m[0][0] = c;
	m.m[0][2] = -s;
	m.m[1][1] = 1.0f;
	m.m[2][0] = s;
	m.m[2][2] = c;
	m.m[3][3] = 1.0f;

	return m;
}

//
// Z軸回転行列
//
static Matrix4x4 MakeRotateZ(float z) {
	const float c = std::cos(z);
	const float s = std::sin(z);

	Matrix4x4 m{};
	m.m[0][0] = c;
	m.m[0][1] = s;
	m.m[1][0] = -s;
	m.m[1][1] = c;
	m.m[2][2] = 1.0f;
	m.m[3][3] = 1.0f;

	return m;
}

//
// 行列同士の積
//
static Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 r{};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			r.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
		}
	}
	return r;
}

//
// 平行移動行列
//
static Matrix4x4 MakeTranslateMatrix(const Vector3& t) {
	Matrix4x4 m{};

	m.m[0][0] = 1.0f;
	m.m[1][1] = 1.0f;
	m.m[2][2] = 1.0f;

	m.m[3][0] = t.x;
	m.m[3][1] = t.y;
	m.m[3][2] = t.z;
	m.m[3][3] = 1.0f;

	return m;
}

//
// SRT（Scale → Rotate → Translate）からワールド行列を生成
//
static Matrix4x4 MakeAffineMatrix(const Vector3& s, const Vector3& r, const Vector3& t) {

	Matrix4x4 S = MakeScaleMatrix(s);
	Matrix4x4 Rx = MakeRotateX(r.x);
	Matrix4x4 Ry = MakeRotateY(r.y);
	Matrix4x4 Rz = MakeRotateZ(r.z);
	Matrix4x4 T = MakeTranslateMatrix(t);

	// 回転順：Z → Y → X（教材準拠）
	Matrix4x4 R = Multiply(Multiply(Rz, Ry), Rx);

	// S → R → T
	return Multiply(Multiply(S, R), T);
}


void GameScene::Initialize() {

	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("mario.jpg");

	// モデル生成
	model_ = Model::Create();
	modelBlock_ = Model::Create();

	// ワールド変換・カメラ初期化
	worldTransform.Initialize();
	camera_.Initialize();

	// プリミティブ描画用カメラ設定
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	// デバッグカメラ生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 軸表示
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	// ブロック配置数
	const uint32_t kNumBlockHorizontal = 20;
	const uint32_t kNumBlockVertical = 10;

	// ブロックサイズ
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	// 2次元配列確保
	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// 千鳥配置でブロック生成
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		for (uint32_t j = i % 2; j < kNumBlockHorizontal; j += 2) {

			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();

			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
		}
	}

	// スカイドーム
	skydome = new Skydome();
	skydome->Initialize();

	
}


void GameScene::Update() {

	// ImGui デバッグ表示
	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::End();

	// ブロックのワールド行列更新
	for (const auto& row : worldTransformBlocks_) {
		for (WorldTransform* block : row) {

			if (!block) {
				continue;
			}

			block->matWorld_ = MakeAffineMatrix(block->scale_, block->rotation_, block->translation_);

			block->TransferMatrix();
		}
	}

#ifdef _DEBUG
	// デバッグカメラ切替
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive = !isDebugCameraActive;
	}
#endif

	// カメラ更新
	if (isDebugCameraActive) {

		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();

	} else {

		camera_.UpdateViewMatrix();
		camera_.UpdateProjectionMatrix();
		camera_.TransferMatrix();
	}

	// スカイドーム更新
	skydome->Update();
}

void GameScene::Draw() {

	Model::PreDraw();

	for (const auto& row : worldTransformBlocks_) {
		for (WorldTransform* block : row) {

			if (!block) {
				continue;
			}

			modelBlock_->Draw(*block, camera_);
		}
	}

	// スカイドーム描画
	skydome->Draw(camera_);
	Model::PostDraw();

	
}

GameScene::GameScene() {
	Initialize();
}

GameScene::~GameScene() {

	delete debugCamera_;
	delete model_;
	delete modelBlock_;

	// ブロック解放
	for (const auto& row : worldTransformBlocks_) {
		for (WorldTransform* block : row) {
			delete block;
		}
	}
	worldTransformBlocks_.clear();

	delete modelSkydome;
}