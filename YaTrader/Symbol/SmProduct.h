#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace DarkHorse {
	class SmSymbol;
	class SmProductYearMonth;
	class SmProduct
	{
	public:
		const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& GetSymbolVec() const { return _SymbolVec; }
		std::shared_ptr<DarkHorse::SmSymbol> AddSymbol(std::string&& symbol_code);
		
		SmProduct(const std::string& product_code) : _ProductCode(product_code) {}
		std::string ProductCode() const { return _ProductCode; }
		void ProductCode(std::string val) { _ProductCode = val; }
		std::string ProductNameEn() const { return _ProductNameEn; }
		void ProductNameEn(std::string val) { _ProductNameEn = val; }
		std::string ProductNameKr() const { return _ProductNameKr; }
		void ProductNameKr(std::string val) { _ProductNameKr = val; }
		std::string MarketName() const { return _MarketName; }
		void MarketName(std::string val) { _MarketName = val; }
		std::string ExchangeName() const { return _ExchangeName; }
		void ExchangeName(std::string val) { _ExchangeName = val; }
		std::shared_ptr<SmProductYearMonth> add_year_month(const std::string& year_month_name);
		const std::map<std::string, std::shared_ptr<SmProductYearMonth>>& get_yearmonth_map() {
			return _YearMonthMap;
		}
		const std::string get_first_year_month_name() {
			if (_YearMonthMap.size() == 0) return "";
			return _YearMonthMap.begin()->first;
		}
		void sort_dm_option_symbol_vector();

		int decimal() const { return decimal_; }
		void decimal(int val) { decimal_ = val; }

		int int_tick_size() const { return int_tick_size_; }
		void int_tick_size(int val) { int_tick_size_ = val; }

		int tick_value() const { return tick_value_; }
		void tick_value(int val) { tick_value_ = val; }

		std::string tick_size() const { return tick_size_; }
		void tick_size(std::string val) { tick_size_ = val; }

		int seung_su() const { return seung_su_; }
		void seung_su(int val) { seung_su_ = val; }

		int hoga_unit() const { return hoga_unit_; }
		void hoga_unit(int val) { hoga_unit_ = val; }

	private:

		// 소수점
		int decimal_;
		// 정수 틱크기
		int int_tick_size_;
		// 틱크기 - 문자열
		std::string tick_size_;
		// 틱가치 - 정수
		int tick_value_;
		// 승수
		int seung_su_;
		// 호가 단위 정수
		int hoga_unit_;
		// 품목 이름
		//std::string name;

		std::string _ProductCode;
		std::string _ProductNameEn;
		std::string _ProductNameKr;
		std::string _MarketName;
		std::string _ExchangeName;
		// key : year month name, value : year month object.
		std::map<std::string, std::shared_ptr<SmProductYearMonth>> _YearMonthMap;
		// symbol vector for the future product.
		std::vector<std::shared_ptr<DarkHorse::SmSymbol>> _SymbolVec;
	};
}

