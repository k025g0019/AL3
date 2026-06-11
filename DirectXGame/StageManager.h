#pragma once

#include <string>
#include <vector>

struct StageData {
	uint32_t stageNo;
	std::string csvFilePath;
};

class StageManager {
public:
	void AddStage(uint32_t stageNo, const std::string& csvFilePath);
	const std::string& GetStageCsvPath(uint32_t stageNo) const;
	uint32_t GetNumStages() const { return static_cast<uint32_t>(stageData_.size()); }

private:
	std::vector<StageData> stageData_;
};
