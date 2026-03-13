#pragma once

#include <math/Vector3.h>

//====================
// AABB
//====================
/*
軸に平行な当たり判定ボックス
*/
struct AABB {
	// 最小座標（左下奥）
	KamataEngine::Vector3 min;

	// 最大座標（右上手前）
	KamataEngine::Vector3 max;
};

//====================
// 判定関数
//====================
/*
AABB 同士が交差しているかを判定する
*/
inline bool IsAABBCollision(const AABB& a, const AABB& b) {
	// X軸の重なりを判定
	const bool overlapX = (a.min.x <= b.max.x) && (a.max.x >= b.min.x);

	// Y軸の重なりを判定
	const bool overlapY = (a.min.y <= b.max.y) && (a.max.y >= b.min.y);

	// Z軸の重なりを判定
	const bool overlapZ = (a.min.z <= b.max.z) && (a.max.z >= b.min.z);

	// 全軸で重なっている時だけ交差
	return overlapX && overlapY && overlapZ;
}
