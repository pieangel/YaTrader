#pragma once
#include <map>
#include <string>
#include <memory>
#include <any>
#include "SmTaskConst.h"
#include "../Json/json.hpp"
namespace DarkHorse {
	typedef std::map<std::string, std::any> task_arg;
	struct SmTaskInfo {
		SmTaskType TaskType;
		std::string TaskTitle;
		std::string TaskDetail;
		size_t TotalTaskCount;
		size_t RemainTaskCount;
		// 타이머 시간 간격 : 밀리초 단위임
		int TimerInterval;
		bool Next;
		std::map<int, task_arg> argMap;
	};
}
