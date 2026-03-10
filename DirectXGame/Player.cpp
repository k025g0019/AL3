#include "Player.h"

#include <algorithm>
#include <numbers>

#include "MapChipField.h"
#define NOMINMAX

namespace KamataEngine {
class Model;
}

using namespace KamataEngine;

Vector3 Add(const Vector3& a, const Vector3& b) {
	Vector3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

Player::Player() {}

Player::~Player() {}

// 初期化
void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	camera_ = camera;
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.translation_ = position;
}

// 更新
void Player::Update() {
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	float deltaTime = 1.0f / 60.0f; // 仮。実際はフレーム時間

	// 着地フラグ
	bool laning = false;



	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

		// 横方向の入力
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};
			// 右方向への入力
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左方向への速度があるときは減衰する
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenu);
				}
				// 方向転換開始
				if (lrDirection != LRDirection::kRigh) {
					lrDirection = LRDirection::kRigh;
					turnFirstRotationY = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				// 右方向への加速度
				acceleration.x += 50.0f;
			}
			// 左方向への入力
			if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右方向への速度があるときは減衰する
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenu);
				}

				// 方向転換開始
				if (lrDirection != LRDirection::KLeft) {
					lrDirection = LRDirection::KLeft;
					turnFirstRotationY = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				// 左方向への加速度
				acceleration.x -= 50.0f;
			}
			// 方向転換中
			if (turnTimer_ > 0.0f) {
				// 方向転換のタイマーを減らす
				turnTimer_ -= deltaTime;
				turnTimer_ = (std::max)(turnTimer_, 0.0f);

				// 方向転換の進行度
				float t = 1.0f - (turnTimer_ / kTimeTurn);

				//	目的の角度のテーブル
				float destinationRotationYTable[]{
				    std::numbers::pi_v<float> / 2.0f,
				    std::numbers::pi_v<float> * 3.0f / 2.0f,
				};

				// 目的の角度
				float destinationRotationY = destinationRotationYTable[static_cast<int>(lrDirection)];
				// 角度を線形補間
				worldTransform_.rotation_.y = std::lerp(turnFirstRotationY, destinationRotationY, t);
			}

			// 加速
			velocity_.x += acceleration.x * deltaTime;
			velocity_.y += acceleration.y * deltaTime;

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 減速
			velocity_.x *= (1.0f - kAttenu);
		}

		// ジャンプ開始
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y += kJumpAcceleration;
		}
	} else {
		// 着地

		if (laning) {
			// めり込み補正
			worldTransform_.translation_.y = 2.0f;

			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenu);

			// 下方向速度をリセット
			velocity_.y = 0.0f;

			// 接地状態に移行
			onGround_ = true;
		}

		velocity_.y += -kGravity;
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}
	// 衝突情報の初期化
	CollisionMapInfo collisionMapInfo{};

	// 移動量に速度の値をコピー
	collisionMapInfo.movement.x = velocity_.x * deltaTime;
	collisionMapInfo.movement.y = velocity_.y * deltaTime;
	collisionMapInfo.movement.z = velocity_.z * deltaTime;

	// マップチップとの当たり判定
	MapCollisionDetection(collisionMapInfo);

	ApplyCollision(collisionMapInfo);
	ProcessCeilingHit(collisionMapInfo);
	groundStateSwiching(collisionMapInfo);
	worldTransform_.TransferMatrix();
}

// 描画
void Player::Draw() { model_->Draw(worldTransform_, *camera_); }
// 	delete debugCamera_;
// 	delete player_;
// }
const WorldTransform& Player::GetWorldTransform() const { return worldTransform_; }

// マップチップとの当たり判定
void Player::MapCollisionDetection(CollisionMapInfo& info) {
	MapCollisionUp(info);
	MapCollisionDown(info);
}

// プレイヤーの当たり判定の角の座標を求める
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	// 角のオフセットテーブル
	Vector3 offsetTable[kNumCorners] = {
        {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
        {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
    };

	// 角の座標を求める
	Vector3 result;
	result.x = center.x + offsetTable[static_cast<int>(corner)].x;
	result.y = center.y + offsetTable[static_cast<int>(corner)].y;
	result.z = center.z + offsetTable[static_cast<int>(corner)].z;

	// 結果を返す
	return result;
}

// 衝突の結果をプレイヤーに反映する
void Player::ApplyCollision(const CollisionMapInfo& info) {
	// 移動量をプレイヤーの座標に反映
	worldTransform_.translation_.x += info.movement.x;
	worldTransform_.translation_.y += info.movement.y;
	worldTransform_.translation_.z += info.movement.z;
}

// 天井に当たったときの処理
void Player::ProcessCeilingHit(const CollisionMapInfo& info) {
	// 天井に当たっていなければスキップ
	if (info.ceiling) {
		// 天井に当たったときの処理
		velocity_.y = 0.0f;
	}
}

void Player::MapCollisionUp(CollisionMapInfo& info) {

	// プレイヤーの当たり判定の角の座標を求める
	std::array<Vector3, 4> positionsNew;

	// 移動後のプレイヤーの当たり判定の角の座標を求める
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	// 上方向以外はスキップ
	if (info.movement.y <= 0) {
		return;
	}

	// 衝突しているか
	bool hit = false;

	// マップチップのインデックスと種類を取得
	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;

	// 左上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);

	// マップチップの種類を取得
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	// ブロックなら衝突
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopRight]);

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);


	// ブロックなら衝突
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 衝突時
	if (hit) {
		// 衝突しているマップチップのインデックスを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);

		// 衝突しているマップチップの矩形を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		// プレイヤーのY座標を取得
		float playerY = worldTransform_.translation_.y;

		// プレイヤーの当たり判定の上辺とマップチップの下辺の距離を求める
		info.movement.y = rect.bottom - (playerY + kHeight / 2.0f) - kBlank;

		// 衝突しているときは天井に当たっているとみなす
		info.ceiling = true;
	}
}

void Player::MapCollisionDown(CollisionMapInfo& info) {

	if (info.movement.y >= 0) {
		return;
	}

	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	MapChipField::IndexSet indexSetLeft = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kBottomLeft]);
	MapChipField::IndexSet indexSetRight = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kBottomRight]);

	bool isHitLeft = mapChipField_->GetMapChipTypeByIndex(indexSetLeft.xIndex, indexSetLeft.yIndex) == MapChipType::kBlock;
	bool isHitRight = mapChipField_->GetMapChipTypeByIndex(indexSetRight.xIndex, indexSetRight.yIndex) == MapChipType::kBlock;
	if (!isHitLeft && !isHitRight) {
		return;
	}

	float playerBottom = worldTransform_.translation_.y - kHeight / 2.0f;
	float yMoveToLand = info.movement.y;

	if (isHitLeft) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSetLeft.xIndex, indexSetLeft.yIndex);
		yMoveToLand = (std::max)(yMoveToLand, rect.top - playerBottom + kBlank);
	}
	if (isHitRight) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSetRight.xIndex, indexSetRight.yIndex);
		yMoveToLand = (std::max)(yMoveToLand, rect.top - playerBottom + kBlank);
	}

	info.movement.y = (std::min)(0.0f, yMoveToLand);
	info.Landing = true;
}

void Player::groundStateSwiching(const CollisionMapInfo& info) {

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
			return;
		}

		Vector3 footOffset = {0.0f, -kBlank, 0.0f};
		Vector3 leftBottom = Add(CornerPosition(worldTransform_.translation_, kBottomLeft), footOffset);
		Vector3 rightBottom = Add(CornerPosition(worldTransform_.translation_, kBottomRight), footOffset);

		MapChipField::IndexSet leftIndex = mapChipField_->GetMapChipIndexSetByPosition(leftBottom);
		MapChipField::IndexSet rightIndex = mapChipField_->GetMapChipIndexSetByPosition(rightBottom);
		bool hitLeft = mapChipField_->GetMapChipTypeByIndex(leftIndex.xIndex, leftIndex.yIndex) == MapChipType::kBlock;
		bool hitRight = mapChipField_->GetMapChipTypeByIndex(rightIndex.xIndex, rightIndex.yIndex) == MapChipType::kBlock;

		if (!hitLeft && !hitRight) {
			onGround_ = false;
		}
	} else {
		if (info.Landing) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}
