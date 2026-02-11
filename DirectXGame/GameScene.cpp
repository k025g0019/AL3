#include "GameScene.h"
#include <cmath>

// KamataEngine の型に合わせる
using namespace KamataEngine;

static Matrix4x4 MakeScaleMatrix(const Vector3& s) {
	Matrix4x4 m{};
	m.m[0][0] = s.x;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;
	m.m[1][0] = 0.0f;
	m.m[1][1] = s.y;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;
	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = s.z;
	m.m[2][3] = 0.0f;
	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
	return m;
}

static Matrix4x4 MakeRotateX(float x) {
	const float c = std::cos(x);
	const float s = std::sin(x);
	Matrix4x4 m{};
	m.m[0][0] = 1.0f;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;
	m.m[1][0] = 0.0f;
	m.m[1][1] = c;
	m.m[1][2] = s;
	m.m[1][3] = 0.0f;
	m.m[2][0] = 0.0f;
	m.m[2][1] = -s;
	m.m[2][2] = c;
	m.m[2][3] = 0.0f;
	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
	return m;
}

static Matrix4x4 MakeRotateY(float y) {
	const float c = std::cos(y);
	const float s = std::sin(y);
	Matrix4x4 m{};
	m.m[0][0] = c;
	m.m[0][1] = 0.0f;
	m.m[0][2] = -s;
	m.m[0][3] = 0.0f;
	m.m[1][0] = 0.0f;
	m.m[1][1] = 1.0f;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;
	m.m[2][0] = s;
	m.m[2][1] = 0.0f;
	m.m[2][2] = c;
	m.m[2][3] = 0.0f;
	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
	return m;
}

static Matrix4x4 MakeRotateZ(float z) {
	const float c = std::cos(z);
	const float s = std::sin(z);
	Matrix4x4 m{};
	m.m[0][0] = c;
	m.m[0][1] = s;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;
	m.m[1][0] = -s;
	m.m[1][1] = c;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;
	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = 1.0f;
	m.m[2][3] = 0.0f;
	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
	return m;
}

static Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 r{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			r.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
		}
	}
	return r;
}

static Matrix4x4 MakeTranslateMatrix(const Vector3& t) {
	Matrix4x4 m{};
	m.m[0][0] = 1.0f;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;
	m.m[1][0] = 0.0f;
	m.m[1][1] = 1.0f;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;
	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = 1.0f;
	m.m[2][3] = 0.0f;
	m.m[3][0] = t.x;
	m.m[3][1] = t.y;
	m.m[3][2] = t.z;
	m.m[3][3] = 1.0f;
	return m;
}

// scale, rotation(xyz), translation からワールド行列を作る
static Matrix4x4 MakeAffineMatrix(const Vector3& s, const Vector3& r, const Vector3& t) {
	Matrix4x4 S = MakeScaleMatrix(s);
	Matrix4x4 Rx = MakeRotateX(r.x);
	Matrix4x4 Ry = MakeRotateY(r.y);
	Matrix4x4 Rz = MakeRotateZ(r.z);
	Matrix4x4 T = MakeTranslateMatrix(t);

	// 回転順は教材に合わせる（ここは Z→Y→X の例）
	Matrix4x4 R = Multiply(Multiply(Rz, Ry), Rx);

	return Multiply(Multiply(S, R), T);
}

using namespace KamataEngine;
void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("mario.jpg");

	// 3Dモデルの生成
	model_ = Model::Create();
	modelBlock_ = Model::Create();

	worldTransform.Initialize();
	camera_.Initialize();

	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	debugCamera_ = new DebugCamera(1280, 720);

	AxisIndicator::GetInstance()->SetVisible(true);

	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	const uint32_t kNumBlockHorizontal = 20;
	const uint32_t kNumBlockVertical = 10;

	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		for (uint32_t j = i % 2; j < kNumBlockHorizontal; j += 2) {
			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
		}
	}
}

void GameScene::Update() {

	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::End();

	for (const std::vector<KamataEngine::WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			worldTransformBlock->TransferMatrix();
		}
	}
	#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			isDebugCameraActive = !isDebugCameraActive;
		}
	#endif

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
}

void GameScene::Draw() {

	// PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});

	Model::PreDraw();
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
	delete modelBlock_;
	sprite_ = nullptr;
	for (const std::vector<KamataEngine::WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}