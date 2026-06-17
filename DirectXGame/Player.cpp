#include "Player.h"

#include <algorithm>
#include <math/MathUtility.h>
#include "Vector&Matrix.h"

using namespace KamataEngine;

void Player::Initialize(Model* model, uint32_t textureHandle) {
	// 蠑墓焚縺ｧ蜿励￠蜿悶▲縺溷､繧偵Γ繝ｳ繝仙､画焚縺ｫ險倬鹸縺吶ｋ
	model_ = model;
	textureHandle_ = textureHandle;
	input_ = Input::GetInstance();

	// 蠢・医ョ繝ｼ繧ｿ縺ｮ蟄伜惠遒ｺ隱・
	assert(model_ != nullptr);
	assert(input_ != nullptr);

	// 繝ｯ繝ｼ繝ｫ繝牙ｺｧ讓吶・蛻晄悄蛹・
	worldTransform_.Initialize();
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransformMatrix(worldTransform_);
}

void Player::Update() {
	// 豈弱ヵ繝ｬ繝ｼ繝遘ｻ蜍暮㍼繧偵Μ繧ｻ繝・ヨ縺吶ｋ
	move_ = {0.0f, 0.0f, 0.0f};

	for (auto bulletIterator = bullets_.begin(); bulletIterator != bullets_.end();) {
		if ((*bulletIterator)->IsDead()) {
			delete *bulletIterator;
			bulletIterator = bullets_.erase(bulletIterator);
		} else {
			++bulletIterator;
		}
	}

	// 蟾ｦ蜿ｳ遘ｻ蜍募・蜉・
	if (input_->PushKey(DIK_LEFT)) {
		move_.x -= kMoveSpeed;
	}

	if (input_->PushKey(DIK_RIGHT)) {
		move_.x += kMoveSpeed;
	}

	// 荳贋ｸ狗ｧｻ蜍募・蜉・
	if (input_->PushKey(DIK_UP)) {
		move_.y += kMoveSpeed;
	}

	if (input_->PushKey(DIK_DOWN)) {
		move_.y -= kMoveSpeed;
	}

	// 遘ｻ蜍募・逅・
	worldTransform_.translation_.x += move_.x;
	worldTransform_.translation_.y += move_.y;

	// 逕ｻ髱｢螟悶↓蜃ｺ縺ｪ縺・ｈ縺・↓蠎ｧ讓吶ｒ蛻ｶ髯舌☆繧・
	worldTransform_.translation_.x =
		std::clamp(worldTransform_.translation_.x, kLowerLimitX, kUpperLimitX);
	worldTransform_.translation_.y =
		std::clamp(worldTransform_.translation_.y, kLowerLimitY, kUpperLimitY);

	Rotate();
	Attack();

	// 陦悟・縺ｮ譖ｴ譁ｰ
	worldTransformMatrix(worldTransform_);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
}

void Player::Rotate() {
	// 蝗櫁ｻ｢騾溷ｺｦ
	constexpr float kRotSpeed = 0.02f;

	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}

	if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}
}

void Player::Draw(const Camera& camera) {
	assert(model_ != nullptr);

	model_->Draw(worldTransform_, camera, textureHandle_);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(camera);
	}
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {
		KamataEngine::Vector3 position = worldTransform_.translation_;
		constexpr float kBulletSpeed = 1.0f;
		KamataEngine::Vector3 velocity{0.0f, 0.0f, kBulletSpeed};

		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		auto newBullet = new PlayerBullet();
		newBullet->Initialize(model_, position, velocity);

		bullets_.push_back(newBullet);
	}
}

Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	bullets_.clear();
}

Player::Player() {
}
