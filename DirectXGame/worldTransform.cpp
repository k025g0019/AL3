#include "worldTransform.h"
#include <algorithm>
#include <math/MathUtility.h>
using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void worldTransformMatrix(WorldTransform& worldTransform) {
	worldTransform.matWorld_ = MakeScaleMatrix(worldTransform.scale_) *
		MakeRotateXMatrix(worldTransform.rotation_.x) *
		MakeRotateYMatrix(worldTransform.rotation_.y) *
		MakeRotateZMatrix(worldTransform.rotation_.z) *
		MakeTranslateMatrix(worldTransform.translation_);
	worldTransform.TransferMatrix();
}
