#include "stdafx.h"
#include "SmChartDataManager.h"
#include "SmChartData.h"
#include "../Task/SmTaskRequestMaker.h"
#include "../Global/SmTotalManager.h"
#include "../Task/SmServerDataReceiver.h"
#include "../Util/VtStringUtil.h"
#include "../Util/SmUtil.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Task/SmTaskArg.h"
#include "../Task/SmTaskRequestManager.h"
#include "../DataFrame/DataFrame.h"  // Main DataFrame header
#include "../Task/SmTaskDefine.h"
#include "../Task/ViServerDataReceiver.h"
#include <chrono>

using namespace std::chrono;

namespace DarkHorse {

	int SmChartDataManager::_Id = 0;

	SmChartDataManager::SmChartDataManager()
	{

	}

	SmChartDataManager::~SmChartDataManager()
	{
		for (auto it = _TimerMap.begin(); it != _TimerMap.end(); it++) {
			_Timer.remove(it->second);
		}
	}

	std::shared_ptr<SmChartData> SmChartDataManager::FindChartData(const std::string& chart_data_key)
	{
		auto it = _ChartDataMap.find(chart_data_key);
		if (it != _ChartDataMap.end()) return it->second;
		else return nullptr;
	}

	std::shared_ptr<SmChartData> SmChartDataManager::FindChartData(const std::string& symbol_code, const SmChartType& chart_type, const int& cycle)
	{
		std::string chart_data_key = MakeDataKey(symbol_code, static_cast<int>(chart_type), cycle);
		return FindChartData(chart_data_key);
	}

	std::shared_ptr<SmChartData> SmChartDataManager::FindAddChartData(const std::string& symbol_code, const SmChartType& chart_type, const int& cycle)
	{
		std::string chart_data_key = MakeDataKey(symbol_code, static_cast<int>(chart_type), cycle);
		auto found = _ChartDataMap.find(chart_data_key);
		if (found != _ChartDataMap.end()) return found->second;
		else {
			std::shared_ptr<DarkHorse::SmChartData> chart_data = std::make_shared<DarkHorse::SmChartData>(symbol_code, chart_type, cycle, GetId());
			CreateTimer(chart_data);
			auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
			if (symbol) {
				symbol->AddChartData(chart_data);
				chart_data->ProductCode(symbol->ProductCode());
			}
			_ChartDataMap[chart_data_key] = chart_data;
			return chart_data;
		}
	}

	void SmChartDataManager::RequestChartData(const std::string& symbol_code, const int& window_id, const int& series_index)
	{
		RequestChartData(symbol_code, SmChartType::MIN, 1, window_id, series_index);
	}


	void SmChartDataManager::MergeChartData(const std::string& symbol_code, const SmChartType& src_chart_type1, const int& src_cycle1, const SmChartType& src_chart_type2, const int& src_cycle2)
	{
		const std::string& data_key1 = MakeDataKey(symbol_code, src_chart_type1, src_cycle1);
		std::shared_ptr<SmChartData> chart_data1 = FindChartData(data_key1);
		const std::string& data_key2 = MakeDataKey(symbol_code, src_chart_type2, src_cycle2);
		std::shared_ptr<SmChartData> chart_data2 = FindChartData(data_key2);

		if (chart_data1 && chart_data1->Received()) return;
		if (chart_data2 && chart_data2->Received()) return;

		hmdf::StdDataFrame<hmdf::DateTime> join_df;


		join_df = chart_data1->GetDataFrame().join_by_index<decltype(join_df), double>(chart_data2->GetDataFrame(), hmdf::join_policy::left_right_join);
	}

	void SmChartDataManager::RequestChartData(const std::string& symbol_code, const SmChartType& chart_type, const int& cycle, const int& count, const int& window_id, const int& series_index)
	{
		const std::string& data_key = MakeDataKey(symbol_code, chart_type, cycle);
		std::shared_ptr<SmChartData> chart_data = FindChartData(data_key);

		if (chart_data && chart_data->Received()) return;

		SmChartDataReq req;
		req.TaskId = SmServerDataReceiver::GetId();
		req.SymbolCode = symbol_code;
		req.ChartType = chart_type;
		req.Cycle = cycle;
		req.Count = count;
		req.WindowId = window_id;
		req.SeriesIndex = series_index;


		DhTaskArg arg;
		arg.detail_task_description = data_key;
		arg.argument_id = ViServerDataReceiver::get_argument_id();
		arg.task_type = DhTaskType::SymbolChartData;
		arg.parameter_map["symbol_code"] = symbol_code;
		arg.parameter_map["chart_type"] = chart_type;
		arg.parameter_map["cycle"] = cycle;
		arg.parameter_map["count"] = count;
		arg.parameter_map["window_id"] = window_id;
		arg.parameter_map["series_index"] = series_index;
		mainApp.TaskReqMgr()->AddTask(std::move(arg));
	}

	void SmChartDataManager::RequestChartData(const std::string& symbol_code, const SmChartType& chart_type, const int& cycle, const int& window_id, const int& series_index)
	{
		const std::string& data_key = MakeDataKey(symbol_code, chart_type, cycle);
		std::shared_ptr<SmChartData> chart_data = FindChartData(data_key);

		if (chart_data && chart_data->Received()) return;

		
		DhTaskArg arg;
		arg.detail_task_description = data_key;
		arg.argument_id = ViServerDataReceiver::get_argument_id();
		arg.task_type = DhTaskType::SymbolChartData;
		arg.parameter_map["symbol_code"] = symbol_code;
		arg.parameter_map["chart_type"] = chart_type;
		arg.parameter_map["cycle"] = cycle;
		arg.parameter_map["count"] = 1500;
		arg.parameter_map["window_id"] = window_id;
		arg.parameter_map["series_index"] = series_index;
		mainApp.TaskReqMgr()->AddTask(std::move(arg));
	}

	void SmChartDataManager::CreateTimer(std::shared_ptr<SmChartData> chart_data)
	{
		if (!chart_data) return;

		auto it = _TimerMap.find(chart_data->GetChartDataKey());
		if (it != _TimerMap.end())
			return;

		// 틱데이터는 처리하지 않는다. 
		if (chart_data->ChartType() == SmChartType::TICK)
			return;

		std::vector<int> date_time = SmUtil::GetLocalDateTime();
		int minMod = date_time[4] % chart_data->Cycle();
		int waitTime = chart_data->Cycle() * 60 - (minMod * 60 + date_time[5]);
		// Add to the timer.
		auto id = _Timer.add(seconds(waitTime), std::bind(&SmChartData::OnTimer, chart_data), seconds(chart_data->Cycle() * 60));
		// Add to the request map.
		_TimerMap[chart_data->GetChartDataKey()] = id;
	}
}
