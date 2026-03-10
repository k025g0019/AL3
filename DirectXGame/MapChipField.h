#pragma once
#include <math/Vector3.h>
#include <string>
#include <vector>

// マップ1マスの種類
enum class MapChipType {
	kBlank,
	kBlock,
};

// CSVから読み込んだマップデータ
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:
	// 1マスのワールドサイズ
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;

	// マップサイズ（縦×横）
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapchipDate_;

	// マップデータ配列を初期化
	void ResetMapChipData();

	// ブロック矩形
	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	// インデックスから矩形を取得
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	// インデックスからマップチップ種別を取得
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	// CSVを読み込んでマップを構築
	void LoadMapChipCsv(const std::string& filePath);

	// 2次元インデックス
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	// ワールド座標からインデックスを取得
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	// インデックスからマス中心座標を取得
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
};