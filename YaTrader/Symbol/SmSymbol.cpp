#include "stdafx.h"
#include "SmSymbol.h"
#include "../Chart/SmChartData.h"
#include "../Util/IdGenerator.h"
#include "../Log/MyLogger.h"
namespace DarkHorse {


	void SmSymbol::ShiftDown(SmTick&& newValue)
	{
		//SmTick* data = TickVec.data();
		//const size_t len = TickVec.size();
		//memmove(data + 1, data, sizeof(*data) * (len - 1));
		std::rotate(TickVec.rbegin(), TickVec.rbegin() + 1, TickVec.rend());
		TickVec[0] = std::move(newValue);
	}

	SmSymbol::SmSymbol(std::string&& symbol_code)
		: _SymbolCode(symbol_code)
	{
		_Id = IdGenerator::get_id();
		TickVec.resize(26);
	}

	SmSymbol::~SmSymbol()
	{
		int i = 0;
		i = i + 0;

		CString msg;
		msg.Format("SymbolCode = %s\n", SymbolCode().c_str());
		//TRACE(msg);
	}
	void SmSymbol::SymbolCode(std::string val){
		_SymbolCode = val;

		LOGINFO(CMyLogger::getInstance(), "set SymbolCode [%s]", _SymbolCode.c_str());
	}
	void SmSymbol::AddChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data)
	{
		if (!chart_data) return;

		_ChartDataMap[chart_data->Id()] = chart_data;
	}

	void SmSymbol::UpdateChartData(DarkHorse::SmTick&& tick_data)
	{
		for (auto it = _ChartDataMap.begin(); it != _ChartDataMap.end(); it++) {
			it->second->OnTickData(tick_data);
		}
	}

}
