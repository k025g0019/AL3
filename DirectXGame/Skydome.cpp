#include "Skydome.h"

using namespace KamataEngine;

void Skydome::Initialize() {

	//// モデル読み込み
	model_ = Model::CreateFromOBJ("skydome");

	
	world_transform_.Initialize();


	world_transform_.translation_ = {0.0f, 0.0f, 0.0f};


	world_transform_.scale_ = {100.0f, 100.0f, 100.0f};
}

void Skydome::Update() {

}

void Skydome::Draw(const Camera& camera) {

	if (!model_) {
		return;
	}

	// ワールド行列をGPUへ転送
	world_transform_.TransferMatrix();

	// 描画
	model_->Draw(world_transform_, camera);
}