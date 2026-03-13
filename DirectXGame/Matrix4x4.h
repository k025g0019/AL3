#pragma once

#include <math/Matrix4x4.h>

namespace KamataEngine {
	struct Vector3;
}

//====================
// 行列ユーティリティ
//====================
/*
S/R/T 行列の作成と行列積を行う補助関数群
*/

//====================
// スケール行列
//====================
/*
拡大縮小行列を作成する
*/
static KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& s) {
	KamataEngine::Matrix4x4 m{};
	m.m[0][0] = s.x;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;
	m.m[1][0] = 0.0f;
	m.m[1][1] = s.y;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;
	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = s.z;
	m.m[2][3] = 0.0f;
	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
	return m;
}

//====================
// X回転行列
//====================
/*
X軸回転行列を作成する
*/
static KamataEngine::Matrix4x4 MakeRotateX(float x) {
	const float c = std::cos(x);
	const float s = std::sin(x);
	KamataEngine::Matrix4x4 m{};
	m.m[0][0] = 1.0f;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;
	m.m[1][0] = 0.0f;
	m.m[1][1] = c;
	m.m[1][2] = s;
	m.m[1][3] = 0.0f;
	m.m[2][0] = 0.0f;
	m.m[2][1] = -s;
	m.m[2][2] = c;
	m.m[2][3] = 0.0f;
	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
	return m;
}

//====================
// Y回転行列
//====================
/*
Y軸回転行列を作成する
*/
static KamataEngine::Matrix4x4 MakeRotateY(float y) {
	const float c = std::cos(y);
	const float s = std::sin(y);
	KamataEngine::Matrix4x4 m{};
	m.m[0][0] = c;
	m.m[0][1] = 0.0f;
	m.m[0][2] = -s;
	m.m[0][3] = 0.0f;
	m.m[1][0] = 0.0f;
	m.m[1][1] = 1.0f;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;
	m.m[2][0] = s;
	m.m[2][1] = 0.0f;
	m.m[2][2] = c;
	m.m[2][3] = 0.0f;
	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
	return m;
}

//====================
// Z回転行列
//====================
/*
Z軸回転行列を作成する
*/
static KamataEngine::Matrix4x4 MakeRotateZ(float z) {
	const float c = std::cos(z);
	const float s = std::sin(z);
	KamataEngine::Matrix4x4 m{};
	m.m[0][0] = c;
	m.m[0][1] = s;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;
	m.m[1][0] = -s;
	m.m[1][1] = c;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;
	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = 1.0f;
	m.m[2][3] = 0.0f;
	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
	return m;
}

//====================
// 行列積
//====================
/*
4x4行列同士の積を計算する
*/
static KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& a, const KamataEngine::Matrix4x4& b) {
	KamataEngine::Matrix4x4 r{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			r.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
		}
	}
	return r;
}

//====================
// 平行移動行列
//====================
/*
平行移動行列を作成する
*/
static KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& t) {
	KamataEngine::Matrix4x4 m{};
	m.m[0][0] = 1.0f;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;
	m.m[1][0] = 0.0f;
	m.m[1][1] = 1.0f;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;
	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = 1.0f;
	m.m[2][3] = 0.0f;
	m.m[3][0] = t.x;
	m.m[3][1] = t.y;
	m.m[3][2] = t.z;
	m.m[3][3] = 1.0f;
	return m;
}

//====================
// アフィン行列
//====================
/*
SRTからアフィン行列を作成する
*/
static KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& s, const KamataEngine::Vector3& r, const KamataEngine::Vector3& t) {
	KamataEngine::Matrix4x4 sMat = MakeScaleMatrix(s);
	KamataEngine::Matrix4x4 rxMat = MakeRotateX(r.x);
	KamataEngine::Matrix4x4 ryMat = MakeRotateY(r.y);
	KamataEngine::Matrix4x4 rzMat = MakeRotateZ(r.z);
	KamataEngine::Matrix4x4 tMat = MakeTranslateMatrix(t);

	// 回転順は Z -> Y -> X
	KamataEngine::Matrix4x4 rMat = Multiply(Multiply(rzMat, ryMat), rxMat);

	// S * R * T の順で合成
	return Multiply(Multiply(sMat, rMat), tMat);
}
