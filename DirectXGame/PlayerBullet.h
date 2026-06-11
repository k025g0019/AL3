#pragma once
#include <algorithm>
#include <math/MathUtility.h>
#include "KamataEngine.h"

class PlayerBullet {
public:
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position);

	void Update();

	void Draw(const KamataEngine::Camera& camera);

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	uint32_t textureHandle_ = 0u;
};
