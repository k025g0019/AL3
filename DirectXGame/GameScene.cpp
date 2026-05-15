#include "GameScene.h"

#include <algorithm>

using namespace KamataEngine;

namespace {
constexpr Vector2 kInitialSpritePosition = {100.0f, 100.0f};
constexpr Vector2 kSpriteSize = {128.0f, 128.0f};
constexpr float kAudioVolume = 0.5f;
}

void GameScene::Initialize() {
	if (isInitialized_) {
		return;
	}

	textureHandle_ = TextureManager::Load("mario.jpg");
	const uint32_t spriteTextureHandle = TextureManager::Load("sample.png");
	soundDataHandle_ = Audio::GetInstance()->LoadWave("fanfare.wav");

	model_ = Model::Create();
	sprite_ = Sprite::Create(spriteTextureHandle, kInitialSpritePosition);
	sprite_->SetSize(kSpriteSize);

	worldTransform_.Initialize();
	camera_.Initialize();

	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	Audio::GetInstance()->PlayWave(soundDataHandle_, false, kAudioVolume);

	isInitialized_ = true;
}

void GameScene::Update() {
#ifdef _DEBUG
	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::Text("Press SPACE to replay audio");
	ImGui::End();
#endif

	debugCamera_->Update();

	Vector2 position = sprite_->GetPosition();
	position.x += spriteVelocity_.x;
	position.y += spriteVelocity_.y;

	if (position.x <= 0.0f || position.x + kSpriteSize.x >= float(WinApp::kWindowWidth)) {
		spriteVelocity_.x *= -1.0f;
		position.x = std::clamp(position.x, 0.0f, float(WinApp::kWindowWidth) - kSpriteSize.x);
	}

	if (position.y <= 0.0f || position.y + kSpriteSize.y >= float(WinApp::kWindowHeight)) {
		spriteVelocity_.y *= -1.0f;
		position.y = std::clamp(position.y, 0.0f, float(WinApp::kWindowHeight) - kSpriteSize.y);
	}

	sprite_->SetPosition(position);

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		Audio::GetInstance()->PlayWave(soundDataHandle_, false, kAudioVolume);
	}
}

void GameScene::Draw() {
	PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});

	Model::PreDraw();
	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);
	Model::PostDraw();

	Sprite::PreDraw();
	sprite_->Draw();
	Sprite::PostDraw();
}

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite_;
	delete debugCamera_;
	delete model_;
}
