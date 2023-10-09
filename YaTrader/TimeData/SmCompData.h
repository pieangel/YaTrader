#pragma once
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include "../Chart/SmChartConsts.h"
#include "../ChartDialog/SmChartStorage.h"
#include "../DataFrame/DataFrame.h"
#include "../time/cpptime.h"

#define UM_COMP_ADDED WM_USER + 4
#define UM_COMP_UPDATED WM_USER + 5

enum OperOption {
	OP_KOSPI,
	OP_HS,
	OP_NK,
	OP_NQ
};

class CWnd;
namespace DarkHorse {
	class SmChartData;
	// 자체적인 타이머를 가지고 봉을 만들어 간다.
	// 원 차트 데이터의 시간에 상관없이 그 당시 종가를 가져와 봉을 만든다. 
	// 그 이후는 차트 데이터와 동일하게 업데이트 한다.
	class SmCompData
	{
	public:
		explicit SmCompData();
		~SmCompData() ;
		bool RequestChartData(const int& wnd_id);
		void SetKospiSymbol(const std::string& symbol_code) {
			_KospiSymbolCode = symbol_code;
		}
		void SetNqSymbol(const std::string& symbol_code) {
			_NqSymbolCode = symbol_code;
		}
		// action :: 0: received, 1 : update, 2 : add
		void OnChartEvent(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action);
		void MakeChartData();
		void ProcessChartData();
		void ReprocessChartData();
		void CalcMultiLineColorPolicy();
		hmdf::StdDataFrame<hmdf::DateTime>& GetDataFrame() {
			return _DataFrame;
		}
		void AddChartData(const double& h, const double& l, const double& o, const double& c, const double& v, const int& date, const int& time, const int& mil_sec);
		std::vector<double> GetLastData();
		size_t GetDataCount() {
			return _DataFrame.shape().first;
		}
		CExternalStorage MainStorage;
		CExternalStorage CompStorage;
		CExternalStorage VirtualStorage;
		std::string KospiSymbolCode() const { return _KospiSymbolCode; }
		void KospiSymbolCode(std::string val) { _KospiSymbolCode = val; }
		std::string NqSymbolCode() const { return _NqSymbolCode; }
		void NqSymbolCode(std::string val) { _NqSymbolCode = val; }
		CWnd* Parent() const { return _Parent; }
		void Parent(CWnd* val) { _Parent = val; }
		void RemoveChartRef();
		void UpdateChartData(const std::string& symbol_code, const double& close);
		void Multiply(int val);
		std::string NikkeiCode() const { return _NikkeiCode; }
		void NikkeiCode(std::string val) { _NikkeiCode = val; }
		std::string HangSengCode() const { return _HangSengCode; }
		void HangSengCode(std::string val) { _HangSengCode = val; }
		bool Ready() const { return _Ready; }
		void Ready(bool val) { _Ready = val; }
		void AddBaseValue(const double& value) {
			_BaseValues.insert(value);
		}
		const std::set<double>& GetBaseValues() {
			return _BaseValues;
		}
		double NqRatio1() const { return _NqRatio1; }
		void NqRatio1(double val) { _NqRatio1 = val; }
		double NqRatio2() const { return _NqRatio2; }
		void NqRatio2(double val) { _NqRatio2 = val; }
		double NqRatio3() const { return _NqRatio3; }
		void NqRatio3(double val) { _NqRatio3 = val; }
		double NqAvgRatio1() const { return _NqAvgRatio1; }
		void NqAvgRatio1(double val) { _NqAvgRatio1 = val; }
		double NqAvgRatio2() const { return _NqAvgRatio2; }
		void NqAvgRatio2(double val) { _NqAvgRatio2 = val; }
		double NqAvgRatio3() const { return _NqAvgRatio3; }
		void NqAvgRatio3(double val) { _NqAvgRatio3 = val; }
		double NkRatio1() const { return _NkRatio1; }
		void NkRatio1(double val) { _NkRatio1 = val; }
		double NkRatio2() const { return _NkRatio2; }
		void NkRatio2(double val) { _NkRatio2 = val; }
		double NkRatio3() const { return _NkRatio3; }
		void NkRatio3(double val) { _NkRatio3 = val; }
		double NkAvgRatio1() const { return _NkAvgRatio1; }
		void NkAvgRatio1(double val) { _NkAvgRatio1 = val; }
		double NkAvgRatio2() const { return _NkAvgRatio2; }
		void NkAvgRatio2(double val) { _NkAvgRatio2 = val; }
		double NkAvgRatio3() const { return _NkAvgRatio3; }
		void NkAvgRatio3(double val) { _NkAvgRatio3 = val; }
		double HsRatio1() const { return _HsRatio1; }
		void HsRatio1(double val) { _HsRatio1 = val; }
		double HsRatio2() const { return _HsRatio2; }
		void HsRatio2(double val) { _HsRatio2 = val; }
		double HsRatio3() const { return _HsRatio3; }
		void HsRatio3(double val) { _HsRatio3 = val; }
		double HsAvgRatio1() const { return _HsAvgRatio1; }
		void HsAvgRatio1(double val) { _HsAvgRatio1 = val; }
		double HsAvgRatio2() const { return _HsAvgRatio2; }
		void HsAvgRatio2(double val) { _HsAvgRatio2 = val; }
		double HsAvgRatio3() const { return _HsAvgRatio3; }
		void HsAvgRatio3(double val) { _HsAvgRatio3 = val; }
		double KpRatio1() const { return _KpRatio1; }
		void KpRatio1(double val) { _KpRatio1 = val; }
		double KpRatio2() const { return _KpRatio2; }
		void KpRatio2(double val) { _KpRatio2 = val; }
		double KpRatio3() const { return _KpRatio3; }
		void KpRatio3(double val) { _KpRatio3 = val; }
		double KpAvgRatio1() const { return _KpAvgRatio1; }
		void KpAvgRatio1(double val) { _KpAvgRatio1 = val; }
		double KpAvgRatio2() const { return _KpAvgRatio2; }
		void KpAvgRatio2(double val) { _KpAvgRatio2 = val; }
		double KpAvgRatio3() const { return _KpAvgRatio3; }
		void KpAvgRatio3(double val) { _KpAvgRatio3 = val; }
		OperOption OpOption() const { return _OpOption; }
		void OpOption(OperOption val) { _OpOption = val; }
	private:

		std::pair<int, int> _IdxTime;

		OperOption _OpOption = OP_KOSPI;

		std::set<double> _BaseValues;

		hmdf::StdDataFrame<COleDateTime> new_data;

		double _KpRatio1 = 1.0;
		double _KpRatio2 = 2.0;
		double _KpRatio3 = 3.0;

		double _KpAvgRatio1 = 1.0;
		double _KpAvgRatio2 = 0.0;
		double _KpAvgRatio3 = 0.0;


		double _NqRatio1 = 1.0;
		double _NqRatio2 = 1.0;
		double _NqRatio3 = 1.0;

		double _NqAvgRatio1 = 1.0;
		double _NkAvgRatio1 = 0.0;
		double _HsAvgRatio1 = 0.0;


		double _NqAvgRatio2 = 0.5;
		double _NkAvgRatio2 = 0.5;
		double _HsAvgRatio2 = 0.0;


		double _NqAvgRatio3 = 0.4;
		double _NkAvgRatio3 = 0.3;
		double _HsAvgRatio3 = 0.3;



		double _NkRatio1 = 1.0;
		double _NkRatio2 = 1.0;
		double _NkRatio3 = 1.0;


		double _HsRatio1 = 1.0;
		double _HsRatio2 = 1.0;
		double _HsRatio3 = 1.0;


		int _Multiply = 1;
		// 타이머가 불릴때 호출 되는 함수
		/// <summary>
		/// 타이머가 불릴 때 사용자 목록을 참조하여 차트 데이터를 조작하여 클라이언트에게 전송한다.
		/// </summary>
		void OnTimer();
		void CreateTimer();
		int _Cycle{ 1 };
		SmChartType _ChartType = SmChartType::MIN;
		std::string _KospiSymbolCode;
		std::string _NqSymbolCode;
		std::string _NikkeiCode;
		std::string _HangSengCode;
		hmdf::StdDataFrame<hmdf::DateTime> _TempFrame;
		hmdf::StdDataFrame<hmdf::DateTime> _DataFrame;
		CppTime::timer_id _TimerId = 0;
		// 타이머 생성을 위한 타이머 객체
		CppTime::Timer _Timer;
		// Unique id for this object.
		int _Id = 0;
		bool _Ready = false;
		CWnd* _Parent = nullptr;
		std::string _KpPrefix;
		std::string _NqPrefix;
		std::string _NkPrefix;
		std::string _HsPrefix;
	};
}

