#include "Player.h"

#include <algorithm>
#include <numbers>

#include "MapChipField.h"
#define NOMINMAX
namespace KamataEngine {
class Model;
}

using namespace KamataEngine;
KamataEngine::Vector3 Add(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b) {
	KamataEngine::Vector3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

Player::Player() {}
Player::~Player() {}

// 初期化
void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	camera_ = camera;
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.translation_ = position;
}
// 更新
void Player::Update() {

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	float deltaTime = 1.0f / 60.0f; // 仮。実際はフレーム時間


	// 着地フラグ
	bool laning = false;

	// 地面との当たり判定
	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 2.0f) {
			laning = true;
			worldTransform_.translation_.y = 2.0f;
			velocity_.y = 0.0f;
		}
	}

	if (onGround_) {

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenu);
				}
				if (lrDirection != LRDirection::kRigh) {
					lrDirection = LRDirection::kRigh;
					turnFirstRotationY = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x += 50.0f;
			}
			if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenu);
				}

				if (lrDirection != LRDirection::KLeft) {
					lrDirection = LRDirection::KLeft;
					turnFirstRotationY = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x -= 50.0f;
			}
			

			
			if (turnTimer_ > 0.0f) {

				turnTimer_ -= deltaTime;
				turnTimer_ = (std::max)(turnTimer_, 0.0f);

				float t = 1.0f - (turnTimer_ / kTimeTurn);

				float destinationRotationYTable[]{
				    std::numbers::pi_v<float> / 2.0f,
				    std::numbers::pi_v<float> * 3.0f / 2.0f,
				};

				float destinationRotationY = destinationRotationYTable[static_cast<int>(lrDirection)];

				worldTransform_.rotation_.y = std::lerp(turnFirstRotationY, destinationRotationY, t);
			}

			velocity_.x += acceleration.x * deltaTime;
			velocity_.y += acceleration.y * deltaTime;

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			velocity_.x *= (1.0f - kAttenu);
		}

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
	worldTransform_.TransferMatrix();
}
// 描画
void Player::Draw() { model_->Draw(worldTransform_, *camera_); }
// 	delete debugCamera_;
// 	delete player_;
// }
const WorldTransform& Player::GetWorldTransform() const { return worldTransform_; }


void Player::MapCollisionDetection(CollisionMapInfo& info) {

	std::array<Vector3, 4> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	// 上方向以外はスキップ
	if (info.movement.y <= 0) {
		return;
	}

	bool hit = false;

	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;

	// 左上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopRight]);

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 衝突時
	if (hit) {

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);

		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		float playerY = worldTransform_.translation_.y;

		info.movement.y = rect.bottom - (playerY + kHeight / 2.0f) - kBlank;

		info.ceiling = true;
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorners] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
	};

	KamataEngine::Vector3 result;
	result.x = center.x + offsetTable[(int)corner].x;
	result.y = center.y + offsetTable[(int)corner].y;
	result.z = center.z + offsetTable[(int)corner].z;

	return result;
}

void Player::ApplyCollision(const CollisionMapInfo& info) {

	worldTransform_.translation_.x += info.movement.x;
	worldTransform_.translation_.y += info.movement.y;
	worldTransform_.translation_.z += info.movement.z;
}

void Player::ProcessCeilingHit(const CollisionMapInfo& info) {

	if (info.ceiling) {
		velocity_.y = 0.0f;
	}
}