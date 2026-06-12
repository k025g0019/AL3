#include "Enemy.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void Enemy::Initialize(Model* model, uint32_t textureHandle) {
	// 外部から受け取った必要データの記録
	model_ = model;
	textureHandle_ = textureHandle;


	// 必須データの確認
	assert(model_ != nullptr);


	// ワールド座標の初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 100.0f};
	worldTransform_.matWorld_ = MakeScaleMatrix(worldTransform_.scale_) * MakeRotateXMatrix(worldTransform_.rotation_.x)
		* MakeRotateYMatrix(worldTransform_.rotation_.y) *
		MakeRotateZMatrix(worldTransform_.rotation_.z) * MakeTranslateMatrix(worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Enemy::Update() {
	move_ = {0.0f, 0.0f, 0.0f};
	move_.z = kMoveSpeed;
	worldTransform_.translation_.z -= move_.z;
	worldTransformMatrix(worldTransform_);
}

void Enemy::Draw(const Camera& camera) {
	assert(model_!=nullptr);
	model_->Draw(worldTransform_, camera, textureHandle_);
}
