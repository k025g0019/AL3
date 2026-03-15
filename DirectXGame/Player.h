#pragma once

#include "AABB.h"
#include "KamataEngine.h"
#include "Matrix4x4.h"

#include <3d/WorldTransform.h>

class Enemy;
class MapChipField;

//====================
// プレイヤー
//====================
/*
移動・衝突・描画を担当するクラス
*/
class Player {
public:
	//====================
	// 変数
	//====================
	/* 描画用カメラ */
	KamataEngine::Camera* camera_ = nullptr;
	/* 現在速度 */
	KamataEngine::Vector3 velocity_ = {};

	/* 向き状態 */
	enum class LRDirection {
		kRigh,
		KLeft,
	};

	/* 当たり判定で使う四隅 */
	enum Corner {
		kTopLeft,
		kTopRight,
		kBottomLeft,
		kBottomRight,
		kNumCorners,
	};

	/* マップ衝突の結果 */
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

	//====================
	// 定数
	//====================
	/* 移動パラメータ */
	static inline const float kGravity = 9.8f;              // 重力加速度
	static inline const float kLimitFallSpeed = 10.0f;      // 落下速度の下限（絶対値の上限）
	static inline const float kJumpAcceleration = 105.0f;   // ジャンプ時に加える上向き加速度
	static inline const float kTimeTurn = 0.3f;             // 方向転換アニメーション時間[秒]
	static inline const float kAttenu = 0.1f;               // 地上移動時の減衰率
	static inline const float kLimitRunSpeed = 80.0f;       // 横移動速度の上限
	static inline const float kAttenuationLanding = 0.5f;   // 着地時の横速度減衰率

	/* 当たり判定サイズ */
	static inline const float kWidth = 2.0f;                // プレイヤー当たり判定の幅
	static inline const float kHeight = 2.0f;               // プレイヤー当たり判定の高さ
	static inline const float kBlank = 0.01f;               // めり込み防止の余白
	static inline const float kGroundProbeDepth = 0.05f;    // 接地判定を下に伸ばす長さ

	//====================
	// 関数
	//====================
	Player();  // コンストラクタ
	~Player(); // デストラクタ

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position); // 初期化
	void Update();                                                                                                  // 更新
	void Draw();                                                                                                    // 描画

	const KamataEngine::WorldTransform& GetWorldTransform() const; // ワールド変換を取得
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; } // 速度を取得
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; } // マップ参照を設定
	bool IsDead() const { return isDead_; }

	/* マップ衝突 */
	void MapCollisionUp(CollisionMapInfo& info);                                    // 上方向の衝突を判定・補正
	void MapCollisionDown(CollisionMapInfo& info);                                  // 下方向の衝突を判定・補正
	void MapCollisionRight(CollisionMapInfo& info);                                 // 右方向の衝突を判定・補正
	void MapCollisionLeft(CollisionMapInfo& info);                                  // 左方向の衝突を判定・補正
	void MapCollisionDetection(CollisionMapInfo& info);                             // 4方向の衝突判定をまとめて実行
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner); // 指定した隅の座標を取得
	void ApplyCollision(const CollisionMapInfo& info);                              // 補正後の移動量を座標へ反映
	void ProcessCeilingHit(const CollisionMapInfo& info);                           // 天井ヒット時の速度処理
	void groundStateSwiching(const CollisionMapInfo& info);                         // 接地/空中状態を切り替え

	/* AABB */
	KamataEngine::Vector3 GetWorldPosition() const; // 座標を取得
	AABB GetAABB() const;                           // AABBを取得
	void OnCollision(const Enemy* enemy);           // 敵と衝突した時の処理

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
	MapChipField* mapChipField_ = nullptr;
	bool isDead_ = false;
};
