#pragma once

#include  <list>

#include "KamataEngine.h"

#include  "PlayerBullet.h"

class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle);

	// 更新
	void Update();

	//回転
	void Rotate();

	// 描画
	void Draw(const KamataEngine::Camera& camera);


	void Attack();

	std::list<PlayerBullet*> bullets_;
	Player();
	~Player();
	// ワールド座標の取得
	const KamataEngine::Vector3& GetWorldPosition() const { return worldTransform_.translation_; }

	PlayerBullet* bullet_ = nullptr;

	KamataEngine::Vector3 GetworldPosition();

private:
	// メンバ変数
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Matrix4x4 worldTransformMatrix_;

	uint32_t textureHandle_ = 0;

	// 移動ベクトル
	KamataEngine::Vector3 move_ = {0.0f, 0.0f, 0.0f};

	// 定数
	static constexpr float kMoveSpeed = 0.2f;
	static constexpr float kLowerLimitX = -15.0f;
	static constexpr float kUpperLimitX = 15.0f;
	static constexpr float kLowerLimitY = -15.0f;
	static constexpr float kUpperLimitY = 15.0f;
};
