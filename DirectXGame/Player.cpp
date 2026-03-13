//====================
// プレイヤー 実装
//====================
#include "Player.h"

#include <algorithm>
#include <numbers>

#include "MapChipField.h"
#define NOMINMAX

namespace KamataEngine {
class Model;
}

using namespace KamataEngine;

//====================
// ベクトル補助
//====================
/*
2つのVector3を加算して返す
*/
Vector3 Add(const Vector3& a, const Vector3& b) {
	// 計算結果を格納する変数
	Vector3 result;
	// 各成分を加算
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	// 加算結果を返す
	return result;
}

//====================
// 生成破棄
//====================
Player::Player() {}
Player::~Player() {}

//====================
// 初期化
//====================
/*
モデル・カメラ・初期座標を設定する
*/
void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// モデルポインタの有効性を確認
	assert(model);
	// 参照を保持
	camera_ = camera;
	model_ = model;

	// ワールド変換の初期設定
	worldTransform_.Initialize();
	// 初期向き（右向き）
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	// プレイヤー表示サイズ
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	// 初期座標
	worldTransform_.translation_ = position;
}

//====================
// 更新
//====================
/*
入力・重力・マップ衝突を反映して座標を更新する
*/
void Player::Update() {
	// 現在のSRTから行列を一度作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 固定デルタ時間
	float deltaTime = 1.0f / 60.0f; // 1フレーム分の固定デルタ時間

	// 着地補助フラグ（現状は未使用）
	bool laning = false;

	// 接地中の挙動
	if (onGround_) {
		// 上向き速度が残っていたら空中扱いへ
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

		// 左右入力がある場合の移動処理
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 1フレーム分の加速度
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 逆方向移動中なら減衰
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenu);
				}
				// 向き変更開始時の情報を保存
				if (lrDirection != LRDirection::kRigh) {
					lrDirection = LRDirection::kRigh;
					turnFirstRotationY = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				// 右向き加速度
				acceleration.x += 50.0f;
			}
			if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 逆方向移動中なら減衰
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenu);
				}

				// 向き変更開始時の情報を保存
				if (lrDirection != LRDirection::KLeft) {
					lrDirection = LRDirection::KLeft;
					turnFirstRotationY = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				// 左向き加速度
				acceleration.x -= 50.0f;
			}
			// 方向転換アニメーション
			if (turnTimer_ > 0.0f) {
				turnTimer_ -= deltaTime;
				turnTimer_ = (std::max)(turnTimer_, 0.0f);

				// 経過率を0~1で計算
				float t = 1.0f - (turnTimer_ / kTimeTurn);

				// 各向きの目標角度テーブル
				float destinationRotationYTable[]{
				    std::numbers::pi_v<float> / 2.0f,
				    std::numbers::pi_v<float> * 3.0f / 2.0f,
				};

				// 現在角度を補間
				float destinationRotationY = destinationRotationYTable[static_cast<int>(lrDirection)];
				worldTransform_.rotation_.y = std::lerp(turnFirstRotationY, destinationRotationY, t);
			}

			// 加速度から速度を更新
			velocity_.x += acceleration.x * deltaTime;
			velocity_.y += acceleration.y * deltaTime;

			// 横速度を上限で制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 入力がないときは自然減衰
			velocity_.x *= (1.0f - kAttenu);
		}

		// ジャンプ入力
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y += kJumpAcceleration;
		}
	} else {

		// 着地補助（現状は無効）
		if (laning) {
			worldTransform_.translation_.y = 2.0f;

			velocity_.x *= (1.0f - kAttenu);
			velocity_.y = 0.0f;
			onGround_ = true;
		}

		// 空中時は重力を適用
		velocity_.y += -kGravity;
		// 落下速度の下限を制限
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}

	// 1フレーム移動量を作成
	CollisionMapInfo collisionMapInfo{};
	collisionMapInfo.movement.x = velocity_.x * deltaTime;
	collisionMapInfo.movement.y = velocity_.y * deltaTime;
	collisionMapInfo.movement.z = velocity_.z * deltaTime;

	// マップ衝突判定
	MapCollisionDetection(collisionMapInfo);

	// 衝突後の補正を反映
	ApplyCollision(collisionMapInfo);
	// 天井衝突時の速度処理
	ProcessCeilingHit(collisionMapInfo);
	// 接地状態を更新
	groundStateSwiching(collisionMapInfo);

	// 最終行列を更新してGPUへ転送
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

//====================
// 描画
//====================
/*
プレイヤーモデルを描画する
*/
void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

const WorldTransform& Player::GetWorldTransform() const { return worldTransform_; }

//====================
// マップ衝突まとめ
//====================
/*
上下左右の衝突判定を順に実行する
*/
void Player::MapCollisionDetection(CollisionMapInfo& info) {
	// 上下左右の順に判定
	MapCollisionUp(info);
	MapCollisionDown(info);
	MapCollisionRight(info);
	MapCollisionLeft(info);
}

//====================
// 角座標取得
//====================
/*
中心座標と指定角から角のワールド座標を返す
*/
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	// 中心から各角へのオフセット
	Vector3 offsetTable[kNumCorners] = {
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
	};

	// 指定角の座標を計算
	Vector3 result;
	result.x = center.x + offsetTable[static_cast<int>(corner)].x;
	result.y = center.y + offsetTable[static_cast<int>(corner)].y;
	result.z = center.z + offsetTable[static_cast<int>(corner)].z;

	// 計算結果を返す
	return result;
}

//====================
// 座標反映
//====================
/*
衝突補正後の移動量を実座標に反映する
*/
void Player::ApplyCollision(const CollisionMapInfo& info) {
	// 各軸に移動量を加算
	worldTransform_.translation_.x += info.movement.x;
	worldTransform_.translation_.y += info.movement.y;
	worldTransform_.translation_.z += info.movement.z;
}

//====================
// 天井ヒット処理
//====================
/*
天井に当たったら上方向速度を止める
*/
void Player::ProcessCeilingHit(const CollisionMapInfo& info) {
	if (info.ceiling) {
		velocity_.y = 0.0f;
	}
}

//====================
// 上方向衝突
//====================
void Player::MapCollisionUp(CollisionMapInfo& info) {
	// 移動後4隅座標を計算
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	// 上移動でなければ不要
	if (info.movement.y <= 0) {
		return;
	}

	// 上辺2点のブロック判定
	bool hit = false;
	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopRight]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込み量を解消するようY移動量を補正
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float playerY = worldTransform_.translation_.y;
		info.movement.y = rect.bottom - (playerY + kHeight / 2.0f) - kBlank;
		// 天井ヒットフラグ
		info.ceiling = true;
	}
}

//====================
// 下方向衝突
//====================
void Player::MapCollisionDown(CollisionMapInfo& info) {
	// 下移動でなければ不要
	if (info.movement.y >= 0) {
		return;
	}

	// 移動後4隅座標を計算
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	// 足元プローブで地面を判定
	Vector3 landingProbeOffset = {0.0f, -kGroundProbeDepth, 0.0f};
	MapChipField::IndexSet indexSetLeft = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kBottomLeft], landingProbeOffset));
	MapChipField::IndexSet indexSetRight = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kBottomRight], landingProbeOffset));

	bool isHitLeft = mapChipField_->GetMapChipTypeByIndex(indexSetLeft.xIndex, indexSetLeft.yIndex) == MapChipType::kBlock;
	bool isHitRight = mapChipField_->GetMapChipTypeByIndex(indexSetRight.xIndex, indexSetRight.yIndex) == MapChipType::kBlock;
	if (!isHitLeft && !isHitRight) {
		return;
	}

	// 着地に必要なY移動量を計算
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

	// 補正移動量と着地フラグを設定
	info.movement.y = yMoveToLand;
	info.Landing = true;
}

//====================
// 右方向衝突
//====================
void Player::MapCollisionRight(CollisionMapInfo& info) {
	// 右移動でなければ不要
	if (info.movement.x <= 0) {
		return;
	}

	// 移動後4隅座標を計算
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	MapChipField::IndexSet topIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopRight]);
	MapChipField::IndexSet bottomIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kBottomRight]);

	bool hitTop = mapChipField_->GetMapChipTypeByIndex(topIndex.xIndex, topIndex.yIndex) == MapChipType::kBlock;
	bool hitBottom = mapChipField_->GetMapChipTypeByIndex(bottomIndex.xIndex, bottomIndex.yIndex) == MapChipType::kBlock;
	if (!hitTop && !hitBottom) {
		return;
	}

	// 壁直前で止まるX移動量を計算
	float playerRight = worldTransform_.translation_.x + kWidth / 2.0f;
	float xMoveToWall = info.movement.x;

	if (hitTop) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(topIndex.xIndex, topIndex.yIndex);
		xMoveToWall = (std::min)(xMoveToWall, rect.left - playerRight - kBlank);
	}
	if (hitBottom) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(bottomIndex.xIndex, bottomIndex.yIndex);
		xMoveToWall = (std::min)(xMoveToWall, rect.left - playerRight - kBlank);
	}

	// 壁ヒット時の補正と横速度停止
	info.movement.x = xMoveToWall;
	info.wall = true;
	velocity_.x = 0.0f;
}

//====================
// 左方向衝突
//====================
void Player::MapCollisionLeft(CollisionMapInfo& info) {
	// 左移動でなければ不要
	if (info.movement.x >= 0) {
		return;
	}

	// 移動後4隅座標を計算
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	MapChipField::IndexSet topIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);
	MapChipField::IndexSet bottomIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kBottomLeft]);

	bool hitTop = mapChipField_->GetMapChipTypeByIndex(topIndex.xIndex, topIndex.yIndex) == MapChipType::kBlock;
	bool hitBottom = mapChipField_->GetMapChipTypeByIndex(bottomIndex.xIndex, bottomIndex.yIndex) == MapChipType::kBlock;
	if (!hitTop && !hitBottom) {
		return;
	}

	// 壁直前で止まるX移動量を計算
	float playerLeft = worldTransform_.translation_.x - kWidth / 2.0f;
	float xMoveToWall = info.movement.x;

	if (hitTop) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(topIndex.xIndex, topIndex.yIndex);
		xMoveToWall = (std::max)(xMoveToWall, rect.right - playerLeft + kBlank);
	}
	if (hitBottom) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(bottomIndex.xIndex, bottomIndex.yIndex);
		xMoveToWall = (std::max)(xMoveToWall, rect.right - playerLeft + kBlank);
	}

	// 壁ヒット時の補正と横速度停止
	info.movement.x = xMoveToWall;
	info.wall = true;
	velocity_.x = 0.0f;
}

//====================
// 接地状態切り替え
//====================
void Player::groundStateSwiching(const CollisionMapInfo& info) {
	// すでに接地中の場合
	if (onGround_) {
		// 上昇したら空中状態へ
		if (velocity_.y > 0.0f) {
			onGround_ = false;
			return;
		}

		// 足元2点で地面継続を確認
		Vector3 footOffset = {0.0f, -kGroundProbeDepth, 0.0f};
		Vector3 leftBottom = Add(CornerPosition(worldTransform_.translation_, kBottomLeft), footOffset);
		Vector3 rightBottom = Add(CornerPosition(worldTransform_.translation_, kBottomRight), footOffset);

		MapChipField::IndexSet leftIndex = mapChipField_->GetMapChipIndexSetByPosition(leftBottom);
		MapChipField::IndexSet rightIndex = mapChipField_->GetMapChipIndexSetByPosition(rightBottom);
		bool hitLeft = mapChipField_->GetMapChipTypeByIndex(leftIndex.xIndex, leftIndex.yIndex) == MapChipType::kBlock;
		bool hitRight = mapChipField_->GetMapChipTypeByIndex(rightIndex.xIndex, rightIndex.yIndex) == MapChipType::kBlock;

		if (!hitLeft && !hitRight) {
			// 地面が切れたので空中へ
			onGround_ = false;
		} else {
			// 接地中は縦速度を0に固定
			velocity_.y = 0.0f;
		}
	} else {
		// 空中中に着地フラグが立ったら接地へ
		if (info.Landing) {
			onGround_ = true;
			// 着地時に横速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}

//====================
// 座標取得
//====================
/*
プレイヤーのワールド座標を返す
*/
KamataEngine::Vector3 Player::GetWorldPosition() const { return worldTransform_.translation_; }

//====================
// AABB取得
//====================
/*
プレイヤーの当たり判定AABBを返す
*/
AABB Player::GetAABB() const {
	// 中心座標と半サイズを計算
	const Vector3 center = GetWorldPosition();
	const Vector3 half = {kWidth * 0.5f, kHeight * 0.5f, kWidth * 0.5f};

	// min/max を設定
	AABB aabb;
	aabb.min = {center.x - half.x, center.y - half.y, center.z - half.z};
	aabb.max = {center.x + half.x, center.y + half.y, center.z + half.z};
	return aabb;
}

//====================
// 衝突時処理
//====================
void Player::OnCollision(const Enemy* enemy) {
	// 未使用引数
	(void)enemy;
	// 衝突時に上方向へ少し弾ませる
	velocity_.y += 10;
}
