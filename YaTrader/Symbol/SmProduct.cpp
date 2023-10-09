#include "stdafx.h"
#include "SmProduct.h"
#include "SmSymbol.h"
#include "SmSymbolManager.h"
#include "SmProductYearMonth.h"
#include "../Global/SmTotalManager.h"
#include "../Log/MyLogger.h"
namespace DarkHorse {

std::shared_ptr<DarkHorse::SmSymbol> SmProduct::AddSymbol(std::string&& symbol_code)
{
	try {
		auto symbol = std::make_shared<DarkHorse::SmSymbol>(std::move(symbol_code));
		if (!symbol) return nullptr;

		symbol->Id(SmSymbolManager::GetId());
		_SymbolVec.push_back(symbol);
		mainApp.SymMgr()->AddSymbol(symbol);
		return symbol;
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error);
	}

	return nullptr;
}

std::shared_ptr<SmProductYearMonth> SmProduct::add_year_month(const std::string& year_month_name)
{
	auto it = _YearMonthMap.find(year_month_name);
	if (it != _YearMonthMap.end()) return it->second;
	std::shared_ptr<SmProductYearMonth> year_month = std::make_shared<SmProductYearMonth>(year_month_name);
	_YearMonthMap[year_month_name] = year_month;
	return year_month;
}

void SmProduct::sort_dm_option_symbol_vector()
{
	if (_YearMonthMap.size() == 0) return;

	for (auto it = _YearMonthMap.begin(); it != _YearMonthMap.end(); ++it) {
		it->second->sort_symbol_vector();
	}
}

}
