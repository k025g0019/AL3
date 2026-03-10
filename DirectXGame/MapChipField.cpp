#include "MapChipField.h"

#include <cassert>
#include <fstream>
#include <map>
#include <cmath>
#include <sstream>
#include <string>

#include "GameScene.h"

namespace {
// CSV値とマップ種別の対応表
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

// マップ配列を既定サイズで初期化
void MapChipField::ResetMapChipData() {
	mapchipDate_.data.clear();
	mapchipDate_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipsDataLine : mapchipDate_.data) {
		mapChipsDataLine.resize(kNumBlockHorizontal);
	}
}

// CSVを読み込み、2次元配列へ格納
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

// インデックス範囲外は空白扱い
MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}
	return mapchipDate_.data[yIndex][xIndex];
}

// インデックスからマス中心のワールド座標を取得
KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return KamataEngine::Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0);
}

// インデックスからマス矩形を取得
MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	KamataEngine::Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;
	return rect;
}

// ワールド座標からマップインデックスへ変換
MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position) {
	IndexSet indexSet;
	float xCell = std::floor((position.x + kBlockWidth * 0.5f) / kBlockWidth);
	float yCell = std::floor((position.y + kBlockHeight * 0.5f) / kBlockHeight);
	indexSet.xIndex = static_cast<uint32_t>(xCell);
	indexSet.yIndex = static_cast<uint32_t>((kNumBlockVirtical - 1) - yCell);
	return indexSet;
}