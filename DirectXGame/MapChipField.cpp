#include "MapChipField.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

namespace {
// CSV 値からチップ種別へ変換するテーブル
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

void MapChipField::ResetMapChipData() {
	mapchipDate_.data.clear();
	mapchipDate_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipsDataLine : mapchipDate_.data) {
		mapChipsDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	ResetMapChipData();

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		std::string line;
		getline(mapChipCsv, line);

		std::istringstream lineStream(line);
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			std::getline(lineStream, word, ',');

			if (mapChipTable.contains(word)) {
				mapchipDate_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}
	return mapchipDate_.data[yIndex][xIndex];
}

KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return KamataEngine::Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0);
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	KamataEngine::Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;
	return rect;
}

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position) {
	IndexSet indexSet;
	float xCell = std::floor((position.x + kBlockWidth * 0.5f) / kBlockWidth);
	float yCell = std::floor((position.y + kBlockHeight * 0.5f) / kBlockHeight);
	indexSet.xIndex = static_cast<uint32_t>(xCell);
	indexSet.yIndex = static_cast<uint32_t>((kNumBlockVirtical - 1) - yCell);
	return indexSet;
}
