#include "Enemy.h"

#include <cassert>
#include <cmath>

#include "Matrix4x4.h"

#define PI 3.141592654f

using namespace KamataEngine;

Enemy::Enemy() {}

Enemy::~Enemy() {}

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	// プレイヤーと同じ向き基準に合わせる
	worldTransform_.rotation_.y = PI * 3.0f / 2.0f;

	velocity_.x = -kWalSpeed;
	walkTimer_ = 0.0f;
}

void Enemy::Update() {
	// 速度から位置を更新
	worldTransform_.translation_.x += velocity_.x;

	// タイマーを加算
	walkTimer_ += (1.0f / 60.0f);

	// 回転アニメーション
	float param = std::sin(PI * 2.0f * walkTimer_ / kWalkMotionTime);
	float t = (param + 1.0f) / 2.0f;
	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * t;
	float rad = degree * (PI / 180.0f);

	// 基準向き + 首振り
	worldTransform_.rotation_.x = rad;
	worldTransform_.rotation_.y = PI * 3.0f / 2.0f;
	worldTransform_.rotation_.z = 0.0f;

	// Enemyのみ回転順を調整して、見た目の上下首振りを優先
	Matrix4x4 S = MakeScaleMatrix(worldTransform_.scale_);
	Matrix4x4 Rx = MakeRotateX(worldTransform_.rotation_.x);
	Matrix4x4 Ry = MakeRotateY(worldTransform_.rotation_.y);
	Matrix4x4 T = MakeTranslateMatrix(worldTransform_.translation_);
	Matrix4x4 R = Multiply(Rx, Ry);
	worldTransform_.matWorld_ = Multiply(Multiply(S, R), T);
	worldTransform_.TransferMatrix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }