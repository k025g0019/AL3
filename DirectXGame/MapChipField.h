#pragma once

#include <math/Vector3.h>

#include <string>
#include <vector>

//====================
// マップチップ種別
//====================
/*
CSV の値とマップチップ種別の対応
0=空白, 1=ブロック
*/
enum class MapChipType {
	kBlank,
	kBlock,
	kPlayer,
	kEnemy,
	kShieldEnemy,
};

struct MapChipDataUnit {
	MapChipType type;
	uint8_t subID;
};
//====================
// マップデータ
//====================
/*
2次元のマップチップ配列
*/
struct MapChipData {
	std::vector<std::vector<MapChipDataUnit>> data;
};

//====================
// マップチップフィールド
//====================
/*
CSV読み込みと座標変換を担当するクラス
*/
class MapChipField {
public:
	//====================
	// 定数
	//====================
	static inline const float kBlockWidth = 2.0f;           // 1ブロックの横幅
	static inline const float kBlockHeight = 2.0f;          // 1ブロックの縦幅
	static inline const uint32_t kNumBlockVirtical = 20;    // マップの縦ブロック数
	static inline const uint32_t kNumBlockHorizontal = 100; // マップの横ブロック数

	//====================
	// 変数
	//====================
	MapChipData mapchipDate_;

	//====================
	// 補助型
	//====================
	/* 矩形情報 */
	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	/* インデックス情報 */
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	enum MapChipCharIndex {
		kChipType = 0,
		kChipSubID = 1,
	};
	uint8_t GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex);
	//====================
	// 関数
	//====================
	void ResetMapChipData();
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	void LoadMapChipCsv(const std::string& filePath);
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
};
