#pragma once
#include <math/Vector3.h>
#include <string>
#include <vector>

enum class MapChipType {
	kBlank,
	kBlock,
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};
class MapChipField {
public:
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapchipDate_;

	void ResetMapChipData();
	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	void LoadMapChipCsv(const std::string& filePath);

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
};