#include "Player.h"

namespace KamataEngine {
class Model;
}

using namespace KamataEngine;

Player::Player() {}
Player::~Player() {}
// 初期化
void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera) {
	assert(model);
	camera_ = camera;
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.translation_ = {5.0f, 0.0f, 0.0f};
}
// 更新
void Player::Update() { worldTransform_.TransferMatrix(); }
// 描画
void Player::Draw() { 
	model_->Draw(worldTransform_, *camera_, textureHandle_); 
}
// 	delete debugCamera_;
// 	delete player_;
// }
