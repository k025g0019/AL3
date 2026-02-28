#pragma once
#include <string>
#include <vector>
#include <math/Vector3.h>

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

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	void LoadMapChipCsv(const std::string& filePath);

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
};