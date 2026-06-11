#pragma once

#include "KamataEngine.h"

class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle);

	// 更新
	void Update();

	// 描画
	void Draw(const KamataEngine::Camera& camera);

	// ワールド座標の取得
	const KamataEngine::Vector3& GetWorldPosition() const { return worldTransform_.translation_; }

private:
	// メンバ変数
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	uint32_t textureHandle_ = 0u;

	// 移動ベクトル
	KamataEngine::Vector3 move_ = {0.0f, 0.0f, 0.0f};

	// 定数
	static inline constexpr float kMoveSpeed = 0.2f;
	static inline constexpr float kLowerLimitX = 0.0f;
	static inline constexpr float kUpperLimitX = 1200.0f;
	static inline constexpr float kLowerLimitY = 0.0f;
	static inline constexpr float kUpperLimitY = 720.0f;
};
