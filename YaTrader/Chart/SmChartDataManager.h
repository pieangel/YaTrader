#pragma once
#include <string>
#include <map>
#include <memory>
#include "SmChartConsts.h"
#include "../Time/cpptime.h"
#include "../Quote/SmQuote.h"
namespace DarkHorse {
	class SmChartData;
	class SmChartDataManager
	{
	public:
		SmChartDataManager();
		~SmChartDataManager();
		// Static Members
		static int _Id;
		static int GetId() { return _Id++; }
		static std::string MakeDataKey(const std::string& symbol_code, const int& chart_type, const int& cycle) {
			std::string data_key = symbol_code;
			data_key.append(":");
			data_key.append(std::to_string(chart_type));
			data_key.append(":");
			data_key.append(std::to_string(cycle));
			return data_key;
		}
		std::shared_ptr<SmChartData> FindChartData(const std::string& chart_data_key);
		std::shared_ptr<SmChartData> FindChartData(const std::string& symbol_code, const SmChartType& chart_type, const int& cycle);
		std::shared_ptr<SmChartData> FindAddChartData(const std::string& symbol_code, const SmChartType& chart_type, const int& cycle);
		void RequestChartData(const std::string& symbol_code, const int& window_id, const int& series_index);
		void RequestChartData(const std::string& symbol_code, const SmChartType& chart_type, const int& cycle, const int& window_id, const int& series_index);
		void RequestChartData(const std::string& symbol_code, const SmChartType& chart_type, const int& cycle, const int& count, const int& window_id, const int& series_index);

		const std::map<std::string, std::shared_ptr<SmChartData>>& GetChartDataMap() { return _ChartDataMap; };

		void MergeChartData(const std::string& symbol_code, const SmChartType& src_chart_type1, const int& src_cycle1, const SmChartType& src_chart_type2, const int& src_cycle2);
		
	private:
		void CreateTimer(std::shared_ptr<SmChartData> chart_data);
		std::map<std::string, std::shared_ptr<SmChartData>> _ChartDataMap;

		// 차트데이터를 주기적으로 받기 위한 타이머 맵
		std::map<std::string, CppTime::timer_id> _TimerMap;
		// 타이머 생성을 위한 타이머 객체
		CppTime::Timer _Timer;
	};
}

