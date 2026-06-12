#include "Player.h"

#include <algorithm>
#include <math/MathUtility.h>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void Player::Initialize(Model* model, uint32_t textureHandle) {
	// 外部から受け取った必要データの記録
	model_ = model;
	textureHandle_ = textureHandle;
	input_ = Input::GetInstance();

	// 必須データの確認
	assert(model_ != nullptr);
	assert(input_ != nullptr);

	// ワールド座標の初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.matWorld_ = MakeScaleMatrix(worldTransform_.scale_) *
		MakeRotateXMatrix(worldTransform_.rotation_.x) *
		MakeRotateYMatrix(worldTransform_.rotation_.y) *
		MakeRotateZMatrix(worldTransform_.rotation_.z) *
		MakeTranslateMatrix(worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Player::Update() {
	// 移動ベクトルの初期化
	move_ = {
		0.0f,
		0.0f, 0.0f
	};
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 左右入力による移動方向の決定
	if (input_->PushKey(DIK_LEFT)) {
		move_.x -= kMoveSpeed;
	}

	if (input_->PushKey(DIK_RIGHT)) {
		move_.x += kMoveSpeed;
	}

	// 上下入力による移動方向の決定
	if (input_->PushKey(DIK_UP)) {
		move_.y += kMoveSpeed;
	}

	if (input_->PushKey(DIK_DOWN)) {
		move_.y -= kMoveSpeed;
	}

	// 移動処理
	worldTransform_.translation_.x += move_.x;
	worldTransform_.translation_.y += move_.y;

	// 画面外に出ないように移動範囲を制限
	worldTransform_.translation_.x =
		std::clamp(worldTransform_.translation_.x, kLowerLimitX, kUpperLimitX);
	worldTransform_.translation_.y =
		std::clamp(worldTransform_.translation_.y, kLowerLimitY, kUpperLimitY);

	Rotate();
	Attack();
	// ワールド行列の転送
	worldTransformMatrix(worldTransform_);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
}

void Player::Rotate() {
	// 回転処理
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
		Vector3 position = worldTransform_.translation_;
		constexpr float kBulletSpeed = 1.0f;
		Vector3 velocity(0.0f, 0.0f, kBulletSpeed);

		velocity = TransformNormal(velocity, MakeRotateYMatrix(worldTransform_.rotation_.y));

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
