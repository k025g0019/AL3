#include "StageManager.h"

#include <cassert>

void StageManager::AddStage(uint32_t stageNo, const std::string& csvFilePath) {
	StageData stage;
	stage.stageNo = stageNo;
	stage.csvFilePath = csvFilePath;
	stageData_.push_back(stage);
}

const std::string& StageManager::GetStageCsvPath(uint32_t stageNo) const {
	assert(stageNo > 0 && stageNo <= stageData_.size());
	return stageData_[stageNo - 1].csvFilePath;
}
