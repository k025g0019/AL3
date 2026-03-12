#pragma once

#include "KamataEngine.h"
#include "Matrix4x4.h"

#include <3d/WorldTransform.h>

class MapChipField;

// プレイヤー本体（移動・衝突・描画）
class Player {
public:
	// 描画用カメラ（借用）
	KamataEngine::Camera* camera_ = nullptr;

	Player();
	~Player();

	// 現在速度
	KamataEngine::Vector3 velocity_ = {};

	// 基本処理
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	const KamataEngine::WorldTransform& GetWorldTransform() const;

	// 向き状態
	enum class LRDirection {
		kRigh,
		KLeft,
	};

	// 当たり判定で使う四隅
	enum Corner {
		kTopLeft,
		kTopRight,
		kBottomLeft,
		kBottomRight,
		kNumCorners,
	};

	// マップ衝突結果
	struct CollisionMapInfo {
		bool ceiling = false;
		bool Landing = false;
		bool wall = false;
		KamataEngine::Vector3 movement;
	};

	LRDirection lrDirection = LRDirection::kRigh;
	float turnFirstRotationY = 0.0f;
	float turnTimer_ = 0.0f;
	bool onGround_ = true;

	// 移動パラメータ
	static inline const float kGravity = 9.8f;
	static inline const float kLimitFallSpeed = 10.0f;
	static inline const float kJumpAcceleration = 105.0f;
	static inline const float kTimeTurn = 0.3f;
	static inline const float kAttenu = 0.1f;
	static inline const float kLimitRunSpeed = 80.0f;
	static inline const float kAttenuationLanding = 0.5f;

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 当たり判定サイズ
	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;
	static inline const float kBlank = 0.01f;
	static inline const float kGroundProbeDepth = 0.05f;

	// マップ衝突
	void MapCollisionUp(CollisionMapInfo& info);
	void MapCollisionDown(CollisionMapInfo& info);
	void MapCollisionRight(CollisionMapInfo& info);
	void MapCollisionLeft(CollisionMapInfo& info);
	void MapCollisionDetection(CollisionMapInfo& info);
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);
	void ApplyCollision(const CollisionMapInfo& info);
	void ProcessCeilingHit(const CollisionMapInfo& info);
	void groundStateSwiching(const CollisionMapInfo& info);

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
	MapChipField* mapChipField_ = nullptr;
};
