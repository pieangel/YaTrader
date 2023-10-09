#include "stdafx.h"
#include "SmTimeData.h"
#include "../Util/SmUtil.h"
#include "../Chart/SmChartData.h"
#include "../Chart/SmChartDataManager.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
using namespace DarkHorse;
void SmTimeData::AddValue(const int& date, const int& time, const std::string& column_name, const double& value)
{
	if (!_Enable) return;

	auto found_col = _ColumnDataMap.find(column_name);
	if (found_col == _ColumnDataMap.end()) return;

	std::string date_time = std::to_string(date);
	date_time.append(std::to_string(time));
	size_t index = _Index.empty() ? 0 : _Index.size();
	auto found = _Index.find(date_time);
	if (found == _Index.end()) {
		std::vector<int> ymd = SmUtil::IntToDate(date);
		std::vector<int> hms = SmUtil::IntToDate(time);
		COleDateTime ole_date_time;
		ole_date_time.SetDateTime(ymd[0], ymd[1], ymd[2], hms[0], hms[1], hms[2]);
		_Index[date_time] = ole_date_time;
		index = _Index.size() - 1;
	}
	else {
		index = std::distance(_Index.begin(), found) - 1;
	}

	std::vector<double>& column_vec = found_col->second;
	
	if (index >= column_vec.size()) {
		column_vec.resize(index, value);
	}
	else {
		column_vec[index] = value;
	}
}

void SmTimeData::AddColumn(const std::string& column_name)
{
	if (!_Enable) return;

	auto found_col = _ColumnDataMap.find(column_name);
	if (found_col != _ColumnDataMap.end()) return;

	std::vector<double> column_vec;
	column_vec.resize(1500);
	_ColumnDataMap[column_name] = std::move(column_vec);
}

void SmTimeData::UpdateValue(const int& date, const int& time, const std::string& column_name, const double& value)
{
	if (!_Enable) return;

	auto found_col = _ColumnDataMap.find(column_name);
	if (found_col == _ColumnDataMap.end()) return;

	std::string date_time = std::to_string(date);
	date_time.append(std::to_string(time));

	auto found = _Index.find(date_time);
	if (found == _Index.end()) return;
	
	size_t index = std::distance(_Index.begin(), found) - 1;

	std::vector<double>& column_vec = found_col->second;

	if (index >= column_vec.size()) {
		column_vec.resize(index, value);
	}
	else {
		column_vec[index] = value;
	}
}

void DarkHorse::SmTimeData::RemoveSymbol(const std::string& symbol_code)
{
	_Enable = false;

	auto found = _SymbolMap.find(symbol_code);
	if (found == _SymbolMap.end()) { _Enable = true; return; }

	const std::string chart_data_key = SmChartDataManager::MakeDataKey(symbol_code, _ChartType, _Cycle);

	auto found_col = _ColumnDataMap.find(chart_data_key);

	if (found_col == _ColumnDataMap.end()) { _Enable = true; return; }

	_ColumnDataMap.erase(found_col);
}

bool DarkHorse::SmTimeData::RequestChartData()
{
	bool requested = false;
	for (auto it = _SymbolMap.begin(); it != _SymbolMap.end(); it++) {
		auto chart_data = mainApp.ChartDataMgr()->FindAddChartData(it->first, _ChartType, _Cycle);
		if (!chart_data || !chart_data->Received()) {
			mainApp.ChartDataMgr()->RequestChartData(it->first, _ChartType, _Cycle, _Id, 0);
			requested = true;
			break;
		}

		mainApp.SymMgr()->RegisterSymbolToServer(it->first, true);
	}

	return requested;
}
