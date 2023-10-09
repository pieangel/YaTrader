#pragma once
#include <string>
#include <vector>
#include <memory>
#include <set>
#include "SmChartConsts.h"
#include "../ChartDialog/SmChartStorage.h"
#include "../Quote/SmQuote.h"
#include "../DataFrame/DataFrame.h"  // Main DataFrame header
#include "../DataFrame/Utils/DateTime.h"
#include "../DataFrame/Utils/FixedSizePriorityQueue.h"
#include "../DataFrame/Utils/FixedSizeString.h"
#include <mutex>
namespace DarkHorse {
	struct SmStockData {
		hmdf::DateTime date_time;
		double open;
		double high;
		double low;
		double close;
		double volume;
	};
	class SmTimeData;
	class SmChartData;
	class SmCompData;
	/// <summary>
	/// 차트타입 약자 : DA : day, WE : week, MO : month, HO : hour, MI : minute, TI : Tick, Year : YE
	/// 차트데이터 칼럼 이름 붙이기 규칙 : 차트타입(2자)_(사이클)_데이터이름
	/// 실례) 120틱 종가 : TI_120_C
	/// 1분 고가 : MI_1_H
	/// 일데이터 저가 : DA_1_L
	/// </summary>
	class SmChartData : public std::enable_shared_from_this<SmChartData>
	{
	public:
		explicit SmChartData(const std::string& symbol_code, const SmChartType& chart_type, const int& cycle, const int& id)
			: _SymbolCode(symbol_code), _ChartType(chart_type), _Cycle(cycle), _Id(id) {};
		~SmChartData() {};
		std::string GetChartDataKey();
		std::string SymbolCode() const { return _SymbolCode; }
		void SymbolCode(std::string val) { _SymbolCode = val; }
		int Cycle() const { return _Cycle; }
		void Cycle(int val) { _Cycle = val; }
		SmChartType ChartType() const { return _ChartType; }
		void ChartType(SmChartType val) { _ChartType = val; }
		bool Received() const { return _Received; }
		void Received(bool val) { _Received = val; }

		void ShiftData(double* data, const int& len, const double& newValue);
		void ShiftData(int* data, const int& len, const int& newValue);

		void AddChartData(const double& h, const double& l, const double& o, const double& c, const double& v, const int& date, const int& time, const int& mil_sec);
		//void SetChartData(std::vector<double>&& h_v, std::vector<double>&& l_v, std::vector<double>&& o_v, std::vector<double>&& c_v, std::vector<double>&& v_v, std::vector<hmdf::DateTime>&& dt_v);
		void SetChartData(std::vector<double>&& cd_v, std::vector<double>&& h_v, std::vector<double>&& l_v, std::vector<double>&& o_v, std::vector<double>&& c_v, std::vector<double>&& v_v, std::vector<hmdf::DateTime>&& dt_v);

		void AddChartData(const double& dt, const double& open, const double& high, const double& low, const double& close);
		void SetChartData(const double& dt, const double& close, const double& high, const double& low);

		void ClearData();
		size_t CreateDataFrame(std::vector<double>& cd_v, std::vector<double>& h_v, std::vector<double>& l_v, std::vector<double>& o_v, std::vector<double>& c_v, std::vector<double>& v_v, std::vector<hmdf::DateTime>& dt_v);
		void InitDataFrame();
		size_t GetChartDataCount() {
			return _DataFrame.shape().first;
		}
		int Id() const { return _Id; }
		void Id(int val) { _Id = val; }
		// 타이머가 불릴때 호출 되는 함수
		/// <summary>
		/// 타이머가 불릴 때 사용자 목록을 참조하여 차트 데이터를 조작하여 클라이언트에게 전송한다.
		/// </summary>
		void OnTimer();
		void OnTickData(const SmTick& tick_data);
		void UpdateLastData(const SmTick& tick_data);
		void GetLastData(SmStockData& last_data);
		CExternalStorage ExternalStorage;
		

		std::vector< hmdf::DateTime>& GetIndex() {
			return _DataFrame.get_index();
		}

		hmdf::StdDataFrame<hmdf::DateTime>& GetDataFrame() {
			return _DataFrame;
		}
		void AddCompData(DarkHorse::SmCompData* data) {
			if (!data) return;

			_CompSet.insert(data);
		}
		void RemoveCompData(DarkHorse::SmCompData* data) {
			if (!data) return;

			auto found = _CompSet.find(data);
			if (found == _CompSet.end()) return;
			_CompSet.erase(found);
		}

		void ProcessDataPerBar();
		void ProcessMultiColorLine();
		int GetMultiColorValue(const double& close, const double& ma20, const double& ma40, const double& ma60, const double& ma120);
		std::string Currency() const { return _Currency; }
		void Currency(std::string val) { _Currency = val; }
		std::string Exchange() const { return _Exchange; }
		void Exchange(std::string val) { _Exchange = val; }
		void JoinChartData(const SmChartType& src_chart_type, const int& src_cycle);
		std::string ProductCode() const { return _ProductCode; }
		void ProductCode(std::string val) { _ProductCode = val; }
		std::string MakePrefix();
	private:
		std::shared_ptr<DarkHorse::SmChartData> _PnfData = nullptr;
		std::set<DarkHorse::SmCompData*> _CompSet;
		void MakeNewBarByTimer();
		int _Id = 0;
		std::string _SymbolCode;
		std::string _ProductCode;
		int _Cycle{ 1 };
		SmChartType _ChartType = SmChartType::MIN;
		bool _Received{ false };
		int _TickCount{ 0 };
		hmdf::StdDataFrame<hmdf::DateTime> _DataFrame;
		std::mutex _m;
		std::string _Currency = "USD";
		std::string _Exchange = "CME";
		int heiken_ashi_smooth_period = 2;
		void UpdatHeikenAsh();
		void UpdateMa();
	};
}

