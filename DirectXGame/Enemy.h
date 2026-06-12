#pragma once
#include "KamataEngine.h"
#include "worldTransform.h"

class Enemy {
public:
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle);

	void Update();
	void Draw(const KamataEngine::Camera& camera);
	const KamataEngine::Vector3& GetWorldPosition() const { return worldTransform_.translation_; }

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	uint32_t textureHandle_ = 0u;
	// 移動ベクトル
	KamataEngine::Vector3 move_ = {0.0f, 0.0f, 0.0f};
	// 定数
	static constexpr float kMoveSpeed = 0.1f;
};
