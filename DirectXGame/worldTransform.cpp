#include "worldTransform.h"

#include <algorithm>
#include <math/MathUtility.h>

void worldTransformMatrix(KamataEngine::WorldTransform& worldTransform) {
	KamataEngine::Matrix4x4 matWorld =
		KamataEngine::MathUtility::MakeScaleMatrix(worldTransform.scale_);

	KamataEngine::MathUtility::operator*=(
		matWorld, KamataEngine::MathUtility::MakeRotateXMatrix(worldTransform.rotation_.x));
	KamataEngine::MathUtility::operator*=(
		matWorld, KamataEngine::MathUtility::MakeRotateYMatrix(worldTransform.rotation_.y));
	KamataEngine::MathUtility::operator*=(
		matWorld, KamataEngine::MathUtility::MakeRotateZMatrix(worldTransform.rotation_.z));
	KamataEngine::MathUtility::operator*=(
		matWorld, KamataEngine::MathUtility::MakeTranslateMatrix(worldTransform.translation_));

	worldTransform.matWorld_ = matWorld;
	worldTransform.TransferMatrix();
}
