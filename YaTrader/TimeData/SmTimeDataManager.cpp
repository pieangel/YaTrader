#include "stdafx.h"
#include "SmTimeDataManager.h"
#include "SmTimeData.h"
using namespace DarkHorse;

int SmTimeDataManager::_Id = 0;

std::shared_ptr<DarkHorse::SmTimeData> DarkHorse::SmTimeDataManager::FindAddTimeData(const std::string name)
{
	auto found = _DataMap.find(name);
	if (found != _DataMap.end())
		return found->second;
	else {
		std::shared_ptr<DarkHorse::SmTimeData> time_data = CreateTimeData(name);
		_DataMap[name] = time_data;
		return time_data;
	}
}

std::shared_ptr<DarkHorse::SmTimeData> DarkHorse::SmTimeDataManager::CreateTimeData(const std::string& name)
{
	return std::make_shared<DarkHorse::SmTimeData>(name, GetId());
}
