#include "stdafx.h"
#include "ViStockClient.h"
#include "../Json/json.hpp"
#include "ViClient.h"
#include "../resource.h"
#include "../Log/MyLogger.h"
#include "../Global/SmTotalManager.h"
#include "../Login/SmLoginManager.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Order/SmOrder.h"
#include "../Quote/SmQuoteManager.h"
#include "../Hoga/SmHogaManager.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Order/SmOrderRequest.h"
#include "../Global/SmTotalManager.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Position/SmAccountPositionManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Event/SmCallbackManager.h"
#include "../Order/SmAccountOrderManager.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../Position/TotalPositionManager.h"
#include "../Position/Position.h"


using namespace nlohmann;
using namespace DarkHorse;

int ViStockClient::ConnectToServer()
{
	if (!_ViCtrol)
	{
		LOGINFO(CMyLogger::getInstance(), "증권사 컨트롤이 만들어 지지 않았습니다.");
		return -1;
	}
	
	if (_ViCtrol->connect_to_server() < 0) {
		LOGINFO(CMyLogger::getInstance(), "증권사와 통신이 되지 않습니다.");
		return -1;
	}

	LOGINFO(CMyLogger::getInstance(), "증권사 접속 성공");
	return 1;
}

int ViStockClient::Login(task_arg&& login_info)
{
	return _ViCtrol->Login(std::move(login_info));
}

int ViStockClient::GetSymbolMaster(task_arg&& arg)
{
	return _ViCtrol->ab_symbol_master(std::move(arg));
}

int ViStockClient::GetSymbolQuote(task_arg&& arg)
{
	return _ViCtrol->ab_symbol_quote(std::move(arg));
}

int ViStockClient::GetSymbolHoga(task_arg&& arg)
{
	return _ViCtrol->ab_symbol_hoga(std::move(arg));
}

void ViStockClient::UnregisterAccount(task_arg&& arg)
{
	_ViCtrol->unregister_account(std::move(arg));
}

void ViStockClient::RegisterAccount(task_arg&& arg)
{
	_ViCtrol->register_account(std::move(arg));
}

void ViStockClient::UnregisterSymbol(task_arg&& arg)
{
	_ViCtrol->unregister_symbol(std::move(arg));
}

void ViStockClient::RegisterSymbol(task_arg&& arg)
{
	_ViCtrol->register_symbol(std::move(arg));
}

void ViStockClient::CancelOrder(task_arg&& arg)
{
	_ViCtrol->ab_cancel_order(std::move(arg));
}

void ViStockClient::ChangeOrder(task_arg&& arg)
{
	_ViCtrol->ab_change_order(std::move(arg));
}

void ViStockClient::NewOrder(task_arg&& arg)
{
	_ViCtrol->ab_new_order(std::move(arg));
}

int ViStockClient::GetChartData(task_arg&& arg)
{
	return _ViCtrol->ab_chart_data(std::move(arg));
}

int ViStockClient::GetFilledOrderList(task_arg&& arg)
{
	return _ViCtrol->ab_filled_order_list(std::move(arg));
}

int ViStockClient::GetAcceptedOrderList(task_arg&& arg)
{
	return 1; // _ViCtrol->ab_accepted_order_list(std::move(arg));
}

int ViStockClient::GetSymbolProfitLoss(task_arg&& arg)
{
	return _ViCtrol->ab_symbol_profit_loss(std::move(arg));
}

int ViStockClient::GetAccountProfitLoss(task_arg&& arg)
{
	return _ViCtrol->ab_account_profit_loss(std::move(arg));
}

int ViStockClient::GetDeposit(task_arg&& arg)
{
	return _ViCtrol->ab_account_deposit(std::move(arg));
}

int ViStockClient::GetAccountAsset(task_arg&& arg)
{
	return _ViCtrol->ab_account_asset(std::move(arg));
}



int DarkHorse::ViStockClient::DownloadFile(task_arg&& arg)
{
	return _ViCtrol->download_file(std::move(arg));
}



int ViStockClient::DownloadMasterFile(task_arg&& arg)
{
	return _ViCtrol->download_symbol_master_file(std::move(arg));
}

int DarkHorse::ViStockClient::GetFilledOrderList(const std::string& account_no, const std::string& pwd)
{
	return _ViCtrol->ab_filled_order_list(account_no, pwd);
}

int ViStockClient::ab_symbol_master(DhTaskArg arg)
{
	return _ViCtrol->ab_symbol_master(arg);
}

int ViStockClient::ab_account_profit_loss(DhTaskArg arg)
{
	return _ViCtrol->ab_account_profit_loss(arg);
}

int ViStockClient::dm_account_profit_loss(DhTaskArg arg)
{
	return _ViCtrol->dm_account_profit_loss(arg);
}


int ViStockClient::ab_symbol_profit_loss(DhTaskArg arg)
{
	return _ViCtrol->ab_symbol_profit_loss(arg);
}

int ViStockClient::dm_symbol_profit_loss(DhTaskArg arg)
{
	return _ViCtrol->dm_symbol_profit_loss(arg);
}

int ViStockClient::ab_symbol_master_file_download(DhTaskArg arg)
{
	return _ViCtrol->ab_symbol_master_file_download(arg);
}

int ViStockClient::dm_symbol_master_file_download(DhTaskArg arg)
{
	return _ViCtrol->dm_symbol_master_file_download(arg);
}

void ViStockClient::ChangeOrder(order_request_p order_req)
{
	_ViCtrol->change_order(order_req);
}

void ViStockClient::NewOrder(order_request_p order_req)
{
	_ViCtrol->new_order(order_req);
}

void ViStockClient::CancelOrder(order_request_p order_req)
{
	_ViCtrol->cancel_order(order_req);
}

void ViStockClient::dm_new_order(order_request_p order_req)
{
	_ViCtrol->new_order(order_req);
}

void ViStockClient::dm_change_order(order_request_p order_req)
{
	_ViCtrol->change_order(order_req);
}

void ViStockClient::dm_cancel_order(order_request_p order_req)
{
	_ViCtrol->cancel_order(order_req);
}

void DarkHorse::ViStockClient::NewOrder(const std::shared_ptr<SmOrderRequest>& order_req)
{
	if (_TestMode) SendOrderAcceptedData(order_req);
	else {
		_OrderReqMap[order_req->RequestId] = order_req;
		_ViCtrol->ab_new_order(order_req);
	}
}

void DarkHorse::ViStockClient::ChangeOrder(const std::shared_ptr<SmOrderRequest>& order_req)
{
	if (_TestMode) SendOrderAcceptedData(order_req);
	else {
		_OrderReqMap[order_req->RequestId] = order_req;
		_ViCtrol->ab_change_order(order_req);
	}
}

void DarkHorse::ViStockClient::CancelOrder(const std::shared_ptr<SmOrderRequest>& order_req)
{
	if (_TestMode) SendOrderAcceptedData(order_req);
	else {
		_OrderReqMap[order_req->RequestId] = order_req;
		_ViCtrol->ab_cancel_order(order_req);
	}
}

void DarkHorse::ViStockClient::RegisterSymbol(const std::string& symbol_code)
{
	_ViCtrol->register_symbol(symbol_code);
}

void DarkHorse::ViStockClient::UnregisterSymbol(const std::string& symbol_code)
{
	_ViCtrol->unregister_symbol(symbol_code);
}

void DarkHorse::ViStockClient::RegisterAccount(const std::string& account_no)
{
	_ViCtrol->register_account(account_no);
}

void DarkHorse::ViStockClient::SendOrderAcceptedData(const std::shared_ptr<SmOrderRequest>& order_req)
{
	nlohmann::json order_info;
	order_info["account_no"] = order_req->AccountNo;
	order_info["order_no"] = std::to_string(order_req->RequestId);
	order_info["symbol_code"] = order_req->SymbolCode;
	order_info["order_price"] = order_req->OrderPrice;
	order_info["order_amount"] = order_req->OrderAmount;
	order_info["order_position"] = order_req->PositionType == SmPositionType::Buy ? "1" : "2";
	order_info["ori_order_no"] = "";
	order_info["first_order_no"] = "";
	order_info["order_time"] = "";
	order_info["order_type"] = "1"; // Order Type New
	order_info["custom_info"] = "";

	OnOrderAccepted(std::move(order_info));
}

void DarkHorse::ViStockClient::SendOrderUnfilledData()
{
	nlohmann::json order_info;
	order_info["account_no"] = "";
	order_info["order_no"] = "";
	order_info["symbol_code"] = "";
	order_info["order_price"] = 0;
	order_info["order_amount"] = 0;
	order_info["order_position"] = "";
	order_info["ori_order_no"] = "";
	order_info["first_order_no"] = "";
	order_info["order_type"] = "";
	order_info["remain_count"] = 0;
	order_info["canceled_count"] = 0;
	order_info["modified_count"] = 0;
	order_info["filled_count"] = 0;

	order_info["custom_info"] = "";
}

void DarkHorse::ViStockClient::SendOrderFilledData()
{
	nlohmann::json order_info;
	order_info["account_no"] = "";
	order_info["order_no"] = "";
	order_info["symbol_code"] = "";
	order_info["order_price"] = 0;
	order_info["order_amount"] = 0;
	order_info["order_position"] = "";
	
	order_info["filled_price"] = 0;
	order_info["filled_count"] = 0;

	order_info["filled_time"] = "";

	order_info["custom_info"] = "";
}

int DarkHorse::ViStockClient::CheckAccountPassword(task_arg&& arg)
{
	return _ViCtrol->check_account_password(std::move(arg));
}

int DarkHorse::ViStockClient::CheckDmAccountPassword(task_arg&& arg)
{
	return _ViCtrol->dm_check_account_password(std::move(arg));
}

void DarkHorse::ViStockClient::Enable(bool val)
{
	_ViCtrol->Enable(val);
}

void DarkHorse::ViStockClient::UnRegAll()
{
	_ViCtrol->unregister_all_symbols();
	_ViCtrol->unregister_all_accounts();
}

DarkHorse::ViStockClient::~ViStockClient()
{

}

void DarkHorse::ViStockClient::CreateControl()
{
	 _ViCtrol = std::make_shared<ViClient>();
	 _ViCtrol->Create(IDD_VI_CLIENT);
	 _ViCtrol->ShowWindow(SW_HIDE);
	 _ViCtrol->Client(weak_from_this());
}

void DarkHorse::ViStockClient::ExecTask(const SmTaskType& task_type, task_arg&& task)
{
	switch (task_type)
	{
	case SmTaskType::MasterFileDownload:
		DownloadMasterFile(std::move(task));
		break;
	case SmTaskType::FileDownload:
		DownloadFile(std::move(task));
		break;
	case SmTaskType::Market:
		break;
	case SmTaskType::SymbolCode:
		break;
	case SmTaskType::SymbolMaster:
		GetSymbolMaster(std::move(task));
		break;
	case SmTaskType::AccountAsset:
		GetAccountAsset(std::move(task));
		break;
	case SmTaskType::AccountProfitLoss:
		GetAccountProfitLoss(std::move(task));
		break;
	case SmTaskType::SymbolProfitLoss:
		GetSymbolProfitLoss(std::move(task));
		break;
	case SmTaskType::SymbolQuote:
		GetSymbolQuote(std::move(task));
		break;
	case SmTaskType::SymbolHoga:
		GetSymbolHoga(std::move(task));
		break;
	case SmTaskType::ChartData:
		GetChartData(std::move(task));
		break;
	case SmTaskType::AcceptedOrderList:
		GetAcceptedOrderList(std::move(task));
		break;
	case SmTaskType::FilledOrderList:
		GetFilledOrderList(std::move(task));
		break;
	case SmTaskType::SymbolPosition:
		break;
	case SmTaskType::RegisterSymbol:
		RegisterSymbol(std::move(task));
		break;
	case SmTaskType::RegisterAccount:
		RegisterAccount(std::move(task));
		break;
	case SmTaskType::DomesticSymbolCode:
		GetDomesticSymbolCode(std::move(task));
		break;
	}
}

void DarkHorse::ViStockClient::ExecTask(DhTaskArg&& task)
{
	switch (task.task_type)
	{
	case DhTaskType::AccountProfitLoss:
		_ViCtrol->account_profit_loss(std::move(task));
		break;
	case DhTaskType::DmSymbolMaster:
		_ViCtrol->dm_symbol_master(std::move(task));
		break;
	case DhTaskType::SymbolChartData:
		_ViCtrol->chart_data(std::move(task));
		break;
	case DhTaskType::RegisterSymbol:
		_ViCtrol->register_symbol(std::move(task));
		break;
	case DhTaskType::AbSymbolQuote:
		_ViCtrol->ab_symbol_quote(std::move(task));
		break;
	case DhTaskType::AbSymbolHoga:
		_ViCtrol->ab_symbol_hoga(std::move(task));
		break;
	}
}

void DarkHorse::ViStockClient::RemoveTask(const int& task_id)
{
	_ViCtrol->remove_task(task_id);
}

void DarkHorse::ViStockClient::OnAccountReceived(nlohmann::json&& arg)
{
	try {
		std::shared_ptr<SmAccount> account = std::make_shared<SmAccount>();
		const std::string account_type = arg["account_type"];
		if (account_type.compare("1") == 0 || 
			account_type.compare("9") == 0) {
			account->No(arg["account_no"]);
			account->Name(arg["account_name"]);
			account->Type(account_type);
			account->is_server_side(true);
			//account->make_default_sub_account();
			mainApp.AcntMgr()->AddAccount(account);
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());

	}
}

void DarkHorse::ViStockClient::OnSymbolMaster(nlohmann::json&& arg)
{
	try {
		const std::string symbol_code = arg["symbol_code"];
		auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
		if (!symbol) return;
		symbol->ExpireDate(arg["expire_day"]);
		symbol->StartTime(arg["local_start_time"]);
		symbol->EndTime(arg["local_end_time"]);
		symbol->Deposit(arg["deposit"]);
		symbol->Currency(arg["currency"]);
		symbol->Exchange(arg["exchange"]);

		//mainApp.CallbackMgr()->OnSymbolMasterEvent(symbol->Id());
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());

	}
}

void DarkHorse::ViStockClient::OnSymbolQuote(nlohmann::json&& quote)
{
	mainApp.QuoteMgr()->AddQuote(std::move(quote));
}

void DarkHorse::ViStockClient::OnDmSymbolMaster(const std::string& symbol_code)
{
	std::shared_ptr<SmSymbol> symbol_p = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (symbol_p) symbol_p->Master_requested(true);
	mainApp.event_hub()->process_symbol_master_event(symbol_p);
}

void DarkHorse::ViStockClient::on_dm_expected(nlohmann::json&& quote)
{
	const std::string& symbol_code = quote["symbol_code"];
	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (!symbol) return;
	std::shared_ptr<SmQuote> quote_p = mainApp.QuoteMgr()->get_quote(symbol_code);
	quote_p->symbol_code = symbol->SymbolCode();
	quote_p->symbol_id = symbol->Id();
	quote_p->expected = quote["expected"];
	mainApp.event_hub()->process_expected_event(quote_p);
}

void DarkHorse::ViStockClient::on_dm_commodity_future_quote(nlohmann::json&& quote)
{
	mainApp.QuoteMgr()->AddQuote(std::move(quote));
}
void DarkHorse::ViStockClient::on_dm_commodity_future_hoga(nlohmann::json&& hoga)
{
	mainApp.HogaMgr()->AddHoga(std::move(hoga));
}

void DarkHorse::ViStockClient::on_dm_option_quote(nlohmann::json&& quote)
{
	mainApp.QuoteMgr()->AddQuote(std::move(quote));
}
void DarkHorse::ViStockClient::on_dm_option_hoga(nlohmann::json&& hoga)
{
	mainApp.HogaMgr()->AddHoga(std::move(hoga));
}

void DarkHorse::ViStockClient::OnDmSymbolQuote(nlohmann::json&& quote)
{
	mainApp.QuoteMgr()->AddQuote(std::move(quote));
}

void DarkHorse::ViStockClient::OnSymbolHoga(nlohmann::json&& hoga)
{
	mainApp.HogaMgr()->AddHoga(std::move(hoga));
}


void DarkHorse::ViStockClient::OnDmSymbolHoga(nlohmann::json&& hoga)
{
	mainApp.HogaMgr()->AddHoga(std::move(hoga));
}

void DarkHorse::ViStockClient::OnRealtimeQuote(nlohmann::json&& arg)
{
	mainApp.QuoteMgr()->AddQuote(std::move(arg));
}

void DarkHorse::ViStockClient::OnRealtimeHoga(nlohmann::json&& arg)
{
	mainApp.HogaMgr()->AddHoga(std::move(arg));
}

void DarkHorse::ViStockClient::on_ab_account_asset(nlohmann::json&& arg)
{
	const std::string account_no = arg["account_no"];
	const auto account = mainApp.AcntMgr()->FindAccount(account_no);
	if (!account) return;

	try {
		account->Asset.EntrustTotal = arg["entrust_total"];
		account->Asset.Balance = arg["balance"];
		account->Asset.OutstandingDeposit = arg["outstanding_deposit"];
		account->Asset.OrderDeposit = arg["order_deposit"];
		account->Asset.EntrustDeposit = arg["entrust_deposit"];
		account->Asset.MaintenanceMargin = arg["maintenance_margin"];
		account->Asset.TradeProfitLoss = arg["settled_profit_loss"];
		account->Asset.Fee = arg["fee"];
		account->Asset.OpenProfitLoss = arg["open_profit_loss"];
		account->Asset.OpenTrustTotal = arg["open_trust_total"];
		account->Asset.AdditionalMargin = arg["additional_margin"];
		account->Asset.OrderMargin = arg["order_margin"];
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}


void DarkHorse::ViStockClient::on_dm_account_asset(nlohmann::json&& arg)
{
	const std::string account_no = arg["account_no"];
	const auto account = mainApp.AcntMgr()->FindAccount(account_no);
	if (!account) return;

	try {
		account->Asset.EntrustTotal = arg["balance"];
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void DarkHorse::ViStockClient::on_ab_symbol_profit_loss(nlohmann::json&& arg)
{
	try {
		const std::string account_no = arg["account_no"];
		const std::string symbol_code = arg["symbol_code"];
		auto position = mainApp.total_position_manager()->get_position(account_no, symbol_code);
		if (position) {
			position->trade_profit_loss = arg["trade_profit_loss"];
			//position->pure_trade_profit_loss = arg["pure_trade_profit_loss"];
			position->trade_fee = arg["trade_fee"];
			position->open_profit_loss = arg["open_profit_loss"];
			//position-> = arg["unsettled_fee"];
			//position-> = arg["pure_unsettled_profit_loss"];
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}


void DarkHorse::ViStockClient::on_symbol_profit_loss(nlohmann::json&& arg)
{
	try {
		const std::string& account_no = arg["account_no"];
		const std::string& symbol_code = arg["symbol_code"];
		auto position = mainApp.total_position_manager()->get_position(account_no, symbol_code);
		if (position) {
			position->trade_profit_loss = arg["trade_profit_loss"];
			//position->pure_trade_profit_loss = arg["pure_trade_profit_loss"];
			position->trade_fee = arg["trade_fee"];
			position->open_profit_loss = arg["open_profit_loss"];
			//position-> = arg["unsettled_fee"];
			//position-> = arg["pure_unsettled_profit_loss"];
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}


void DarkHorse::ViStockClient::on_dm_symbol_profit_loss(nlohmann::json&& arg)
{
	try {
		const std::string account_no = arg["account_no"];
		const std::string symbol_code = arg["symbol_code"];
		auto position = mainApp.total_position_manager()->get_position(account_no, symbol_code);
		if (position) {
			position->trade_profit_loss = arg["trade_profit_loss"];
			//position->pure_trade_profit_loss = arg["pure_trade_profit_loss"];
			position->trade_fee = arg["trade_fee"];
			position->open_profit_loss = arg["open_profit_loss"];
			//position-> = arg["unsettled_fee"];
			//position-> = arg["pure_unsettled_profit_loss"];
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void DarkHorse::ViStockClient::on_ab_symbol_position(nlohmann::json&& arg)
{
	try {
		const std::string symbol_code = arg["symbol_code"];
		const std::string account_no = arg["account_no"];
		const int average_price = arg["symbol_avg_price"];
		const int order_position = arg["symbol_position"]; // buy : 1, sell : -1
		const int pre_open_qty = arg["symbol_pre_open_qty"];
		const int today_open_qty = arg["symbol_open_qty"];
		const int open_quantity = pre_open_qty + today_open_qty;
		const double open_profit_loss = arg["symbol_open_profit_loss"];

		
		std::shared_ptr<DarkHorse::Position> position = mainApp.total_position_manager()->get_position(account_no, symbol_code);
		position->account_no = account_no;
		position->symbol_code = symbol_code;
		position->average_price = average_price;
		position->open_quantity = open_quantity * order_position;
		position->open_profit_loss = open_profit_loss;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void DarkHorse::ViStockClient::OnAcceptedList(nlohmann::json&& arg)
{
	OnOrderAccepted(std::move(arg));
}

void DarkHorse::ViStockClient::on_ab_filled_order_list(nlohmann::json&& arg)
{
	OnOrderFilledList(std::move(arg));
}

int DarkHorse::ViStockClient::GetDomesticSymbolCode(task_arg&& arg)
{
	return _ViCtrol->dm_symbol_code(std::move(arg));
}

void DarkHorse::ViStockClient::SetOrderAuxInfo(const std::shared_ptr<DarkHorse::SmOrder>& order)
{
	const std::string custom_info = order->Custom;
	if (custom_info.length() == 0) return;
	try {
		const int zero_pos = custom_info.find_first_of('0');
		const int req_pos = custom_info.find_first_of('k');
		std::string req_string = custom_info.substr(zero_pos, req_pos - zero_pos);
		const int req_id = std::stoi(req_string);
		auto found = _OrderReqMap.find(req_id);
		if (found == _OrderReqMap.end()) return;

		order->OrderReqId = found->second->RequestId;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void DarkHorse::ViStockClient::SendOrderEvent(const std::shared_ptr<SmOrder>& order)
{
	auto account = mainApp.AcntMgr()->FindAccount(order->AccountNo);
	auto symbol = mainApp.SymMgr()->FindSymbol(order->SymbolCode);
	if (!account || !symbol) return;
	//mainApp.CallbackMgr()->OnWndOrderEvent(account->Id(), symbol->Id());
	//mainApp.CallbackMgr()->OnOrderUpdateEvent(account->Id(), symbol->Id());

	auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), symbol->SymbolCode());
	
	symbol_order_mgr->UpdateOrder();
	mainApp.CallbackMgr()->OnOrderEvent(order->AccountNo, order->SymbolCode);
}

std::shared_ptr<SmOrder> DarkHorse::ViStockClient::SetOrderAccepted(nlohmann::json&& arg)
{
	try {
		const std::string order_no = arg["order_no"];
		std::shared_ptr<SmOrder> order = mainApp.TotalOrderMgr()->FindAddOrder(order_no);
		order->OrderNo = order_no;
		order->AccountNo = arg["account_no"];
		order->SymbolCode = arg["symbol_code"];
		order->OrderPrice = arg["order_price"];
		order->OrderAmount = arg["order_amount"];
		order->OriOrderNo = arg["ori_order_no"];
		order->FirstOrderNo = arg["first_order_no"];
		order->OrderTime = arg["order_time"];
		order->Custom = arg["custom"];

		// 주문 유형 - 매수 / 매도
		const std::string order_position = arg["order_position"];
		if (order_position.compare("1") == 0) order->PositionType = SmPositionType::Buy;
		else if (order_position.compare("2") == 0) order->PositionType = SmPositionType::Sell;

		// 주문 타입 : 신규/정정/취소
		const std::string order_type = arg["order_type"];
		if (order_type.compare("1") == 0) order->OrderType = SmOrderType::New;
		else if (order_type.compare("2") == 0) order->OrderType = SmOrderType::Modify;
		else if (order_type.compare("3") == 0) order->OrderType = SmOrderType::Cancel;

		return order;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		return nullptr;
	}
}

void DarkHorse::ViStockClient::OnOrderAccepted(nlohmann::json&& arg)
{
	std::shared_ptr<SmOrder> order = SetOrderAccepted(std::move(arg));

	if (!order) return;

	auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(order->AccountNo, order->SymbolCode);

	SetOrderAuxInfo(order);
	symbol_order_mgr->OnAcceptedOrder(order);

	SendOrderEvent(order);
}

std::shared_ptr<SmOrder> DarkHorse::ViStockClient::SetOrderUnfilled(nlohmann::json&& arg)
{
	try {
		const std::string order_no = arg["order_no"];
		std::shared_ptr<SmOrder> order = mainApp.TotalOrderMgr()->FindAddOrder(order_no);
		order->OrderNo = order_no;
		order->AccountNo = arg["account_no"];
		order->SymbolCode = arg["symbol_code"];
		order->OrderPrice = arg["order_price"];
		order->OrderAmount = arg["order_amount"];
		order->OriOrderNo = arg["ori_order_no"];
		order->FirstOrderNo = arg["first_order_no"];
		//order->OrderTime = arg["order_time"];

		order->RemainCount = arg["remain_count"];
		order->CanceledCount = arg["canceled_count"];
		order->ModifiedCount = arg["modified_count"];
		order->FilledCount = arg["filled_count"];
		order->OrderSequence = arg["order_sequence"];
		order->Custom = arg["custom"];

		// 주문 유형 - 매수 / 매도
		const std::string order_position = arg["order_position"];
		if (order_position.compare("1") == 0) order->PositionType = SmPositionType::Buy;
		else if (order_position.compare("2") == 0) order->PositionType = SmPositionType::Sell;

		/*
		// 주문 타입 : 신규/정정/취소
		const std::string order_type = arg["order_type"];
		if (order_type.compare("1") == 0) order->OrderType = SmOrderType::New;
		else if (order_type.compare("2") == 0) order->OrderType = SmOrderType::Modify;
		else if (order_type.compare("3") == 0) order->OrderType = SmOrderType::Cancel;
		*/
		return order;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		return nullptr;
	}
}


void DarkHorse::ViStockClient::OnOrderUnfilled(nlohmann::json&& arg)
{
	std::shared_ptr<SmOrder> order = SetOrderUnfilled(std::move(arg));

	if (!order) return;

	auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(order->AccountNo, order->SymbolCode);

	SetOrderAuxInfo(order);
	symbol_order_mgr->OnUnfilledOrder(order);

	SendOrderEvent(order);
}

void DarkHorse::ViStockClient::OnOrderFilledList(nlohmann::json&& arg)
{
	std::shared_ptr<SmOrder> order = SetOrderFilled(std::move(arg));

	if (!order) return;

	auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(order->AccountNo, order->SymbolCode);

	symbol_order_mgr->AddHistoryOrder(order);
}

std::shared_ptr<SmOrder> DarkHorse::ViStockClient::SetOrderFilled(nlohmann::json&& arg)
{
	try {
		const std::string order_no = arg["order_no"];
		std::shared_ptr<SmOrder> order = mainApp.TotalOrderMgr()->FindAddOrder(order_no);
		order->OrderNo = order_no;
		order->AccountNo = arg["account_no"];
		order->SymbolCode = arg["symbol_code"];
		order->OrderPrice = arg["order_price"];
		order->OrderAmount = arg["order_amount"];
		//order->OriOrderNo = arg["ori_order_no"];
		//order->FirstOrderNo = arg["first_order_no"];
		//order->OrderTime = arg["order_time"];
		order->FilledPrice = arg["filled_price"];
		order->FilledCount = arg["filled_count"];
		order->FilledTime = arg["filled_time"];
		// 부분 체결 문제로 주석 처리
		//order->UnsettledQty = order->FilledCount;

		order->Custom = arg["custom"];

		// 주문 유형 - 매수 / 매도
		const std::string order_position = arg["order_position"];
		if (order_position.compare("1") == 0) order->PositionType = SmPositionType::Buy;
		else if (order_position.compare("2") == 0) {
			//order->FilledCount *= -1;
			//order->UnsettledQty *= -1;
			order->PositionType = SmPositionType::Sell;
		}

		return order;

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		return nullptr;
	}
}


void DarkHorse::ViStockClient::OnOrderFilled(nlohmann::json&& arg)
{
	std::shared_ptr<SmOrder> order = SetOrderFilled(std::move(arg));

	if (!order) return;

	auto account_order_mgr = mainApp.TotalOrderMgr()->FindAccountOrderManager(order->AccountNo);
	if (account_order_mgr && order->FilledCount != 0) { account_order_mgr->AddFilledInfo(order); }

	auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(order->AccountNo, order->SymbolCode);

	SetOrderAuxInfo(order);

	symbol_order_mgr->OnFilledOrder(order);

	SendOrderEvent(order);
}

int ViStockClient::dm_symbol_master(DhTaskArg arg)
{
	return _ViCtrol->dm_symbol_master(arg);	
}

int DarkHorse::ViStockClient::ab_account_asset(DhTaskArg arg)
{
	return _ViCtrol->ab_account_asset(arg);
}
int DarkHorse::ViStockClient::dm_account_asset(DhTaskArg arg)
{
	return _ViCtrol->dm_account_asset(arg);
}

int ViStockClient::dm_symbol_position(DhTaskArg arg)
{
	return _ViCtrol->dm_symbol_position(arg);
}

int ViStockClient::ab_symbol_position(DhTaskArg arg)
{
	return _ViCtrol->ab_symbol_position(arg);
}

int DarkHorse::ViStockClient::ab_accepted_order(DhTaskArg arg)
{
	return _ViCtrol->ab_accepted_order(arg);
}
int DarkHorse::ViStockClient::dm_accepted_order(DhTaskArg arg)
{
	return _ViCtrol->dm_accepted_order(arg);
}

int ViStockClient::ab_symbol_quote(DhTaskArg arg)
{
	return _ViCtrol->ab_symbol_quote(std::move(arg));
}

int ViStockClient::ab_symbol_hoga(DhTaskArg arg)
{
	return _ViCtrol->ab_symbol_hoga(std::move(arg));
}

