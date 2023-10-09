#include "stdafx.h"
#include "SmSystem.h"
using namespace DarkHorse;

DarkHorse::SmSystem::SmSystem(const int& id, const std::string& system_name)
	: _Id(id), _Name(system_name)
{
}

void DarkHorse::SmSystem::BindSystemData()
{
	_Data = std::make_shared<SmSystemData>();
	_Data->Sysmtem(shared_from_this());
}

void SmSystem::ProcessData(std::shared_ptr<DarkHorse::SmChartData> chart_data)
{
	_Data->SetChartData(chart_data);
	_Data->ProcessData();
	_DataProcessed = true;
}
