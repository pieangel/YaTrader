#pragma once
#include "../DataFrame/DataFrame.h"
#include "../time/cpptime.h"
#include <memory>
#include <string>
#include <map>
namespace DarkHorse {
	class SmChartData;
	class SmSystem;
	class SmSystemData
	{
	public:
		/// <summary>
		/// Get the Data Frame Object.
		/// </summary>
		/// <returns></returns>
		hmdf::StdDataFrame<hmdf::DateTime>& GetDataFrame() {
			return _DataFrame;
		}
		/// <summary>
		/// Get the count of all data.
		/// </summary>
		/// <returns></returns>
		size_t GetDataCount() {
			return _DataFrame.shape().first;
		}
		/// <summary>
		/// Set the chart data for this System data.
		/// You must set the chart data before calling any function on this class.
		/// </summary>
		/// <param name="chart_data"></param>
		void SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data);
		void JoinAllChartData(const std::string& symbol_code);
		void ProcessData();
		void Sysmtem(std::weak_ptr<DarkHorse::SmSystem> val) { _System = val; }
	private:
		hmdf::StdDataFrame<hmdf::DateTime> _DataFrame;
		std::weak_ptr<DarkHorse::SmSystem> _System;
		hmdf::StdDataFrame<hmdf::DateTime> _JoinedDataFrame;
		std::map<std::string, std::shared_ptr<DarkHorse::SmChartData>> _ChartDataMap;
		std::shared_ptr<DarkHorse::SmChartData> _MainChartData = nullptr;
	};
}

