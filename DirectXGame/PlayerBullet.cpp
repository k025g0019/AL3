#include "PlayerBullet.h"
#include "worldTransform.h"
using namespace KamataEngine;

void PlayerBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = velocity;
	worldTransformMatrix(worldTransform_);
}

void PlayerBullet::Update() {
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
	worldTransformMatrix(worldTransform_);
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw(const Camera& camera) {
	model_->Draw(worldTransform_, camera, textureHandle_);
}
