#pragma once

#include <math/Vector3.h>

#include <string>
#include <vector>

// マップチップ種別
// CSV の値: 0=空白, 1=ブロック
enum class MapChipType {
	kBlank,
	kBlock,
};

// 2 次元マップチップデータ
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:
	// 1 ブロックのワールドサイズ
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;

	// マップの固定サイズ
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapchipDate_;

	// マップ配列を初期化する
	void ResetMapChipData();

	// ブロックの境界矩形
	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	// インデックスからブロック矩形を取得
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	// インデックスからチップ種別を取得（範囲外は空白）
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	// CSV からマップデータを読み込む
	void LoadMapChipCsv(const std::string& filePath);

	// 2 次元インデックス
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	// ワールド座標からインデックスを取得
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	// インデックスからブロック中心座標を取得
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
};
