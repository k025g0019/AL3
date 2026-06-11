//====================
// マップチップ 実装
//====================
#include "MapChipField.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

namespace {
	
	
	std::map<char, MapChipType> mapChipTypeTable = {
		{'B', MapChipType::kBlock},
		{'P', MapChipType::kPlayer},
		{'E', MapChipType::kEnemy},
		{'S', MapChipType::kShieldEnemy},
	};
}


//====================
// マップ配列初期化
//====================
/*
マップ配列を規定サイズで作り直す
*/
void MapChipField::ResetMapChipData() {
	// 既存データをクリア
	mapchipDate_.data.clear();

	// 縦行を確保
	mapchipDate_.data.resize(kNumBlockVirtical);

	// 各行の横要素を確保
	for (std::vector<MapChipDataUnit>& mapChipsDataLine : mapchipDate_.data) {
		mapChipsDataLine.resize(kNumBlockHorizontal);
	}
}

//====================
// CSV読み込み
//====================
/*
CSVファイルからマップチップ配列を作成する
*/
void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	// 配列を初期化
	ResetMapChipData();

	// CSVファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	// 全文を文字列ストリームへ読み込む
	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	// 行・列ごとに分解してチップ種別を設定
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		std::string line;
		getline(mapChipCsv, line);

		std::istringstream lineStream(line);
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			std::getline(lineStream, word, ',');

			if (word.empty()) {
				continue;
				
			}

			if (!mapChipTypeTable.contains(word[kChipType])) {
				continue;
			}

			mapchipDate_.data[i][j].type = mapChipTypeTable[word[kChipType]];

			if (word.size() <= kChipSubID) {
				continue;;

			}

			mapchipDate_.data[i][j].subID = static_cast<uint8_t>(word[kChipSubID] - '0');

		}
	}
}

//====================
// 種別取得
//====================
/*
インデックスからマップチップ種別を取得する
*/
MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	// 範囲外は空白として扱う
	if (kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}

	// 範囲内は配列値を返す
	return mapchipDate_.data[yIndex][xIndex].type;
}

//====================
// サブID取得
//====================
/*
インデックスからマップチップサブIDを取得する
*/
uint8_t MapChipField::GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex) {
	// 範囲外は0を返す
	if (kNumBlockHorizontal - 1 < xIndex) {
		return 0;
	}
	if (kNumBlockVirtical - 1 < yIndex) {
		return 0;
	}

	// 範囲内は配列値を返す
	return mapchipDate_.data[yIndex][xIndex].subID;
}

//====================
// 座標取得
//====================
/*
インデックスからブロック中心座標を返す
*/
KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return KamataEngine::Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0.0f);
}

//====================
// 矩形取得
//====================
/*
インデックスからブロック矩形を返す
*/
MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	// ブロック中心座標を取得
	KamataEngine::Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	// 中心座標から左右上下を計算
	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;
	return rect;
}

//====================
// インデックス取得
//====================
/*
ワールド座標からマップインデックスへ変換する
*/
MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position) {
	IndexSet indexSet;

	// 座標をセル座標に変換
	float xCell = std::floor((position.x + kBlockWidth * 0.5f) / kBlockWidth);
	float yCell = std::floor((position.y + kBlockHeight * 0.5f) / kBlockHeight);

	// セル座標を配列インデックスへ変換
	indexSet.xIndex = static_cast<uint32_t>(xCell);
	indexSet.yIndex = static_cast<uint32_t>((kNumBlockVirtical - 1) - yCell);
	return indexSet;
}
