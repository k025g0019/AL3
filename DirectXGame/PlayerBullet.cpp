#include "PlayerBullet.h"

#include "worldTransform.h"
using namespace KamataEngine;

void PlayerBullet::Initialize(Model* model, const Vector3& position) {
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransformMatrix(worldTransform_);
}

void PlayerBullet::Update() {
	worldTransformMatrix(worldTransform_);
}

void PlayerBullet::Draw(const Camera& camera) {
	model_->Draw(worldTransform_, camera, textureHandle_);
}
