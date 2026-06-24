#include "Skydome.h"

using namespace KamataEngine;

void Skydome::Initialize() {
	// Load skydome model with inverted normals enabled.
	model_ = Model::CreateFromOBJ("skydome", true);

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

	world_transform_.TransferMatrix();
	model_->Draw(world_transform_, camera);
}
