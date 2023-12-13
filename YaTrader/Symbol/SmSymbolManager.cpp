#include "stdafx.h"
#include "SmSymbolManager.h"
#include "SmMarket.h"
#include "SmProduct.h"
#include "../Global/SmTotalManager.h"
#include "SmSymbolReader.h"
#include "SmSymbol.h"
#include "../Task/SmTaskRequestMaker.h"
#include "../Task/SmServerDataReceiver.h"
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "../Config/SmConfigManager.h"
#include "../Log/MyLogger.h"
#include "../Util/VtStringUtil.h"
#include "SmProductYearMonth.h"
#include "../Quote/SmQuoteManager.h"
#include "../Quote/SmQuote.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace DarkHorse {
int SmSymbolManager::_Id = 0;

std::shared_ptr<SmMarket> SmSymbolManager::AddMarket(const std::string& market_name)
{
	const auto found = _MarketMap.find(market_name);
	if (found != _MarketMap.end()) return found->second;
	auto market = std::make_shared<SmMarket>(market_name);
	_MarketMap[market_name] = market;
	return market;
}

SmSymbolManager::SmSymbolManager()
{
	InitFavoriteProduct();
	InitDomesticProducts();
	market_code_map_init_dm_ya();
}

SmSymbolManager::~SmSymbolManager()
{

}

void DarkHorse::SmSymbolManager::InitDomesticProducts()
{
	_DomesticProductVec.push_back("101F");
}

void SmSymbolManager::market_code_map_init_dm_ya()
{
	ya_market_code_map_["21"] = "지수선물";
	ya_market_code_map_["61"] = "미니선물";
	ya_market_code_map_["84"] = "코닥선물";
	ya_market_code_map_["45"] = "상품선물";
	ya_market_code_map_["22"] = "코스피옵션";
	ya_market_code_map_["53"] = "코스닥옵션";
	ya_market_code_map_["64"] = "미니코스피옵션";
	ya_market_code_map_["94"] = "코스피위클리옵션";
}

void SmSymbolManager::add_to_yearmonth(std::shared_ptr<SmSymbol> symbol)
{
	const std::string product_code = symbol->ProductCode();
	const std::string symbol_name_en = symbol->SymbolNameEn();
	const std::string market_name = symbol->MarketName();
	if (symbol->ProductCode() == "101") { // KOSPI 200 F 202303
		std::string year = symbol_name_en.substr(12, 4);
		std::string month = symbol_name_en.substr(16, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "105") { // MINI KOSPI F 202303
		std::string year = symbol_name_en.substr(13, 4);
		std::string month = symbol_name_en.substr(17, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "106") { // KOSDAQ150 F 202303
		std::string year = symbol_name_en.substr(12, 4);
		std::string month = symbol_name_en.substr(16, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "167") { // KTB10      F 202303
		std::string year = symbol_name_en.substr(13, 4);
		std::string month = symbol_name_en.substr(17, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "175") { // USD F 202303
		std::string year = symbol_name_en.substr(6, 4);
		std::string month = symbol_name_en.substr(10, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "201") { // KOSPI 200 C 202303 160.0 
		std::string year = symbol_name_en.substr(12, 4);
		std::string month = symbol_name_en.substr(16, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "301") { // KOSPI 200 P 202303 340.0
		std::string year = symbol_name_en.substr(12, 4);
		std::string month = symbol_name_en.substr(16, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "205") { // MINI KOSPI C 202303 200.0 
		std::string year = symbol_name_en.substr(13, 4);
		std::string month = symbol_name_en.substr(17, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "305") { // MINI KOSPI P 202303 200.0
		std::string year = symbol_name_en.substr(13, 4);
		std::string month = symbol_name_en.substr(17, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "209") { // KOSPI WEEKLY C 2303W1 275.0
		std::string year = symbol_name_en.substr(15, 2);
		std::string month = symbol_name_en.substr(17, 2);
		std::string week = symbol_name_en.substr(19, 2);
		std::string year_month_name("20");
		year_month_name.append(year);
		year_month_name.append("-");
		year_month_name.append(month);
		year_month_name.append("-");
		year_month_name.append(week);
		year_month_name.append("-");
		year_month_name.append("T");
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "309") { // KOSPI WEEKLY P 2303W1 337.5
		std::string year = symbol_name_en.substr(15, 2);
		std::string month = symbol_name_en.substr(17, 2);
		std::string week = symbol_name_en.substr(19, 2);
		std::string year_month_name("20");
		year_month_name.append(year);
		year_month_name.append("-");
		year_month_name.append(month);
		year_month_name.append("-");
		year_month_name.append(week);
		year_month_name.append("-");
		year_month_name.append("T");
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "2AF") { // KOSPI WEEKLY M C 2308W1 302.5
		std::string year = symbol_name_en.substr(17, 2);
		std::string month = symbol_name_en.substr(19, 2);
		std::string week = symbol_name_en.substr(21, 2);
		std::string year_month_name("20");
		year_month_name.append(year);
		year_month_name.append("-");
		year_month_name.append(month);
		year_month_name.append("-");
		year_month_name.append(week);
		year_month_name.append("-");
		year_month_name.append("M");
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "3AF") { // KOSPI WEEKLY P 2303W1 337.5
		std::string year = symbol_name_en.substr(17, 2);
		std::string month = symbol_name_en.substr(19, 2);
		std::string week = symbol_name_en.substr(21, 2);
		std::string year_month_name("20");
		year_month_name.append(year);
		year_month_name.append("-");
		year_month_name.append(month);
		year_month_name.append("-");
		year_month_name.append(week);
		year_month_name.append("-");
		year_month_name.append("M");
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "206") { // KOSDAQ150 C 202303 1,275
		std::string year = symbol_name_en.substr(12, 4);
		std::string month = symbol_name_en.substr(16, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "306") { // KOSDAQ150 P 202303 1,275
		std::string year = symbol_name_en.substr(12, 4);
		std::string month = symbol_name_en.substr(16, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
}

void SmSymbolManager::add_to_yearmonth_dm_ya(std::shared_ptr<SmSymbol> symbol)
{
	const std::string product_code = symbol->ProductCode();
	const std::string symbol_name_kr = symbol->SymbolNameKr();
	const std::string market_name = symbol->MarketName();
	std::vector<std::string> name_vec = VtStringUtil::split(symbol_name_kr, " ", true);
	if (symbol->ProductCode() == "101") { // 코스피200 F 202312
		std::string year = name_vec.back().substr(0, 4);
		std::string month = name_vec.back().substr(4, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "105") { // 미니코스피 F 202311
		std::string year = name_vec.back().substr(0, 4);
		std::string month = name_vec.back().substr(4, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "106") { // 코스닥150 F 202312
		std::string year = name_vec.back().substr(0, 4);
		std::string month = name_vec.back().substr(4, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "167") { // 3년국채    F 202312
		std::string year = name_vec.back().substr(0, 4);
		std::string month = name_vec.back().substr(4, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "175") { // USD F 202303
		std::string year = symbol_name_kr.substr(0, 4);
		std::string month = symbol_name_kr.substr(4, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "201") { // 코스피200 C 202311 205.0 
		std::string year = name_vec[2].substr(0, 4);
		std::string month = name_vec[2].substr(4, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "301") { // 코스피200 P 202311 205.0
		std::string year = name_vec[2].substr(0, 4);
		std::string month = name_vec[2].substr(4, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "205") { // 미니코스피 C 202311 237.5 
		std::string year = name_vec[2].substr(0, 4);
		std::string month = name_vec[2].substr(4, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "305") { // 미니코스피 P 202311 237.5
		std::string year = name_vec[2].substr(0, 4);
		std::string month = name_vec[2].substr(4, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "209") { // 코스피위클리 C 2310W3 285.0
		std::string year = name_vec[2].substr(0, 2);
		std::string month = name_vec[2].substr(2, 2);
		std::string week = name_vec[2].substr(4, 2);
		std::string year_month_name("20");
		year_month_name.append(year);
		year_month_name.append("-");
		year_month_name.append(month);
		year_month_name.append("-");
		year_month_name.append(week);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "309") { // 코스피위클리 P 2310W3 285.0
		std::string year = name_vec[2].substr(0, 2);
		std::string month = name_vec[2].substr(2, 2);
		std::string week = name_vec[2].substr(4, 2);
		std::string year_month_name("20");
		year_month_name.append(year);
		year_month_name.append("-");
		year_month_name.append(month);
		year_month_name.append("-");
		year_month_name.append(week);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "2AF") { // 코스피위클리M C 2310M3 280.0
		std::string year = name_vec[2].substr(0, 2);
		std::string month = name_vec[2].substr(2, 2);
		std::string week = name_vec[2].substr(4, 2);
		std::string year_month_name("20");
		year_month_name.append(year);
		year_month_name.append("-");
		year_month_name.append(month);
		year_month_name.append("-");
		year_month_name.append(week);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "3AF") { // 코스피위클리M P 2310M3 280.0
		std::string year = name_vec[2].substr(0, 2);
		std::string month = name_vec[2].substr(2, 2);
		std::string week = name_vec[2].substr(4, 2);
		std::string year_month_name("20");
		year_month_name.append(year);
		year_month_name.append("-");
		year_month_name.append(month);
		year_month_name.append("-");
		year_month_name.append(week);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "206") { // 코스닥150 C 202311 1,275
		std::string year = name_vec[2].substr(0, 4);
		std::string month = name_vec[2].substr(4, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
	else if (product_code == "306") { // 코스닥150 P 202311 1,275
		std::string year = name_vec[2].substr(0, 4);
		std::string month = name_vec[2].substr(4, 2);
		std::string year_month_name(year);
		year_month_name.append("-");
		year_month_name.append(month);
		std::shared_ptr<SmProduct> product = find_product(market_name, product_code);
		if (!product) return;
		std::shared_ptr<SmProductYearMonth> year_month = product->add_year_month(year_month_name);
		year_month->AddSymbol(symbol);
	}
}

void SmSymbolManager::sort_dm_option_symbol_vector()
{
	for (size_t i = 0; i < _DomesticOptionVec.size(); i++) {
		_DomesticOptionVec[i].call_product->sort_dm_option_symbol_vector();
		_DomesticOptionVec[i].put_product->sort_dm_option_symbol_vector();
	}
}

void SmSymbolManager::set_domestic_symbol_info(std::shared_ptr<SmSymbol> symbol)
{
	if (!symbol) return;

	symbol->symbol_type(SymbolType::Domestic);
	symbol->StartTime("084500");
	symbol->EndTime("154500");
	symbol->Exchange("KRX");
	symbol->Currency("\\");
	set_product_info(symbol);
	//set_quote_preday_close(symbol, pre_day_close);
	AddSymbol(symbol);
	//LOGINFO(CMyLogger::getInstance(), "read symbol %s complete!", symbol->SymbolCode().c_str());
	add_to_yearmonth(symbol);
}

void SmSymbolManager::get_ab_recent_symbols(std::set<std::shared_ptr<SmSymbol>>& ab_symbol_set)
{
	for (auto it = Ab_Market_Set_.begin(); it != Ab_Market_Set_.end(); it++) {
		std::string market_name = *it;
		auto market = mainApp.SymMgr()->FindMarket(market_name);
		if (!market) continue;
		const std::map<std::string, std::shared_ptr<SmProduct>>& product_map = market->GetProductMap();
		for (auto it = product_map.begin(); it != product_map.end(); it++) {
			const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec = it->second->GetSymbolVec();
			if (symbol_vec.empty()) continue;
			ab_symbol_set.insert(*symbol_vec.begin());
		}
	}
}

std::shared_ptr<SmMarket> SmSymbolManager::get_dm_market_by_product_code(const std::string& product_code)
{
	if (product_code.empty()) return nullptr;
	if (product_code.substr(0, 1) == "1") {
		return get_market(DmFutureMarketName);
	}
	else if (product_code.substr(0, 1) == "2" ||
		product_code.substr(0, 1) == "3") {
		return get_market(DmOptionMarketName);
	}
	return nullptr;
}

void SmSymbolManager::read_domestic_productfile()
{
	try {
		std::string file_path;
		file_path = SmConfigManager::GetApplicationPath();
		file_path.append(_T("\\"));
		file_path.append(_T("table"));
		file_path.append(_T("\\"));
		std::string file_name = "dm_product.cod";
		//TRACE(file_name.c_str());
		std::string full_name = file_path + file_name;
		if (!fs::exists(full_name)) {
			AfxMessageBox("국내시장 제품 정보 파일(dm_product.cod)이 없습니다!");
			return;
		}
		std::ifstream infile(full_name);
		std::string line;
		std::string value;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);
			int index = 0;
			std::string temp;
			temp = line.substr(index, 8);
			VtStringUtil::rtrim(temp);
			auto market = mainApp.SymMgr()->get_dm_market_by_product_code(temp);
			if (!market) continue;
			std::shared_ptr<SmProduct> product = market->AddProduct(temp);
			if (!product) continue;
			index = index + 8;
			temp = line.substr(index, 2);
			product->decimal(std::stoi(temp));
			index = index + 2;
			temp = line.substr(index, 5);
			VtStringUtil::ltrim(temp);
			product->tick_size(temp);
			temp.erase(std::remove(temp.begin(), temp.end(), '.'), temp.end());
			product->int_tick_size(std::stoi(temp));
			index = index + 5;
			temp = line.substr(index, 5);
			product->tick_value(std::stoi(temp));
			index = index + 5;
			temp = line.substr(index, 10);
			VtStringUtil::ltrim(temp);
			product->seung_su(std::stoi(temp));
			index = index + 10;
			temp = line.substr(index, 30);
			VtStringUtil::ltrim(temp);
			product->ProductNameEn(temp);
		}

		LOGINFO(CMyLogger::getInstance(), "read %s file complete!", full_name.c_str());
	}
	catch (std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}
void SmSymbolManager::read_dm_masterfile_si()
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
		std::string value;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);
			int index = 0;
			value = line.substr(index + 1, 8); index += 9;
			VtStringUtil::trim(value);
			std::string symbol_code = value;
			std::shared_ptr<SmSymbol> symbol = std::make_shared<SmSymbol>(std::move(symbol_code));
			std::shared_ptr<SmQuote> quote_p = mainApp.QuoteMgr()->get_quote(value);
			const std::string market_name = symbol->SymbolCode().substr(0, 1).at(0) == '1' ? DmFutureMarketName : DmOptionMarketName;
			symbol->MarketName(market_name);
			const std::string product_code = symbol->SymbolCode().substr(0, 3);
			symbol->ProductCode(product_code);
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol->FullCode(value);
			value = line.substr(index, 30); index += 30;
			VtStringUtil::trim(value);
			symbol->SymbolNameKr(value);
			value = line.substr(index, 30); index += 30;
			VtStringUtil::trim(value);
			symbol->SymbolNameEn(value);
			value = line.substr(index, 5); index += 5;
			VtStringUtil::trim(value);
			symbol->RemainDays(_ttoi(value.c_str()));
			value = line.substr(index, 8); index += 8;
			VtStringUtil::trim(value);
			symbol->LastTradeDay(value);
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol->HighLimitPrice(value);
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol->LowLimitPrice(value);
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol->PreDayClose(value);
			std::string close_value = value;
			close_value.erase(std::remove(close_value.begin(), close_value.end(), '.'), close_value.end());
			//if (product_code != "101")
			quote_p->close = _ttoi(close_value.c_str());
			quote_p->pre_day_close = _ttoi(close_value.c_str());

			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			std::string pre_day_close = value;
			symbol->StandardPrice(value);
			value = line.substr(index, 17); index += 17;
			VtStringUtil::trim(value);
			symbol->Strike(value);
			value = line.substr(index, 1); index += 1;
			VtStringUtil::trim(value);
			symbol->AtmType(_ttoi(value.c_str()));
			value = line.substr(index, 1); index += 1;
			VtStringUtil::trim(value);
			symbol->RecentMonth(_ttoi(value.c_str()));
			value = line.substr(index, 8);
			symbol->ExpireDate(value);
			symbol->symbol_type(SymbolType::Domestic);
			symbol->StartTime("084500");
			symbol->EndTime("154500");
			symbol->Exchange("KRX");
			symbol->Currency("\\");
			set_product_info(symbol);
			set_quote_preday_close(symbol, pre_day_close);
			AddSymbol(symbol);
			//LOGINFO(CMyLogger::getInstance(), "read symbol %s complete!", symbol->SymbolCode().c_str());
			add_to_yearmonth(symbol);
		}

		LOGINFO(CMyLogger::getInstance(), "read %s file complete!", full_name.c_str());
	}
	catch (std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}

void SmSymbolManager::read_dm_masterfile_ya()
{
	try {
		std::string file_path;
		file_path = SmConfigManager::GetApplicationPath();
		file_path.append(_T("\\"));
		file_path.append(_T("table"));
		file_path.append(_T("\\"));
		std::string file_name = "master.tbl";
		//TRACE(file_name.c_str());
		std::string full_name = file_path + file_name;
		std::ifstream infile(full_name);
		std::string line;
		std::string value;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);
			int index = 0;
			std::string gubun = line.substr(index, 2); index += 2;

			auto found = ya_market_code_map_.find(gubun);
			if (found == ya_market_code_map_.end()) continue;

			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			std::string symbol_code = value;
			const std::string product_code = symbol_code.substr(0, 3);
			if (gubun == "45") {
				if (!(product_code == "167" || product_code == "175")) continue;
			}

			//if (symbol_code.substr(0, 1).at(0) == '1')
			//	symbol_code.append("000");

			std::shared_ptr<SmSymbol> symbol = std::make_shared<SmSymbol>(std::move(symbol_code));
			std::shared_ptr<SmQuote> quote_p = mainApp.QuoteMgr()->get_quote(value);
			const std::string market_name = symbol->SymbolCode().substr(0, 1).at(0) == '1' ? DmFutureMarketName : DmOptionMarketName;
			symbol->MarketName(market_name);
			//const std::string product_code = symbol->SymbolCode().substr(0, 3);
			symbol->ProductCode(product_code);
			value = line.substr(index, 12); index += 12;
			VtStringUtil::trim(value);
			symbol->FullCode(value);
			value = line.substr(index, 30); index += 30;
			VtStringUtil::trim(value);
			symbol->SymbolNameKr(value);
			
			std::vector<std::string> name_vec = VtStringUtil::split(value, " ", true);

			
			value = line.substr(index, 24); index += 24;
			VtStringUtil::trim(value);
			if (name_vec[1] != "F") { // Only for the option
				symbol->AtmType(_ttoi(value.c_str()));

				symbol->Strike(name_vec.back());
			}
			value = line.substr(index, 40);

			std::string standard_value = value.substr(0, 8);
			std::string highest_price = value.substr(8, 8);
			std::string lowest_price = value.substr(16, 8);
			symbol->gubun_code(gubun);
			symbol->symbol_type(SymbolType::Domestic);
			symbol->StartTime("084500");
			symbol->EndTime("154500");
			symbol->Exchange("KRX");
			symbol->Currency("\\");
			set_product_info(symbol);
			AddSymbol(symbol);
			//LOGINFO(CMyLogger::getInstance(), "read symbol [%s] gubun_code = [%s] complete!", symbol->SymbolCode().c_str(), gubun.c_str());
			add_to_yearmonth_dm_ya(symbol);
		}

		LOGINFO(CMyLogger::getInstance(), "read %s file complete!", full_name.c_str());
	}
	catch (std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error : %s", error.c_str());
	}
}

void SmSymbolManager::set_product_info(std::shared_ptr<SmSymbol> symbol)
{
	if (!symbol) return;
	auto product = find_product(symbol->MarketName(), symbol->ProductCode());
	if (!product) return;
	symbol->TickSize(_ttof(product->tick_size().c_str()));
	symbol->TickValue(product->tick_value());
	symbol->decimal(product->decimal());
	symbol->seung_su(product->seung_su());
}

void SmSymbolManager::set_quote_preday_close(std::shared_ptr<SmSymbol> symbol, const std::string& pre_day_str)
{
	if (!symbol || pre_day_str.empty()) return;
	auto quote = mainApp.QuoteMgr()->get_quote(symbol->SymbolCode());

	double converted_value = _ttof(pre_day_str.c_str());
	converted_value = converted_value * pow(10, symbol->decimal());

	quote->pre_day_close = static_cast<int>(converted_value);
}

void SmSymbolManager::AddDomesticSymbolCode(const std::string& product_code, const std::string& symbol_code)
{
	auto found = _DomesticSymbolCodeMap.find(product_code);
	if (found != _DomesticSymbolCodeMap.end()) {
		std::vector<std::string>& symbol_code_vec = found->second;
		symbol_code_vec.push_back(symbol_code);
	}
	else {
		std::vector<std::string> symbol_code_vec;
		symbol_code_vec.push_back(symbol_code);
		_DomesticSymbolCodeMap[product_code] = std::move(symbol_code_vec);
	}
}

void SmSymbolManager::MakeAbFavorite()
{
	for (auto it = _FavoriteProduct.begin(); it != _FavoriteProduct.end(); it++) {
		std::string product_code = *it;
		std::shared_ptr<SmProduct> product = FindProduct(product_code);
		if (!product) continue;
		const auto& symbol = GetRecentSymbol(product_code);
		if (symbol) {
			_FavoriteMap[symbol->Id()] = symbol;
			//mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
		}
	}
}

void SmSymbolManager::InitFavoriteProduct()
{
	std::string section = _T("FAVORITE");
	std::string favorite;
	std::string name = _T("SYMBOL");
	//favorite = mainApp.ConfigMgr().getString(section, name);
	if (favorite.length() == 0) {
		_FavoriteProduct.insert("HSI");
		_FavoriteProduct.insert("SFC");
		_FavoriteProduct.insert("MHI");
		_FavoriteProduct.insert("CES");
		
		//if (mainApp.is_simul()) {
			_FavoriteProduct.insert("CL");
			_FavoriteProduct.insert("MNQ");
			_FavoriteProduct.insert("CN");
			_FavoriteProduct.insert("GC");
			_FavoriteProduct.insert("NKD");
			_FavoriteProduct.insert("NQ");
		//}
	}
}

void SmSymbolManager::AddFavorite(const int& symbol_id)
{
	auto symbol = FindSymbolById(symbol_id);
	if (!symbol) return;
	_FavoriteMap[symbol_id] = symbol;
}

void SmSymbolManager::RegisterFavoriteSymbols()
{
	for (auto it = _FavoriteMap.begin(); it != _FavoriteMap.end(); it++) {
		RegisterSymbolToServer(it->second->SymbolCode(), true);
	}
}

void SmSymbolManager::RemoveFavorite(const std::string& symbol_code)
{
	auto symbol = FindSymbol(symbol_code);
	if (!symbol) return;
	auto found = _FavoriteMap.find(symbol->Id());
	if (found != _FavoriteMap.end()) {
		_FavoriteMap.erase(found);
	}
}

std::shared_ptr<SmSymbol> SmSymbolManager::FindSymbolById(const int& id)
{
	auto found = _SymbolIdMap.find(id);
	if (found != _SymbolIdMap.end())
		return found->second;
	else
		return nullptr;
}

void SmSymbolManager::AddSymbol(std::shared_ptr<SmSymbol> symbol)
{
	if (!symbol) return;
	SymbolMap_[symbol->SymbolCode()] = symbol;
	_SymbolIdMap[symbol->Id()] = symbol;
}

std::shared_ptr<SmSymbol> SmSymbolManager::FindSymbol(const std::string& symbol_code) const
{
	const auto it = SymbolMap_.find(symbol_code);
	if (it != SymbolMap_.end())
		return it->second;
	else
		return nullptr;
}

int SmSymbolManager::get_symbol_id(const std::string& symbol_code)
{
	auto found = SymbolMap_.find(symbol_code);
	if (found != SymbolMap_.end()) {
		return found->second->Id();
	}
	return -1;
}

void SmSymbolManager::GetRecentSymbolVector(std::vector<std::shared_ptr<SmSymbol>>& symbol_list)
{
	for (auto it = _MarketMap.begin(); it != _MarketMap.end(); ++it) {
		const auto market = it->second;
		const std::map<std::string, std::shared_ptr<SmProduct>>& product_map = market->GetProductMap();
		for (auto it2 = product_map.begin(); it2 != product_map.end(); ++it2) {
			const auto product = it2->second;
			const auto symbol = GetRecentSymbol(product->ProductCode());
			if (!symbol) continue;
			symbol_list.push_back(symbol);
		}
	}
}

std::shared_ptr<SmSymbol> SmSymbolManager::GetRecentSymbol(const std::string& product_code)
{
	std::shared_ptr<SmProduct> product = FindProduct(product_code);

	const std::vector<std::shared_ptr<SmSymbol>>& symbol_vec = product->GetSymbolVec();
	if (symbol_vec.empty()) return nullptr;
	int preday_volume = symbol_vec.front()->PreDayVolume();
	std::shared_ptr<SmSymbol> symbol = symbol_vec.front();
	for (size_t i = 0; i < symbol_vec.size(); i++) {
		if (symbol_vec[i]->PreDayVolume() > preday_volume) {
			symbol = symbol_vec[i];
			preday_volume = symbol_vec[i]->PreDayVolume();
		}
		// 차월물 중에 거래량이 제일 많은 것을 검색하지만 근월물로부터 4번째까지만 검사하고 더이상 검사하지 않는다. 
		if (i > 3) break;
	}
	return symbol;
}

std::string SmSymbolManager::GetRecentSymbolCode(const std::string& product_code)
{
	std::shared_ptr<SmProduct> product = FindProduct(product_code);

	const auto& symbol = GetRecentSymbol(product->ProductCode());
	if (symbol) return symbol->SymbolCode();
	return "";
}

std::string SmSymbolManager::GetNextSymbolCode(const std::string& product_code)
{
	std::string symbol_code("");

	std::shared_ptr<SmProduct> product = FindProduct(product_code);

	const std::vector<std::shared_ptr<SmSymbol>>& symbol_vec = product->GetSymbolVec();
	if (symbol_vec.empty() || symbol_vec.size() == 1) symbol_code;

	return (*(symbol_vec.begin() + 1))->SymbolCode();
}

std::shared_ptr<SmProduct> SmSymbolManager::FindProduct(const std::string& product_code)
{
	for (auto it = _MarketMap.begin(); it != _MarketMap.end(); ++it) {
		const auto found = it->second->FindProduct(product_code);
		if (found) return found;
	}

	return nullptr;
}

std::shared_ptr<SmMarket> SmSymbolManager::get_market(const std::string& market_name)
{
	auto market = FindMarket(market_name);
	if (market) return market;
	return AddMarket(market_name);
}

std::shared_ptr<SmMarket> SmSymbolManager::FindMarket(const std::string& market_name)
{
	auto found = _MarketMap.find(market_name);
	if (found == _MarketMap.end()) return nullptr;
	return found->second;
}

void SmSymbolManager::ReadAbroadSymbols() const noexcept
{
	mainApp.SymRdr()->ReadAbroadMarketFile();
	Sleep(500);
	mainApp.SymRdr()->read_abroad_symbol_file();
}

void SmSymbolManager::MakeDomesticMarket()
{
	DmFuture future;
	DmOption option;
	std::string market_name = DmFutureMarketName;
	std::shared_ptr<SmMarket> market = mainApp.SymMgr()->AddMarket(market_name);
	std::shared_ptr<SmProduct> product = market->AddProduct("101");
	future.product_code = "101F";
	future.future_name = "지수선물";
	future.product = product;
	_DomesticFutureVec.push_back(future);
	product->MarketName(market_name);
	product = market->AddProduct("105");
	future.product_code = "105F";
	future.future_name = "미니선물";
	future.product = product;
	_DomesticFutureVec.push_back(future);
	product->MarketName(market_name);
	product = market->AddProduct("106");
	future.product_code = "106F";
	future.future_name = "코닥선물";
	future.product = product;
	_DomesticFutureVec.push_back(future);
	product->MarketName(market_name);
	product = market->AddProduct("167");
	future.product_code = "167F";
	future.future_name = "국채선물";
	future.product = product;
	_DomesticFutureVec.push_back(future);
	product->MarketName(market_name);
	product = market->AddProduct("175");
	future.product_code = "175F";
	future.future_name = "달러선물";
	future.product = product;
	_DomesticFutureVec.push_back(future);
	product->MarketName(market_name);
	market_name = DmOptionMarketName;
	market = mainApp.SymMgr()->AddMarket(market_name);
	option.option_name = "코스피옵션";
	product = market->AddProduct("201");
	option.call_product = product;
	product->MarketName(market_name);
	product = market->AddProduct("301");
	option.put_product = product;
	product->MarketName(market_name);
	_DomesticOptionVec.push_back(option);

	option.option_name = "미니코스피옵션";
	product = market->AddProduct("205");
	option.call_product = product;
	product->MarketName(market_name);
	product = market->AddProduct("305");
	option.put_product = product;
	product->MarketName(market_name);
	_DomesticOptionVec.push_back(option);

	option.option_name = "코스피위클리옵션[T]";
	product = market->AddProduct("209");
	option.call_product = product;
	product->MarketName(market_name);
	product = market->AddProduct("309");
	option.put_product = product;
	product->MarketName(market_name);
	_DomesticOptionVec.push_back(option);

	option.option_name = "코스피위클리옵션[M]";
	product = market->AddProduct("2AF");
	option.call_product = product;
	product->MarketName(market_name);
	product = market->AddProduct("3AF");
	option.put_product = product;
	product->MarketName(market_name);

	_DomesticOptionVec.push_back(option);

	option.option_name = "코스닥옵션";
	product = market->AddProduct("206");
	option.call_product = product;
	product->MarketName(market_name);
	product = market->AddProduct("306");
	option.put_product = product;
	product->MarketName(market_name);
	_DomesticOptionVec.push_back(option);
}

void SmSymbolManager::RegisterSymbolToServer(const std::string& symbol_code, const bool& reg)
{
	if (reg) {
		auto found = _RegisteredSymbolMap.find(symbol_code);
		if (found != _RegisteredSymbolMap.end()) return;

		mainApp.Client()->RegisterSymbol(symbol_code);
		_RegisteredSymbolMap.insert(symbol_code);
	}
	else {
		auto found = _RegisteredSymbolMap.find(symbol_code);
		if (found == _RegisteredSymbolMap.end()) return;
		_RegisteredSymbolMap.erase(found);
		mainApp.Client()->UnregisterSymbol(symbol_code);
	}
}

std::shared_ptr<SmProduct> SmSymbolManager::find_product(
	const std::string& market_name,
	const std::string& product_code)
{
	std::shared_ptr<SmMarket> market = FindMarket(market_name);
	if (!market) return nullptr;
	return FindProduct(product_code);
}

std::string SmSymbolManager::get_first_year_month_name(const std::string& product_code)
{
	auto product = FindProduct(product_code);
	if (!product) return "";
	return product->get_first_year_month_name();
}

}
