#include "stdafx.h"
#include "SmChartFrameManager.h"
#include "SmChartFrame.h"
using namespace DarkHorse;

int SmChartFrameManager::_Id = 0;

DarkHorse::SmChartFrameManager::~SmChartFrameManager()
{

}

void DarkHorse::SmChartFrameManager::AddChartFrame(std::shared_ptr<SmChartFrame> chart_frame)
{
	if (!chart_frame) return;

	_ChartMap[chart_frame->Id()] = chart_frame;
}

std::shared_ptr<SmChartFrame> DarkHorse::SmChartFrameManager::FindChartFrame(const int& chart_id)
{
	auto found = _ChartMap.find(chart_id);
	if (found == _ChartMap.end()) return nullptr;

	return found->second;
}
