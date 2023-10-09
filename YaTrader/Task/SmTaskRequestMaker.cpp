#include "stdafx.h"
#include "SmTaskRequestMaker.h"
#include "SmServerDataReceiver.h"
#include "../Json/json.hpp"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Account/SmAccountManager.h"
#include "../Account/SmAccount.h"
#include <vector>
#include <unordered_map>

using namespace DarkHorse;
using namespace nlohmann;

const int FileDownloadInterval = 500;
const int SymbolMasterInterval = 10;
const int SymbolQuoteInterval = 10;
const int SymbolHogaInterval = 10;
const int AccountAssetInterval = 700;
const int AccountProfitLossInterval = 700;
const int SymbolProfitLossInterval = 700;
const int AcceptedOrderInterval = 700;
const int FilledOrderInterval = 700;
const int MarketInterval = 700;
const int ChartDataInterval = 700;

std::shared_ptr<SmTaskInfo> SmTaskRequestMaker::MakeFileDownloadRequest() 
{
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::FileDownload;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::FileDownload];
	task_info->TimerInterval = FileDownloadInterval;
	task_info->Next = false;

	const int task_id = SmServerDataReceiver::GetId();
	task_arg req;
	req["task_id"] = task_id;
	req["range"] = "futures";
	req["requested"] = false;

	task_info->argMap[task_id] = std::move(req);
	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}


void DarkHorse::SmTaskRequestMaker::MaketInitialBatchTask(std::vector<std::shared_ptr<SmTaskInfo>>& task_list)
{
	//task_list.push_back(MakeTaskRequest(SmTaskType::FileDownload));
	//task_list.push_back(MakeTaskRequest(SmTaskType::DomesticSymbolCode));
	//task_list.push_back(MakeTaskRequest(SmTaskType::SymbolMaster));
	task_list.push_back(MakeTaskRequest(SmTaskType::SymbolQuote));
	//task_list.push_back(MakeTaskRequest(SmTaskType::SymbolHoga));
	//task_list.push_back(MakeTaskRequest(SmTaskType::AccountAsset));
	//task_list.push_back(MakeTaskRequest(SmTaskType::AccountProfitLoss));
	//task_list.push_back(MakeTaskRequest(SmTaskType::SymbolProfitLoss));
	task_list.push_back(MakeTaskRequest(SmTaskType::AcceptedOrderList));
	//task_list.push_back(MakeTaskRequest(SmTaskType::ChartData));
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeDomesticSymbolCodeRequest()
{
	std::vector<std::string> symbol_code_vec = mainApp.SymMgr()->GetDomesticProductVec();

	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::DomesticSymbolCode;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::DomesticSymbolCode];
	task_info->TimerInterval = SymbolMasterInterval;
	for (const auto symbol : symbol_code_vec) {
		const int task_id = SmServerDataReceiver::GetId();
		task_arg req;
		req["task_id"] = task_id;
		req["product_code"] = symbol;
		req["requested"] = false;
		task_info->argMap[task_id] = std::move(req);
	}

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeTaskRequest(const SmTaskType& task_type)
{
	switch (task_type) {
	case SmTaskType::DomesticSymbolCode: return MakeDomesticSymbolCodeRequest();
	case SmTaskType::FileDownload: return MakeFileDownloadRequest();
	case SmTaskType::Market: break;
	case SmTaskType::SymbolMaster: return MakeSymbolMasterRequest(); break;
	case SmTaskType::SymbolQuote: return MakeSymbolQuoteRequest();
	case SmTaskType::SymbolHoga: return MakeSymbolHogaRequest(); break;
	case SmTaskType::AccountAsset: return MakeAccountAssetRequest(); break;
	case SmTaskType::AccountProfitLoss: return MakeAccountProfitLossRequest(); break;
	case SmTaskType::SymbolProfitLoss: return MakeSymbolProfitLossRequest(); break;
	case SmTaskType::AcceptedOrderList: return MakeAcceptedOrderRequest(); break;
	case SmTaskType::ChartData: return MakeDefaultChartDataRequest(1500); break;
	case SmTaskType::FilledOrderList: return MakeFilledOrderRequest(); break;
	}
	return nullptr;
}

std::shared_ptr<DarkHorse::SmTaskInfo> SmTaskRequestMaker::MakeMasterFileDownloadRequest(const std::string& file_name)
{
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::MasterFileDownload;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::MasterFileDownload];
	task_info->TimerInterval = FileDownloadInterval;
	task_info->Next = false;

	const int task_id = SmServerDataReceiver::GetId();
	task_arg req;
	req["task_id"] = task_id;
	req["file_name"] = file_name;
	req["requested"] = false;

	task_info->argMap[task_id] = std::move(req);
	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeSymbolMasterRequest()
{
	std::vector < std::shared_ptr<SmSymbol>> symbol_list;
	mainApp.SymMgr()->GetRecentSymbolVector(symbol_list);

	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::SymbolMaster;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::SymbolMaster];
	task_info->TimerInterval = SymbolMasterInterval;
	for(const auto symbol : symbol_list) {
		const int task_id = SmServerDataReceiver::GetId();
		task_arg req;
		req["task_id"] = task_id;
		req["symbol_code"] = symbol->SymbolCode();
		req["symbol_name_kr"] = symbol->SymbolNameKr();
		req["requested"] = false;
		task_info->argMap[task_id] = std::move(req);
	}

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

task_arg DarkHorse::SmTaskRequestMaker::MakeSymbolMasterRequest(const std::string& symbol_code)
{
	const int task_id = SmServerDataReceiver::GetId();
	task_arg req;
	req["task_id"] = task_id;
	req["symbol_code"] = symbol_code;
	req["symbol_name_kr"] = "";
	req["requested"] = false;

	return req;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeSymbolQuoteRequest()
{
	std::vector < std::shared_ptr<SmSymbol>> symbol_list;
	mainApp.SymMgr()->GetRecentSymbolVector(symbol_list);

	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::SymbolQuote;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::SymbolQuote];
	task_info->TimerInterval = SymbolQuoteInterval;
	for (const auto symbol : symbol_list) {
		const int task_id = SmServerDataReceiver::GetId();
		task_arg req;
		req["task_id"] = task_id;
		req["symbol_code"] = symbol->SymbolCode();
		req["symbol_name_kr"] = symbol->SymbolNameKr();
		req["requested"] = false;
		task_info->argMap[task_id] = std::move(req);
	}

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

task_arg DarkHorse::SmTaskRequestMaker::MakeSymbolQuoteRequest(const std::string& symbol_code)
{
	const int task_id = SmServerDataReceiver::GetId();
	task_arg req;
	req["task_id"] = task_id;
	req["symbol_code"] = symbol_code;
	req["symbol_name_kr"] = "";
	req["requested"] = false;
	return (req);
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeSymbolHogaRequest()
{
	std::vector < std::shared_ptr<SmSymbol>> symbol_list;
	mainApp.SymMgr()->GetRecentSymbolVector(symbol_list);

	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::SymbolHoga;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::SymbolHoga];
	task_info->TimerInterval = SymbolHogaInterval;
	for (const auto symbol : symbol_list) {
		const int task_id = SmServerDataReceiver::GetId();
		task_arg req;
		req["task_id"] = task_id;
		req["symbol_code"] = symbol->SymbolCode();
		req["symbol_name_kr"] = symbol->SymbolNameKr();
		req["requested"] = false;
		task_info->argMap[task_id] = std::move(req);
	}

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

task_arg DarkHorse::SmTaskRequestMaker::MakeSymbolHogaRequest(const std::string& symbol_code)
{
	const int task_id = SmServerDataReceiver::GetId();
	task_arg req;
	req["task_id"] = task_id;
	req["symbol_code"] = symbol_code;
	req["symbol_name_kr"] = "";
	req["requested"] = false;
	return req;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeAccountAssetRequest()
{
	const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::AccountAsset;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::AccountAsset];
	task_info->TimerInterval = AccountAssetInterval;
	for (const auto account : account_map) {
		const int task_id = SmServerDataReceiver::GetId();
		task_arg req;
		req["task_id"] = task_id;
		req["account_no"] = account.second->No();
		req["account_name"] = account.second->Name();
		req["type"] = account.second->Type();
		req["password"] = account.second->Pwd();
		req["requested"] = false;
		task_info->argMap[task_id] = std::move(req);
	}

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

task_arg DarkHorse::SmTaskRequestMaker::MakeAccountAssetRequest(
	const std::string& account_no, 
	const std::string& pwd,
	const std::string& type)
{
	const int task_id = SmServerDataReceiver::GetId();
	task_arg req;
	req["task_id"] = task_id;
	req["account_no"] = account_no;
	req["account_name"] = "";
	req["password"] = pwd;
	req["requested"] = false;
	req["type"] = type;

	return req;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeAccountProfitLossRequest()
{
	const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::AccountProfitLoss;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::AccountProfitLoss];
	task_info->TimerInterval = AccountProfitLossInterval;
	for (const auto account : account_map) {
		const int task_id = SmServerDataReceiver::GetId();
		task_arg req;
		req["task_id"] = task_id;
		req["account_no"] = account.second->No();
		req["account_name"] = account.second->Name();
		req["type"] = account.second->Type();
		req["password"] = account.second->Pwd();
		req["requested"] = false;
		task_info->argMap[task_id] = std::move(req);
	}

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeSymbolProfitLossRequest()
{
	const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::SymbolProfitLoss;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::SymbolProfitLoss];
	task_info->TimerInterval = SymbolProfitLossInterval;
	for (const auto account : account_map) {
		const int task_id = SmServerDataReceiver::GetId();
		task_arg req;
		req["task_id"] = task_id;
		req["account_no"] = account.second->No();
		req["account_name"] = account.second->Name();
		req["password"] = account.second->Pwd();
		req["type"] = account.second->Type();
		req["requested"] = false;
		task_info->argMap[task_id] = std::move(req);
	}

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeAcceptedOrderRequest()
{
	const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::AcceptedOrderList;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::AcceptedOrderList];
	task_info->TimerInterval = AcceptedOrderInterval;
	for (const auto account : account_map) {
		const int task_id = SmServerDataReceiver::GetId();
		task_arg req;
		req["task_id"] = task_id;
		req["account_no"] = account.second->No();
		req["account_name"] = account.second->Name();
		req["password"] = account.second->Pwd();
		req["type"] = account.second->Type();
		if (account.second->Type() == "1")
			req["tr_code"] = "g11004.AQ0401%";
		else
			req["tr_code"] = "g11002.DQ0104&";
		req["requested"] = false;
		task_info->argMap[task_id] = std::move(req);
	}

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeFilledOrderRequest()
{
	const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& account_map = mainApp.AcntMgr()->GetAccountMap();
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::FilledOrderList;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::FilledOrderList];
	task_info->TimerInterval = FilledOrderInterval;
	for (const auto account : account_map) {
		const int task_id = SmServerDataReceiver::GetId();
		task_arg req;
		req["task_id"] = task_id;
		req["account_no"] = account.second->No();
		req["account_name"] = account.second->Name();
		req["password"] = account.second->Pwd();
		req["type"] = account.second->Type();
		req["requested"] = false;
		task_info->argMap[task_id] = std::move(req);
	}

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeChartDataRequest(const std::string& symbol_code, SmChartType chart_type, const int& cycle, const int& count)
{
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::ChartData;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::ChartData];
	task_info->TimerInterval = ChartDataInterval;
	const int task_id = SmServerDataReceiver::GetId();
	task_arg req;
	req["task_id"] = task_id;
	req["symbol_code"] = symbol_code;
	req["chart_type"] = static_cast<int>(chart_type);
	req["cycle"] = cycle;
	req["count"] = count;
	req["next"] = 0;
	req["requested"] = false;
	task_info->argMap[task_id] = std::move(req);

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeDefaultChartDataRequest(const int& count)
{
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::ChartData;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::ChartData];
	task_info->TimerInterval = ChartDataInterval;


	const std::map<int, std::shared_ptr<DarkHorse::SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	std::vector<std::string> symbol_code_vec;
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		symbol_code_vec.push_back(it->second->SymbolCode());
	}

	for (size_t i = 0; i < symbol_code_vec.size(); i++) {
		const std::string& symbol_code = symbol_code_vec[i];
		int task_id = SmServerDataReceiver::GetId();
		const std::string product_code = symbol_code.substr(0, 2);
		task_arg req;
		if (product_code == "NQ") {
			req["task_id"] = task_id;
			req["symbol_code"] = symbol_code;
			req["chart_type"] = static_cast<int>(SmChartType::MIN);
			req["cycle"] = 1;
			req["count"] = count;
			req["next"] = 0;
			req["requested"] = false;
			task_info->argMap[task_id] = req;

			task_id = SmServerDataReceiver::GetId();
			req["task_id"] = task_id;
			req["symbol_code"] = symbol_code;
			req["chart_type"] = static_cast<int>(SmChartType::MIN);
			req["cycle"] = 5;
			req["count"] = count;
			req["next"] = 0;
			req["requested"] = false;
			task_info->argMap[task_id] = req;

			task_id = SmServerDataReceiver::GetId();
			req["task_id"] = task_id;
			req["symbol_code"] = symbol_code;
			req["chart_type"] = static_cast<int>(SmChartType::MIN);
			req["cycle"] = 10;
			req["count"] = count;
			req["next"] = 0;
			req["requested"] = false;
			task_info->argMap[task_id] = req;

			task_id = SmServerDataReceiver::GetId();
			req["task_id"] = task_id;
			req["symbol_code"] = symbol_code;
			req["chart_type"] = static_cast<int>(SmChartType::MIN);
			req["cycle"] = 60;
			req["count"] = count;
			req["next"] = 0;
			req["requested"] = false;
			task_info->argMap[task_id] = req;

			task_id = SmServerDataReceiver::GetId();
			req["task_id"] = task_id;
			req["symbol_code"] = symbol_code;
			req["chart_type"] = static_cast<int>(SmChartType::MIN);
			req["cycle"] = 240;
			req["count"] = count;
			req["next"] = 0;
			req["requested"] = false;
			task_info->argMap[task_id] = req;

			task_id = SmServerDataReceiver::GetId();
			req["task_id"] = task_id;
			req["symbol_code"] = symbol_code;
			req["chart_type"] = static_cast<int>(SmChartType::TICK);
			req["cycle"] = 120;
			req["count"] = count;
			req["next"] = 0;
			req["requested"] = false;
			task_info->argMap[task_id] = req;

			task_id = SmServerDataReceiver::GetId();
			req["task_id"] = task_id;
			req["symbol_code"] = symbol_code;
			req["chart_type"] = static_cast<int>(SmChartType::TICK);
			req["cycle"] = 300;
			req["count"] = count;
			req["next"] = 0;
			req["requested"] = false;
			task_info->argMap[task_id] = req;

			task_id = SmServerDataReceiver::GetId();
			req["task_id"] = task_id;
			req["symbol_code"] = symbol_code;
			req["chart_type"] = static_cast<int>(SmChartType::TICK);
			req["cycle"] = 600;
			req["count"] = count;
			req["next"] = 0;
			req["requested"] = false;
			task_info->argMap[task_id] = req;

			task_id = SmServerDataReceiver::GetId();
			req["task_id"] = task_id;
			req["symbol_code"] = symbol_code;
			req["chart_type"] = static_cast<int>(SmChartType::TICK);
			req["cycle"] = 900;
			req["count"] = count;
			req["next"] = 0;
			req["requested"] = false;
			task_info->argMap[task_id] = req;

			task_id = SmServerDataReceiver::GetId();
			req["task_id"] = task_id;
			req["symbol_code"] = symbol_code;
			req["chart_type"] = static_cast<int>(SmChartType::DAY);
			req["cycle"] = 1;
			req["count"] = count;
			req["next"] = 0;
			req["requested"] = false;
			task_info->argMap[task_id] = req;
		}
		else {
			req["task_id"] = task_id;
			req["symbol_code"] = symbol_code;
			req["chart_type"] = static_cast<int>(SmChartType::MIN);
			req["cycle"] = 1;
			req["count"] = count;
			req["next"] = 0;
			req["requested"] = false;
			task_info->argMap[task_id] = std::move(req);
		}
	}

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeDefaultChartDataRequest(std::vector<std::shared_ptr<SmTaskInfo>>& task_list, const int& count)
{
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::ChartData;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::ChartData];
	task_info->TimerInterval = ChartDataInterval;


	const std::map<int, std::shared_ptr<DarkHorse::SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	std::vector<std::string> symbol_code_vec;
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		symbol_code_vec.push_back(it->second->SymbolCode());
	}

	for (size_t i = 0; i < symbol_code_vec.size(); i++) {
		const int task_id = SmServerDataReceiver::GetId();
		task_arg req;
		req["task_id"] = task_id;
		req["symbol_code"] = symbol_code_vec[i];
		req["chart_type"] = static_cast<int>(SmChartType::MIN);
		req["cycle"] = 1;
		req["count"] = count;
		req["next"] = 0;
		req["prev"] = 0;
		req["requested"] = false;
		task_info->argMap[task_id] = std::move(req);
	}

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	task_list.push_back(task_info);

	return task_info;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeSymbolRegisterRequest(const std::string& symbol_code, const bool& reg)
{
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::RegisterSymbol;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::RegisterSymbol];
	task_info->TimerInterval = ChartDataInterval;
	const int task_id = SmServerDataReceiver::GetId();
	task_arg req;
	req["task_id"] = task_id;
	req["symbol_code"] = symbol_code;
// 	req["chart_type"] = static_cast<int>(chart_type);
// 	req["cycle"] = cycle;
// 	req["count"] = count;
	req["next"] = 0;
	req["requested"] = false;
	task_info->argMap[task_id] = std::move(req);

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}

std::shared_ptr<DarkHorse::SmTaskInfo> DarkHorse::SmTaskRequestMaker::MakeAccountRegisterRequest(const std::string& account_no, const bool& reg)
{
	std::shared_ptr<SmTaskInfo> task_info = std::make_shared<SmTaskInfo>();
	task_info->TaskType = SmTaskType::RegisterAccount;
	task_info->TaskTitle = SmTaskTitleMap[SmTaskType::RegisterAccount];
	task_info->TimerInterval = ChartDataInterval;
	const int task_id = SmServerDataReceiver::GetId();
	task_arg req;
	req["task_id"] = task_id;
// 	req["symbol_code"] = symbol_code;
// 	req["chart_type"] = static_cast<int>(chart_type);
// 	req["cycle"] = cycle;
// 	req["count"] = count;
	req["next"] = 0;
	req["requested"] = false;
	task_info->argMap[task_id] = std::move(req);

	task_info->TotalTaskCount = task_info->argMap.size();
	task_info->RemainTaskCount = task_info->argMap.size();

	return task_info;
}
