#include "GameScene.h"

#include <cmath>

using namespace KamataEngine;

static Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 matrix{};
	matrix.m[0][0] = scale.x;
	matrix.m[1][1] = scale.y;
	matrix.m[2][2] = scale.z;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

static Matrix4x4 MakeRotateX(float angle) {
	const float cosValue = std::cos(angle);
	const float sinValue = std::sin(angle);

	Matrix4x4 matrix{};
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cosValue;
	matrix.m[1][2] = sinValue;
	matrix.m[2][1] = -sinValue;
	matrix.m[2][2] = cosValue;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

static Matrix4x4 MakeRotateY(float angle) {
	const float cosValue = std::cos(angle);
	const float sinValue = std::sin(angle);

	Matrix4x4 matrix{};
	matrix.m[0][0] = cosValue;
	matrix.m[0][2] = -sinValue;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][0] = sinValue;
	matrix.m[2][2] = cosValue;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

static Matrix4x4 MakeRotateZ(float angle) {
	const float cosValue = std::cos(angle);
	const float sinValue = std::sin(angle);

	Matrix4x4 matrix{};
	matrix.m[0][0] = cosValue;
	matrix.m[0][1] = sinValue;
	matrix.m[1][0] = -sinValue;
	matrix.m[1][1] = cosValue;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

static Matrix4x4 Multiply(const Matrix4x4& left, const Matrix4x4& right) {
	Matrix4x4 result{};

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = left.m[row][0] * right.m[0][column] + left.m[row][1] * right.m[1][column] +
				left.m[row][2] * right.m[2][column] + left.m[row][3] * right.m[3][column];
		}
	}

	return result;
}

static Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 matrix{};
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][0] = translate.x;
	matrix.m[3][1] = translate.y;
	matrix.m[3][2] = translate.z;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateXMatrix = MakeRotateX(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateY(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZ(rotate.z);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
	Matrix4x4 rotateMatrix = Multiply(Multiply(rotateZMatrix, rotateYMatrix), rotateXMatrix);
	return Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
}

void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("mario.jpg");

	model_ = Model::Create();
	modelBlock_ = Model::Create();

	worldTransform.Initialize();
	camera_.Initialize();
	camera_.farZ = 200.0f;

	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	debugCamera_ = new DebugCamera(1280, 720);
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	constexpr uint32_t kNumBlockHorizontal = 20;
	constexpr uint32_t kNumBlockVertical = 10;
	constexpr float kBlockWidth = 2.0f;
	constexpr float kBlockHeight = 2.0f;

	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t row = 0; row < kNumBlockVertical; row++) {
		worldTransformBlocks_[row].resize(kNumBlockHorizontal);
	}

	for (uint32_t row = 0; row < kNumBlockVertical; row++) {
		for (uint32_t column = row % 2; column < kNumBlockHorizontal; column += 2) {
			worldTransformBlocks_[row][column] = new WorldTransform();
			worldTransformBlocks_[row][column]->Initialize();
			worldTransformBlocks_[row][column]->translation_.x = kBlockWidth * static_cast<float>(column);
			worldTransformBlocks_[row][column]->translation_.y = kBlockHeight * static_cast<float>(row);
		}
	}

	skydome = new Skydome();
	skydome->Initialize();

	player_ = new Player();
	player_->Initialize(model_, textureHandle_, &camera_);
}

void GameScene::Update() {
	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::End();

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
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive = !isDebugCameraActive;
	}
#endif

	if (isDebugCameraActive) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	}
	else {
		camera_.UpdateViewMatrix();
		camera_.UpdateProjectionMatrix();
		camera_.TransferMatrix();
	}

	skydome->Update();
	player_->Update();
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

	skydome->Draw(camera_);
	player_->Draw();

	Model::PostDraw();
}

GameScene::GameScene() {
}

GameScene::~GameScene() {
	delete debugCamera_;
	delete model_;
	delete modelBlock_;

	for (const auto& row : worldTransformBlocks_) {
		for (WorldTransform* block : row) {
			delete block;
		}
	}
	worldTransformBlocks_.clear();

	delete player_;
	delete skydome;
}
