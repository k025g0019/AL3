#include "Player.h"

#include <algorithm>
#include <math/MathUtility.h>
#include "Vector&Matrix.h"

using namespace KamataEngine;

void Player::Initialize(Model* model, uint32_t textureHandle) {
	// 引数で受け取った値をメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;
	input_ = Input::GetInstance();

	// 必須データの存在確認
	assert(model_ != nullptr);
	assert(input_ != nullptr);

	// ワールド座標の初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransformMatrix(worldTransform_);
}

void Player::Update() {
	// 毎フレーム移動量をリセットする
	move_ = {0.0f, 0.0f, 0.0f};

	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}

		return false;
	});

	// 左右移動入力
	if (input_->PushKey(DIK_LEFT)) {
		move_.x -= kMoveSpeed;
	}

	if (input_->PushKey(DIK_RIGHT)) {
		move_.x += kMoveSpeed;
	}

	// 上下移動入力
	if (input_->PushKey(DIK_UP)) {
		move_.y += kMoveSpeed;
	}

	if (input_->PushKey(DIK_DOWN)) {
		move_.y -= kMoveSpeed;
	}

	// 移動処理
	worldTransform_.translation_.x += move_.x;
	worldTransform_.translation_.y += move_.y;

	// 画面外に出ないように座標を制限する
	worldTransform_.translation_.x =
		std::clamp(worldTransform_.translation_.x, kLowerLimitX, kUpperLimitX);
	worldTransform_.translation_.y =
		std::clamp(worldTransform_.translation_.y, kLowerLimitY, kUpperLimitY);

	Rotate();
	Attack();

	// 行列の更新
	worldTransformMatrix(worldTransform_);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
}

void Player::Rotate() {
	// 回転速度
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
