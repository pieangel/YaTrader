#pragma once
#include <map>
#include <string>
#include <vector>
#include "../Chart/SmChartConsts.h"
#include "../ChartDialog/SmChartStorage.h"
namespace DarkHorse {
	class SmTimeData
	{
	public:
		explicit SmTimeData(const std::string name, const int& id) : _Name(name), _Id(id) { };
		~SmTimeData() {};
		void AddValue(const int& date, const int& time, const std::string& column_name, const double& value);
		void UpdateValue(const int& date, const int& time, const std::string& column_name, const double& value);
		int Id() const { return _Id; }
		void Id(int val) { _Id = val; }
		CExternalStorage ExternalStorage;
		std::string Name() const { return _Name; }
		void Name(std::string val) { _Name = val; }
		void AddSymbol(const std::string& symbol_code) {
			_SymbolMap[symbol_code] = false;
		}
		void RemoveSymbol(const std::string& symbol_code);
		bool RequestChartData();
	private:
		void AddColumn(const std::string& column_name);
		// key : datetime string, value : a pair of a date int and a time int. 
		std::map<std::string, COleDateTime > _Index;
		// key : column name, value : data values of the column
		std::map<std::string, std::vector<double>> _ColumnDataMap;
		int _Id;
		std::string _Name;

		int _Cycle{ 1 };
		SmChartType _ChartType = SmChartType::MIN;
		bool _Received{ false };
		// key : symbol code, value : chart data received or not
		std::map<std::string, bool> _SymbolMap;
		bool _Enable = false;
	};
}

