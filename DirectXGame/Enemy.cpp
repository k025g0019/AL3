#include "Enemy.h"
#include <algorithm>
#include <cassert>
#include <math/MathUtility.h>
#include  "Player.h"
using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void Enemy::Initialize(Model* model, uint32_t textureHandle) {
	// 外部から受け取った必要データの記録
	model_ = model;
	textureHandle_ = textureHandle;


	// 必須データの確認
	assert(model_ != nullptr);
	approachPhaseInitialize();


	// ワールド座標の初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.translation_ = {2.0f, 0.0f, 100.0f};
	worldTransform_.matWorld_ = MakeScaleMatrix(worldTransform_.scale_) * MakeRotateXMatrix(worldTransform_.rotation_.x)
		* MakeRotateYMatrix(worldTransform_.rotation_.y) *
		MakeRotateZMatrix(worldTransform_.rotation_.z) * MakeTranslateMatrix(worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Enemy::Update() {
	move_ = {0.0f, 0.0f, 0.0f};
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	switch (phase_) {
	case Phase::Approach:
		move_.z = kMoveSpeed;
		worldTransform_.translation_.z -= move_.z;

		if (worldTransform_.translation_.z < 0.0f) {
			phase_ = Phase::Leave;
		}
		break;
	default:


	case Phase::Leave:
		move_.z = kMoveSpeed;
		worldTransform_.translation_.z += move_.z;
		break;
	}
	fireTimer--;

	if (fireTimer <= 0) {
		Fire();
		fireTimer = kFireInterval;
	}

	worldTransformMatrix(worldTransform_);

	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}
}

void Enemy::Draw(const Camera& camera) {
	assert(model_!=nullptr);
	model_->Draw(worldTransform_, camera, textureHandle_);

	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(camera);
	}
}

void Enemy::Fire() {
	assert(player_);
	Vector3 position = worldTransform_.translation_;
	constexpr float kBulletSpeed = 0.5f;
	Vector3 velocity(0.0f, 0.0f, -kBulletSpeed);

	velocity = TransformNormal(velocity, MakeRotateYMatrix(worldTransform_.rotation_.y));

	const Vector3 playerPosition = player_->GetWorldPosition();
	const Vector3 enemyWorldPosition = GetWorldPosition();

	Vector3 toPlayer = playerPosition - enemyWorldPosition;
	Vector3 direction = Normalize(toPlayer);

	velocity = direction * kBulletSpeed;

	auto newBullet = new EnemyBullet();
	newBullet->Initialize(model_, position, velocity);

	bullets_.push_back(newBullet);
}

Enemy::~Enemy() {
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
	bullets_.clear();
}

void Enemy::approachPhaseInitialize() {
	fireTimer = 60;
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
