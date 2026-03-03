#pragma once
#include "KamataEngine.h"
#include "Matrix4x4.h"
#include <3d/WorldTransform.h>
class Player {

public:
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// コンストラクタとデストラクタ
	Player();

	// デストラクタ
	~Player();

	
	KamataEngine::Vector3 velocity_ = {};
	
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();

	const KamataEngine::WorldTransform& GetWorldTransform() const;

	enum class LRDirection {
		kRigh,
		KLeft,
	};

	LRDirection lrDirection = LRDirection::kRigh;
	float turnFirstRotationY = 0.0f;
	float turnTimer_ = 0.0f;
	bool onGround_ = true;

	// 重力加速度
	static inline const float kGravity = 9.8f;
	// 落下速度の上限
	static inline const float kLimitFallSpeed = 10.0f;
	// ジャンプ加速度
	static inline const float kJumpAcceleration = 30.0f;
	// 方向転換にかかる時間
	static inline const float kTimeTurn = 0.3f;


	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

private:
	
	KamataEngine::WorldTransform worldTransform_;

	static inline const float kAttenu = 0.1f;
	static inline const float kLimitRunSpeed = 10.0f;
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
};
