#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../Quote/SmQuote.h"
#include "../Hoga/SmHoga.h"
#include "SymbolConst.h"
namespace DarkHorse {
	class SmChartData;
	class SmSymbol
	{
	public:
		// shared_ptr에서 static 함수는 쓰레드에서 사용하지 않는게 좋다. 
		// 쓰레드에서 사용하고 있을 경우 shared_ptr은 메모리에서 해제되지 않고 두번 해제되는등 
		// 예상치 못한 행동을 할 수 있다. static 함수를 사용에 신중하길 바란다.
		//static void ShiftData(SmTick* data, const int& len, SmTick&& newValue);
		void ShiftDown(SmTick&& newValue);
		SmSymbol(std::string&& symbol_code);
		~SmSymbol();
		std::string SymbolCode() const { return _SymbolCode; }
		void SymbolCode(std::string val);
		std::string MarketName() const { return _MarketName; }
		void MarketName(std::string val) { _MarketName = val; }
		std::string ProductCode() const { return _ProductCode; }
		void ProductCode(std::string val) { _ProductCode = val; }
		int seung_su() const { return seung_su_; }
		void seung_su(int val) { seung_su_ = val; }
		double CtrtSize() const { return _CtrtSize; }
		void CtrtSize(double val) { _CtrtSize = val; }
		double TickValue() const { return _TickValue; }
		void TickValue(double val) { _TickValue = val; }
		double TickSize() const { return _TickSize; }
		void TickSize(double val) { _TickSize = val; }
		int decimal() const { return decimal_; }
		void decimal(int val) { decimal_ = val; }
		std::string ExpireDate() const { return _ExpireDate; }
		void ExpireDate(std::string val) { _ExpireDate = val; }
		std::string SymbolNameKr() const { return _SymbolNameKr; }
		void SymbolNameKr(std::string val) { _SymbolNameKr = val; }
		std::string SymbolNameEn() const { return _SymbolNameEn; }
		void SymbolNameEn(std::string val) { _SymbolNameEn = val; }
		int Id() const { return _Id; }
		void Id(int val) { _Id = val; }
		std::string StartTime() const { return _StartTime; }
		void StartTime(std::string val) { _StartTime = val; }
		std::string EndTime() const { return _EndTime; }
		void EndTime(std::string val) { _EndTime = val; }
		int TotalVolume() const { return _TotalVolume; }
		void TotalVolume(int val) { _TotalVolume = val; }
		
		
		std::string Deposit() const { return _Deposit; }
		void Deposit(std::string val) { _Deposit = val; }
		std::string Currency() const { return _Currency; }
		void Currency(std::string val) { _Currency = val; }
		std::string Exchange() const { return _Exchange; }
		void Exchange(std::string val) { _Exchange = val; }
		std::string PreDayRate() const { return _PreDayRate; }
		void PreDayRate(std::string val) { _PreDayRate = val; }
		int PreDayVolume() const { return _PreDayVolume; }
		void PreDayVolume(int val) { _PreDayVolume = val; }

		std::string FullCode() const { return _FullCode; }
		void FullCode(std::string val) { _FullCode = val; }
		int RemainDays() const { return _RemainDays; }
		void RemainDays(int val) { _RemainDays = val; }
		std::string LastTradeDay() const { return _LastTradeDay; }
		void LastTradeDay(std::string val) { _LastTradeDay = val; }
		std::string HighLimitPrice() const { return _HighLimitPrice; }
		void HighLimitPrice(std::string val) { _HighLimitPrice = val; }
		std::string LowLimitPrice() const { return _LowLimitPrice; }
		void LowLimitPrice(std::string val) { _LowLimitPrice = val; }
		std::string PreDayClose() const { return _PreDayClose; }
		void PreDayClose(std::string val) { _PreDayClose = val; }
		std::string StandardPrice() const { return _StandardPrice; }
		void StandardPrice(std::string val) { _StandardPrice = val; }
		std::string Strike() const { return _Strike; }
		void Strike(std::string val) { _Strike = val; }
		int AtmType() const { return _AtmType; }
		void AtmType(int val) { _AtmType = val; }
		int RecentMonth() const { return _RecentMonth; }
		void RecentMonth(int val) { _RecentMonth = val; }
		bool Master_requested() const { return master_requested_; }
		void Master_requested(bool val) { master_requested_ = val; }
		bool quote_requested() const { return quote_requested_; }
		void quote_requested(bool val) { quote_requested_ = val; }
		bool hoga_requested() const { return hoga_requested_; }	
		void hoga_requested(bool val) { hoga_requested_ = val; }
		SymbolType symbol_type() const { return symbol_type_; }
		void symbol_type(SymbolType val) { symbol_type_ = val; }

	private:
		SymbolType symbol_type_{ SymbolType::None };
		bool hoga_requested_{ false };
		bool quote_requested_{ false };
		bool master_requested_{ false };
		/// <summary>
		/// 0 : future, 1 : atm , 2 : itm, 3 : otm
		/// </summary>
		int _AtmType{ 0 };
		/// <summary>
		/// 1 : 최근원물, 선물 스프레드, 2 : 2째월물, 3등등.
		/// </summary>
		int _RecentMonth{ 1 };
		std::string _Strike;
		std::string _StandardPrice;
		std::string _PreDayClose;
		std::string _LowLimitPrice;
		std::string _HighLimitPrice;
		std::string _LastTradeDay;
		int _RemainDays{ 0 };
		
		int _Id{ 0 };
		int seung_su_{ 250000 };
		int decimal_{ 2 };
		double _CtrtSize{ 0.05 };
		double _TickValue{ 12500 };
		double _TickSize{ 0.05 };
		std::string _FullCode;
		std::string _SymbolNameKr;
		std::string _SymbolNameEn;
		std::string _SymbolCode;
		std::string _MarketName;
		std::string _ProductCode;
		std::string _ExpireDate;
		int _TotalVolume{ 0 };
		int _PreDayVolume{ 0 };

		std::string _Deposit;
		
		std::string _StartTime;
		std::string _EndTime;
		std::string _PreDayRate;
		std::string _Currency;
		std::string _Exchange;
		std::map<int, std::shared_ptr<DarkHorse::SmChartData>> _ChartDataMap;
	public:
		void AddChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data);
		void UpdateChartData(DarkHorse::SmTick&& tick_data);
		SmHoga Hoga;
		SmQuote Qoute;
		std::vector<SmTick> TickVec;
	};

	struct strike_less_than_key
	{
		inline bool operator() (const std::shared_ptr<SmSymbol> struct1, const std::shared_ptr<SmSymbol> struct2)
		{
			return (struct1->Strike() < struct2->Strike());
		}
	};
}

