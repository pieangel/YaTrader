#include "stdafx.h"
#include <string>
#include "../Config/SmConfigManager.h"
#include "../Log/MyLogger.h"
#include "SymbolManager.h"
#include "../Util/VtStringUtil.h"
namespace DarkHorse {
void SymbolManager::read_domestic_masterfile()
{
	try {
		std::string file_path;
		file_path = SmConfigManager::GetApplicationPath();
		file_path.append(_T("\\"));
		file_path.append(_T("mst"));
		file_path.append(_T("\\"));
		std::string file_name = "chocode.cod";
		//TRACE(file_name.c_str());
		std::string full_name = file_path + file_name;
		std::ifstream infile(full_name);
		std::string line;
		int index = 0;
		std::string value;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);
			Symbol symbol;
			value = line.substr(index, 9); index += 9;
			VtStringUtil::trim(value);
			symbol.symbol_code = value;
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol.full_code = value;
			value = line.substr(index, 30); index += 30;
			VtStringUtil::trim(value);
			symbol.name_kr = value;
			value = line.substr(index, 30); index += 30;
			VtStringUtil::trim(value);
			symbol.name_en = value;
			value = line.substr(index, 5); index += 5;
			VtStringUtil::trim(value);
			symbol.remain_days = _ttoi(value.c_str());
			value = line.substr(index, 8); index += 8;
			VtStringUtil::trim(value);
			symbol.last_trade_day = value;
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol.high_limit_price = value;
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol.low_limit_price = value;
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol.preday_close = value;
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol.standard_price = value;
			value = line.substr(index, 17); index += 17;
			VtStringUtil::trim(value);
			symbol.strike = value;
			value = line.substr(index, 1); index += 1;
			VtStringUtil::trim(value);
			symbol.atm_type = _ttoi(value.c_str());
			value = line.substr(index, 1); index += 1;
			VtStringUtil::trim(value);
			symbol.recent_month = _ttoi(value.c_str());
			value = line.substr(index, 8);
			symbol.expire_day = value;
			add_symbol(std::move(symbol));
			index = 0;
			LOGINFO(CMyLogger::getInstance(), "read symbol %s complete!", symbol.symbol_code.c_str());
		}

		LOGINFO(CMyLogger::getInstance(), "read %s file complete!", full_name.c_str());
	}
	catch (std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}

void SymbolManager::add_symbol(Symbol&& symbol)
{
	symbol_map_[symbol.symbol_code] = std::move(symbol);
}

}