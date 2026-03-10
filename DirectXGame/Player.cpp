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

// 陋ｻ譎・ｄ陋ｹ繝ｻ
void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	camera_ = camera;
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform_.translation_ = position;
}

// 隴厄ｽｴ隴・ｽｰ
void Player::Update() {
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	float deltaTime = 1.0f / 60.0f; // 闔会ｽｮ邵ｲ繧・ｽｮ貊・怙邵ｺ・ｯ郢晁ｼ釆樒ｹ晢ｽｼ郢晢｣ｰ隴弱ｋ菫｣

	// 騾ｹﾂ陜ｨ・ｰ郢晁ｼ釆帷ｹｧ・ｰ
	bool laning = false;

	if (onGround_) {
		// 郢ｧ・ｸ郢晢ｽ｣郢晢ｽｳ郢晞斡蟷戊沂繝ｻ
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

		// 隶難ｽｪ隴・ｽｹ陷ｷ莉｣繝ｻ陷茨ｽ･陷峨・
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};
			// 陷ｿ・ｳ隴・ｽｹ陷ｷ莉｣竏育ｸｺ・ｮ陷茨ｽ･陷峨・
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 陝ｾ・ｦ隴・ｽｹ陷ｷ莉｣竏育ｸｺ・ｮ鬨ｾ貅ｷ・ｺ・ｦ邵ｺ蠕娯旺郢ｧ荵昶・邵ｺ髦ｪ繝ｻ雋ょｹ・ｽ｡・ｰ邵ｺ蜷ｶ・・
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenu);
				}
				// 隴・ｽｹ陷ｷ鬘鯉ｽｻ・｢隰蟷・ｹ戊沂繝ｻ
				if (lrDirection != LRDirection::kRigh) {
					lrDirection = LRDirection::kRigh;
					turnFirstRotationY = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				// 陷ｿ・ｳ隴・ｽｹ陷ｷ莉｣竏育ｸｺ・ｮ陷会｣ｰ鬨ｾ貅ｷ・ｺ・ｦ
				acceleration.x += 50.0f;
			}
			// 陝ｾ・ｦ隴・ｽｹ陷ｷ莉｣竏育ｸｺ・ｮ陷茨ｽ･陷峨・
			if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 陷ｿ・ｳ隴・ｽｹ陷ｷ莉｣竏育ｸｺ・ｮ鬨ｾ貅ｷ・ｺ・ｦ邵ｺ蠕娯旺郢ｧ荵昶・邵ｺ髦ｪ繝ｻ雋ょｹ・ｽ｡・ｰ邵ｺ蜷ｶ・・
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenu);
				}

				// 隴・ｽｹ陷ｷ鬘鯉ｽｻ・｢隰蟷・ｹ戊沂繝ｻ
				if (lrDirection != LRDirection::KLeft) {
					lrDirection = LRDirection::KLeft;
					turnFirstRotationY = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				// 陝ｾ・ｦ隴・ｽｹ陷ｷ莉｣竏育ｸｺ・ｮ陷会｣ｰ鬨ｾ貅ｷ・ｺ・ｦ
				acceleration.x -= 50.0f;
			}
			// 隴・ｽｹ陷ｷ鬘鯉ｽｻ・｢隰蟶托ｽｸ・ｭ
			if (turnTimer_ > 0.0f) {
				// 隴・ｽｹ陷ｷ鬘鯉ｽｻ・｢隰蟶吶・郢ｧ・ｿ郢ｧ・､郢晄ｧｭ繝ｻ郢ｧ蜻茨ｽｸ蟶呻ｽ臥ｸｺ繝ｻ
				turnTimer_ -= deltaTime;
				turnTimer_ = (std::max)(turnTimer_, 0.0f);

				// 隴・ｽｹ陷ｷ鬘鯉ｽｻ・｢隰蟶吶・鬨ｾ・ｲ髯ｦ謔滂ｽｺ・ｦ
				float t = 1.0f - (turnTimer_ / kTimeTurn);

				//	騾ｶ・ｮ騾ｧ繝ｻ繝ｻ髫苓ｲ橸ｽｺ・ｦ邵ｺ・ｮ郢昴・繝ｻ郢晄じﾎ・
				float destinationRotationYTable[]{
				    std::numbers::pi_v<float> / 2.0f,
				    std::numbers::pi_v<float> * 3.0f / 2.0f,
				};

				// 騾ｶ・ｮ騾ｧ繝ｻ繝ｻ髫苓ｲ橸ｽｺ・ｦ
				float destinationRotationY = destinationRotationYTable[static_cast<int>(lrDirection)];
				// 髫苓ｲ橸ｽｺ・ｦ郢ｧ蝣､・ｷ螢ｼ・ｽ・｢髯ｬ諞ｺ菫｣
				worldTransform_.rotation_.y = std::lerp(turnFirstRotationY, destinationRotationY, t);
			}

			// 陷会｣ｰ鬨ｾ繝ｻ
			velocity_.x += acceleration.x * deltaTime;
			velocity_.y += acceleration.y * deltaTime;

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 雋ょｹδ繝ｻ
			velocity_.x *= (1.0f - kAttenu);
		}

		// 郢ｧ・ｸ郢晢ｽ｣郢晢ｽｳ郢晞斡蟷戊沂繝ｻ
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y += kJumpAcceleration;
		}
	} else {
		// 騾ｹﾂ陜ｨ・ｰ

		if (laning) {
			// 郢ｧ竏夲ｽ企恷・ｼ邵ｺ・ｿ髯ｬ諛茨ｽｭ・｣
			worldTransform_.translation_.y = 2.0f;

			// 髷ｫ・ｩ隰ｫ・ｦ邵ｺ・ｧ隶難ｽｪ隴・ｽｹ陷ｷ鮃ｹﾂ貅ｷ・ｺ・ｦ邵ｺ譴ｧ・ｸ蟷・ｽ｡・ｰ邵ｺ蜷ｶ・・
			velocity_.x *= (1.0f - kAttenu);

			// 闕ｳ蛹ｺ蟀ｿ陷ｷ鮃ｹﾂ貅ｷ・ｺ・ｦ郢ｧ蛛ｵﾎ懃ｹｧ・ｻ郢昴・繝ｨ
			velocity_.y = 0.0f;

			// 隰暦ｽ･陜ｨ・ｰ霑･・ｶ隲ｷ荵昶・驕假ｽｻ髯ｦ繝ｻ
			onGround_ = true;
		}

		velocity_.y += -kGravity;
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}
	// 髯ｦ譎会ｽｪ竏ｵ繝･陜｣・ｱ邵ｺ・ｮ陋ｻ譎・ｄ陋ｹ繝ｻ
	CollisionMapInfo collisionMapInfo{};

	// 驕假ｽｻ陷肴坩纃ｼ邵ｺ・ｫ鬨ｾ貅ｷ・ｺ・ｦ邵ｺ・ｮ陋滂ｽ､郢ｧ蛛ｵ縺慕ｹ晄鱒繝ｻ
	collisionMapInfo.movement.x = velocity_.x * deltaTime;
	collisionMapInfo.movement.y = velocity_.y * deltaTime;
	collisionMapInfo.movement.z = velocity_.z * deltaTime;

	// 郢晄ｧｭ繝｣郢晏干繝｡郢昴・繝ｻ邵ｺ・ｨ邵ｺ・ｮ陟冶侭笳・ｹｧ髮∵・陞ｳ繝ｻ
	MapCollisionDetection(collisionMapInfo);

	ApplyCollision(collisionMapInfo);
	ProcessCeilingHit(collisionMapInfo);
	groundStateSwiching(collisionMapInfo);
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

// 隰蜀怜愛
void Player::Draw() { model_->Draw(worldTransform_, *camera_); }
// 	delete debugCamera_;
// 	delete player_;
// }
const WorldTransform& Player::GetWorldTransform() const { return worldTransform_; }

// 郢晄ｧｭ繝｣郢晏干繝｡郢昴・繝ｻ邵ｺ・ｨ邵ｺ・ｮ陟冶侭笳・ｹｧ髮∵・陞ｳ繝ｻ
void Player::MapCollisionDetection(CollisionMapInfo& info) {
	MapCollisionUp(info);
	MapCollisionDown(info);
	MapCollisionRight(info);
	MapCollisionLeft(info);
}

// 郢晏干ﾎ樒ｹｧ・､郢晢ｽ､郢晢ｽｼ邵ｺ・ｮ陟冶侭笳・ｹｧ髮∵・陞ｳ螢ｹ繝ｻ髫怜・繝ｻ陟趣ｽｧ隶灘生・定ｱ弱ｅ・∫ｹｧ繝ｻ
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	// 髫怜・繝ｻ郢ｧ・ｪ郢晁ｼ斐◎郢昴・繝ｨ郢昴・繝ｻ郢晄じﾎ・
	Vector3 offsetTable[kNumCorners] = {
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
	};

	// 髫怜・繝ｻ陟趣ｽｧ隶灘生・定ｱ弱ｅ・∫ｹｧ繝ｻ
	Vector3 result;
	result.x = center.x + offsetTable[static_cast<int>(corner)].x;
	result.y = center.y + offsetTable[static_cast<int>(corner)].y;
	result.z = center.z + offsetTable[static_cast<int>(corner)].z;

	// 驍ｨ蜈域｣｡郢ｧ螳夲ｽｿ譁絶・
	return result;
}

// 髯ｦ譎会ｽｪ竏壹・驍ｨ蜈域｣｡郢ｧ蛛ｵ繝ｻ郢晢ｽｬ郢ｧ・､郢晢ｽ､郢晢ｽｼ邵ｺ・ｫ陷ｿ閧ｴ荳千ｸｺ蜷ｶ・・
void Player::ApplyCollision(const CollisionMapInfo& info) {
	// 驕假ｽｻ陷肴坩纃ｼ郢ｧ蛛ｵ繝ｻ郢晢ｽｬ郢ｧ・､郢晢ｽ､郢晢ｽｼ邵ｺ・ｮ陟趣ｽｧ隶灘生竊楢愾閧ｴ荳・
	worldTransform_.translation_.x += info.movement.x;
	worldTransform_.translation_.y += info.movement.y;
	worldTransform_.translation_.z += info.movement.z;
}

// 陞滂ｽｩ闔霈披・陟冶侭笳・ｸｺ・｣邵ｺ貅倪・邵ｺ髦ｪ繝ｻ陷・ｽｦ騾・・
void Player::ProcessCeilingHit(const CollisionMapInfo& info) {
	// 陞滂ｽｩ闔霈披・陟冶侭笳・ｸｺ・｣邵ｺ・ｦ邵ｺ繝ｻ竊醍ｸｺ莉｣・檎ｸｺ・ｰ郢ｧ・ｹ郢ｧ・ｭ郢昴・繝ｻ
	if (info.ceiling) {
		// 陞滂ｽｩ闔霈披・陟冶侭笳・ｸｺ・｣邵ｺ貅倪・邵ｺ髦ｪ繝ｻ陷・ｽｦ騾・・
		velocity_.y = 0.0f;
	}
}

void Player::MapCollisionUp(CollisionMapInfo& info) {

	// 郢晏干ﾎ樒ｹｧ・､郢晢ｽ､郢晢ｽｼ邵ｺ・ｮ陟冶侭笳・ｹｧ髮∵・陞ｳ螢ｹ繝ｻ髫怜・繝ｻ陟趣ｽｧ隶灘生・定ｱ弱ｅ・∫ｹｧ繝ｻ
	std::array<Vector3, 4> positionsNew;

	// 驕假ｽｻ陷榊供・ｾ蠕後・郢晏干ﾎ樒ｹｧ・､郢晢ｽ､郢晢ｽｼ邵ｺ・ｮ陟冶侭笳・ｹｧ髮∵・陞ｳ螢ｹ繝ｻ髫怜・繝ｻ陟趣ｽｧ隶灘生・定ｱ弱ｅ・∫ｹｧ繝ｻ
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	// 闕ｳ鬆大ｩｿ陷ｷ蜿ｰ・ｻ・･陞滓じ繝ｻ郢ｧ・ｹ郢ｧ・ｭ郢昴・繝ｻ
	if (info.movement.y <= 0) {
		return;
	}

	// 髯ｦ譎会ｽｪ竏夲ｼ邵ｺ・ｦ邵ｺ繝ｻ・狗ｸｺ繝ｻ
	bool hit = false;

	// 郢晄ｧｭ繝｣郢晏干繝｡郢昴・繝ｻ邵ｺ・ｮ郢ｧ・､郢晢ｽｳ郢昴・繝｣郢ｧ・ｯ郢ｧ・ｹ邵ｺ・ｨ驕橸ｽｮ鬯俶ｧｭ・定愾髢・ｾ繝ｻ
	MapChipField::IndexSet indexSet;
	MapChipType mapChipType;

	// 陝ｾ・ｦ闕ｳ繝ｻ
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);

	// 郢晄ｧｭ繝｣郢晏干繝｡郢昴・繝ｻ邵ｺ・ｮ驕橸ｽｮ鬯俶ｧｭ・定愾髢・ｾ繝ｻ
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	// 郢晄じﾎ溽ｹ昴・縺醍ｸｺ・ｪ郢ｧ闃ｽ・｡譎会ｽｪ繝ｻ
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 陷ｿ・ｳ闕ｳ繝ｻ
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopRight]);

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	// 郢晄じﾎ溽ｹ昴・縺醍ｸｺ・ｪ郢ｧ闃ｽ・｡譎会ｽｪ繝ｻ
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 髯ｦ譎会ｽｪ竏ｵ蜃ｾ
	if (hit) {
		// 髯ｦ譎会ｽｪ竏夲ｼ邵ｺ・ｦ邵ｺ繝ｻ・狗ｹ晄ｧｭ繝｣郢晏干繝｡郢昴・繝ｻ邵ｺ・ｮ郢ｧ・､郢晢ｽｳ郢昴・繝｣郢ｧ・ｯ郢ｧ・ｹ郢ｧ雋槫徐陟輔・
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);

		// 髯ｦ譎会ｽｪ竏夲ｼ邵ｺ・ｦ邵ｺ繝ｻ・狗ｹ晄ｧｭ繝｣郢晏干繝｡郢昴・繝ｻ邵ｺ・ｮ驕擾ｽｩ陟厄ｽ｢郢ｧ雋槫徐陟輔・
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		// 郢晏干ﾎ樒ｹｧ・､郢晢ｽ､郢晢ｽｼ邵ｺ・ｮY陟趣ｽｧ隶灘生・定愾髢・ｾ繝ｻ
		float playerY = worldTransform_.translation_.y;

		// 郢晏干ﾎ樒ｹｧ・､郢晢ｽ､郢晢ｽｼ邵ｺ・ｮ陟冶侭笳・ｹｧ髮∵・陞ｳ螢ｹ繝ｻ闕ｳ鬘假ｽｾ・ｺ邵ｺ・ｨ郢晄ｧｭ繝｣郢晏干繝｡郢昴・繝ｻ邵ｺ・ｮ闕ｳ邇厄ｽｾ・ｺ邵ｺ・ｮ髴肴辨螻ｬ郢ｧ蜻茨ｽｱ繧・ｽ∫ｹｧ繝ｻ
		info.movement.y = rect.bottom - (playerY + kHeight / 2.0f) - kBlank;

		// 髯ｦ譎会ｽｪ竏夲ｼ邵ｺ・ｦ邵ｺ繝ｻ・狗ｸｺ・ｨ邵ｺ髦ｪ繝ｻ陞滂ｽｩ闔霈披・陟冶侭笳・ｸｺ・｣邵ｺ・ｦ邵ｺ繝ｻ・狗ｸｺ・ｨ邵ｺ・ｿ邵ｺ・ｪ邵ｺ繝ｻ
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

	Vector3 landingProbeOffset = {0.0f, -kGroundProbeDepth, 0.0f};
	MapChipField::IndexSet indexSetLeft = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kBottomLeft], landingProbeOffset));
	MapChipField::IndexSet indexSetRight = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kBottomRight], landingProbeOffset));

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

	info.movement.y = yMoveToLand;
	info.Landing = true;
}

void Player::MapCollisionRight(CollisionMapInfo& info) {
	// 右方向以外はスキップ
	if (info.movement.x <= 0) {
		return;
	}

	// プレイヤーの当たり判定の角の座標を求める
	std::array<Vector3, 4> positionsNew;
	// 移動後のプレイヤーの当たり判定の角の座標を求める
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	// 右上・右下のマップチップを取得
	MapChipField::IndexSet topIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopRight]);
	MapChipField::IndexSet bottomIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kBottomRight]);

	// ブロックに当たっているか判定
	bool hitTop = mapChipField_->GetMapChipTypeByIndex(topIndex.xIndex, topIndex.yIndex) == MapChipType::kBlock;
	bool hitBottom = mapChipField_->GetMapChipTypeByIndex(bottomIndex.xIndex, bottomIndex.yIndex) == MapChipType::kBlock;
	if (!hitTop && !hitBottom) {
		return;
	}

	// 右壁までの移動量を計算
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

	// X移動量を壁手前に制限
	info.movement.x = xMoveToWall;
	info.wall = true;
	// 壁衝突時は横速度を止める
	velocity_.x = 0.0f;
}

void Player::MapCollisionLeft(CollisionMapInfo& info) {
	// 左方向以外はスキップ
	if (info.movement.x >= 0) {
		return;
	}

	// プレイヤーの当たり判定の角の座標を求める
	std::array<Vector3, 4> positionsNew;
	// 移動後のプレイヤーの当たり判定の角の座標を求める
	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.movement), static_cast<Corner>(i));
	}

	// 左上・左下のマップチップを取得
	MapChipField::IndexSet topIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kTopLeft]);
	MapChipField::IndexSet bottomIndex = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kBottomLeft]);

	// ブロックに当たっているか判定
	bool hitTop = mapChipField_->GetMapChipTypeByIndex(topIndex.xIndex, topIndex.yIndex) == MapChipType::kBlock;
	bool hitBottom = mapChipField_->GetMapChipTypeByIndex(bottomIndex.xIndex, bottomIndex.yIndex) == MapChipType::kBlock;
	if (!hitTop && !hitBottom) {
		return;
	}

	// 左壁までの移動量を計算
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

	// X移動量を壁手前に制限
	info.movement.x = xMoveToWall;
	info.wall = true;
	// 壁衝突時は横速度を止める
	velocity_.x = 0.0f;
}
void Player::groundStateSwiching(const CollisionMapInfo& info) {

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
			return;
		}

		Vector3 footOffset = {0.0f, -kGroundProbeDepth, 0.0f};
		Vector3 leftBottom = Add(CornerPosition(worldTransform_.translation_, kBottomLeft), footOffset);
		Vector3 rightBottom = Add(CornerPosition(worldTransform_.translation_, kBottomRight), footOffset);

		MapChipField::IndexSet leftIndex = mapChipField_->GetMapChipIndexSetByPosition(leftBottom);
		MapChipField::IndexSet rightIndex = mapChipField_->GetMapChipIndexSetByPosition(rightBottom);
		bool hitLeft = mapChipField_->GetMapChipTypeByIndex(leftIndex.xIndex, leftIndex.yIndex) == MapChipType::kBlock;
		bool hitRight = mapChipField_->GetMapChipTypeByIndex(rightIndex.xIndex, rightIndex.yIndex) == MapChipType::kBlock;

		if (!hitLeft && !hitRight) {
			onGround_ = false;
		} else {
			velocity_.y = 0.0f;
		}
	} else {
		if (info.Landing) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}
