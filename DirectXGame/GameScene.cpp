#include "GameScene.h"

using namespace KamataEngine;
static Matrix4x4 MakeScaleMatrix(const Vector3& s) {
	Matrix4x4 m{};
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

static Matrix4x4 MakeRotateX(float x) {
	const float c = std::cos(x);
	const float s = std::sin(x);
	Matrix4x4 m{};
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

static Matrix4x4 MakeRotateY(float y) {
	const float c = std::cos(y);
	const float s = std::sin(y);
	Matrix4x4 m{};
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

static Matrix4x4 MakeRotateZ(float z) {
	const float c = std::cos(z);
	const float s = std::sin(z);
	Matrix4x4 m{};
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

static Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 r{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			r.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
		}
	}
	return r;
}

static Matrix4x4 MakeTranslateMatrix(const Vector3& t) {
	Matrix4x4 m{};
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

// scale, rotation(xyz), translation からワールド行列を作る
static Matrix4x4 MakeAffineMatrix(const Vector3& s, const Vector3& r, const Vector3& t) {
	Matrix4x4 S = MakeScaleMatrix(s);
	Matrix4x4 Rx = MakeRotateX(r.x);
	Matrix4x4 Ry = MakeRotateY(r.y);
	Matrix4x4 Rz = MakeRotateZ(r.z);
	Matrix4x4 T = MakeTranslateMatrix(t);

	// 回転順は教材に合わせる（ここは Z→Y→X の例）
	Matrix4x4 R = Multiply(Multiply(Rz, Ry), Rx);

	return Multiply(Multiply(S, R), T);
}

void GameScene::GenerateBlocks() {
	uint32_t kNumBlockVertical = 20;
	uint32_t kNumBlockHorizontal = 100;

	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		for (uint32_t j = i % 2; j < kNumBlockHorizontal; j += 2) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				worldTransformBlocks_[i][j] = new WorldTransform();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->scale_ = {1.0f, 1.0f, 1.0f};
				worldTransformBlocks_[i][j]->rotation_ = {0.0f, 0.0f, 0.0f};

				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j,i);
			}
		}
	}

}
	void GameScene::Initialize() {
		textureHandle_ = TextureManager::Load("mario.jpg");

		// �X�v���C�g�C���X�^���X�̐���
		model_ = Model::Create();
	    modelBlock_ = Model::Create();
		worldTransform_.Initialize();
		camera_.Initialize();

		PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

		debugCamera_ = new DebugCamera(1280, 720);
		mapChipField_ = new MapChipField;

		mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
		// �������\���̕\����L���ɂ���
		AxisIndicator::GetInstance()->SetVisible(true);

		// �������\�����Q�Ƃ���r���[�v���W�F�N�V������w�肷��
		AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
		GenerateBlocks();
		player_ = new Player();
		player_->Initialize(model_, textureHandle_, &camera_);
	}

	void GameScene::Update() {

		ImGui::Begin("Debug1");
		ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
		ImGui::End();

		debugCamera_->Update();
		//// �X�v���C�g�̍��W��擾
		// Vector2 position = sprite_->GetPosition();
	    for (const std::vector<KamataEngine::WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		    for (KamataEngine::WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			    if (!worldTransformBlock) {
				    continue;
			    }
			    worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			    worldTransformBlock->TransferMatrix();
		    }
	    }
		//// ���W��{2.0f, 1.0f}�ړ�
		// position.x += 2.0f;
		// position.y += 1.0f;

		//// �ړ��������W��X�v���C�g�ɔ��f
		// sprite_->SetPosition(position);

		player_->Update();
	}

	void GameScene::Draw() {

		// PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});

		Model::PreDraw();
		player_->Draw();
	    for (const std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		    for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			    if (!worldTransformBlock) {
				    continue;
			    }
			    modelBlock_->Draw(*worldTransformBlock, camera_);
		    }
	    }
		Model::PostDraw();
	}

	GameScene::GameScene() { Initialize(); }

	GameScene::~GameScene() {
		delete debugCamera_;
		delete model_;
		sprite_ = nullptr;
		delete player_;
		delete mapChipField_;
	}