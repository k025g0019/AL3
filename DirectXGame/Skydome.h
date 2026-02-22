#include "KamataEngine.h"

class Skydome {
	public:
	void Initialize();
	
	void Update();

	void Draw(const KamataEngine::Camera& camera);
	
private:
	KamataEngine::WorldTransform world_transform_;

	KamataEngine::Model* model_ = nullptr;

	
};
