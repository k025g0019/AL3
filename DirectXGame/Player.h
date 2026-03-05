#pragma once
#include "KamataEngine.h"
#include "Matrix4x4.h"
#include <3d/WorldTransform.h>

class MapChipField;

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

	//角
	enum Corner {
		kTopLeft,
		kTopRight,
		kBottomLeft,
		kBottomRight,

		kNumCorners,
	};

	
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

	// 重力加速度
	static inline const float kGravity = 9.8f;
	// 落下速度の上限
	static inline const float kLimitFallSpeed = 10.0f;
	// ジャンプ加速度
	static inline const float kJumpAcceleration = 50.0f;
	// 方向転換にかかる時間
	static inline const float kTimeTurn = 0.3f;

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// キャラクターの当たり判定のサイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	void MapCollisionDetection(CollisionMapInfo& info);
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);
	static inline const float kBlank = 0.01f;

	void ApplyCollision(const CollisionMapInfo& info);
	void ProcessCeilingHit(const CollisionMapInfo& info);
	KamataEngine::WorldTransform worldTransform_;

	static inline const float kAttenu = 0.1f;
	static inline const float kLimitRunSpeed = 10.0f;
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;


};
