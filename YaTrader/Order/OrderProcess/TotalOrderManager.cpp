#include "stdafx.h"
#include "TotalOrderManager.h"
#include "AccountOrderManager.h"
#include "../../Log/MyLogger.h"
#include "../OrderRequest/OrderRequestManager.h"
#include "../OrderRequest/OrderRequest.h"
#include "../../Global/SmTotalManager.h"
#include "../Order.h"
#include "../../Account/SmAccount.h"
#include "../../Account/SmAccountManager.h"
#include "SymbolOrderManager.h"
#include "../../Fund/SmFund.h"
#include "../../Fund/SmFundManager.h"
namespace DarkHorse 
{
using account_order_manager_p = std::shared_ptr<AccountOrderManager>;

void TotalOrderManager::on_order_event(order_event&& order_info)
{
	//order_p order = make_order(order_info);
	order_p order = make_order(std::forward<order_event>(order_info));
	if (!order) return;
	const std::string& custom_info = order_info["custom_info"];
	set_order_request_info(custom_info, order);
	const OrderEvent order_event = order_info["order_event"];
	write_order_history(order_event, order);
	dispatch_order(order_event, order);
}

void TotalOrderManager::dispatch_order(const OrderEvent order_event, order_p order)
{
	assert(order);
	account_order_manager_p order_manager = get_account_order_manager(order->account_no);
	order_manager->dispatch_order(order_event, order);
}
void TotalOrderManager::write_order_history(const OrderEvent order_event, order_p order)
{
	assert(order);

// 	LOGINFO(CMyLogger::getInstance(), "order log order event[%d], 계좌[%s],[서브계좌번호[%s], [부모계좌번호[%s], 펀드이름[%s], 신호이름[%s], 종목[%s], 주문번호[%s], 주문구분[%d], 주문수량[%d], 체결수량[%d], 잔량[%d]\n",
// 		(int)order_event,
// 		order->account_no.c_str(),
// 		order->order_context.sub_account_no.c_str(),
// 		order->order_context.fund_name.c_str(),
// 		order->order_context.signal_name.c_str(),
// 		order->order_context.parent_account_no.c_str(),
// 		order->symbol_code.c_str(),
// 		order->order_no.c_str(),
// 		(int)order->order_type,
// 		order->order_amount,
// 		order->filled_count,
// 		order->remain_count);
}

account_order_manager_p TotalOrderManager::find_account_order_manager(const std::string& account_no) 
{
	auto it = account_order_manager_map_.find(account_no);
	return it != account_order_manager_map_.end() ? it->second : nullptr;
}
account_order_manager_p TotalOrderManager::create_account_order_manager(const std::string& account_no)
{
	account_order_manager_p order_manager = std::make_shared<AccountOrderManager>();
	order_manager->set_account_no(account_no);
	account_order_manager_map_[account_no] = order_manager;
	return order_manager;
}
account_order_manager_p TotalOrderManager::get_account_order_manager(const std::string& account_no) 
{
	account_order_manager_p order_manager = find_account_order_manager(account_no);
	if (order_manager) return order_manager;
	return create_account_order_manager(account_no);
}

std::pair<bool, int> TotalOrderManager::get_init_and_acpt_order_count_from_account(const std::string& account_no, const std::string& symbol_code)
{
	std::pair<bool, int> result(false, 0);
	auto account = mainApp.AcntMgr()->FindAccount(account_no);
	if (!account) return result;
	auto account_order_manager = get_account_order_manager(account_no);
	if (!account_order_manager) return result;
	auto symbol_order_manager = account_order_manager->find_symbol_order_manager(symbol_code);
	if (!symbol_order_manager) return result;
	result.first = symbol_order_manager->get_ordered_before();
	result.second = symbol_order_manager->get_accepted_count();
	return result;
}

std::pair<bool, int> TotalOrderManager::get_init_and_acpt_order_count_from_fund(const std::string& fund_name, const std::string& symbol_code)
{
	std::pair<bool, int> result(false, 0);
	auto fund = mainApp.FundMgr()->FindFund(fund_name);
	if (!fund) return result;
	const std::vector<std::shared_ptr<SmAccount>>& sub_accounts = fund->GetAccountVector();
	for (auto it = sub_accounts.begin(); it != sub_accounts.end(); ++it) {
		auto account_order_manager = get_account_order_manager((*it)->No());
		if (!account_order_manager) continue;
		auto symbol_order_manager = account_order_manager->find_symbol_order_manager(symbol_code);
		if (!symbol_order_manager) continue;
		// Only if the current symbol order manager is not ordered before, we set the result.first to the next value.
		if (!result.first) result.first = symbol_order_manager->get_ordered_before();
		result.second += symbol_order_manager->get_accepted_count();
	}
	return result;
}

std::pair<bool, int> TotalOrderManager::get_init_and_acpt_order_count_from_parent_account(const std::string& account_no, const std::string& symbol_code)
{
	std::pair<bool, int> result(false, 0);
	auto account = mainApp.AcntMgr()->FindAccount(account_no);
	if (!account) return result;
	result = get_init_and_acpt_order_count_from_account(account_no, symbol_code);
	const std::vector<std::shared_ptr<SmAccount>>& sub_accounts = account->get_sub_accounts();
	for (auto it = sub_accounts.begin(); it != sub_accounts.end(); ++it) {
		auto account_order_manager = get_account_order_manager((*it)->No());
		if (!account_order_manager) continue;
		auto symbol_order_manager = account_order_manager->find_symbol_order_manager(symbol_code);
		if (!symbol_order_manager) continue;
		// Only if the current symbol order manager is not ordered before, we set the result.first to the next value.
		if (!result.first) result.first = symbol_order_manager->get_ordered_before();
		result.second += symbol_order_manager->get_accepted_count();
	}
	return result;
}

order_p TotalOrderManager::make_order(const order_event& order_info)
{
	try {
		const std::string order_no = order_info["order_no"];
		const int order_event = order_info["order_event"];
		order_p order = mainApp.total_order_manager()->get_order(order_no);
		order->order_no = order_no;
		order->account_no = order_info["account_no"];
		order->symbol_code = order_info["symbol_code"];
		// 주문 유형 - 매수 / 매도
		const std::string position_type = order_info["position_type"];
		if (position_type.compare("1") == 0) order->position = SmPositionType::Buy;
		else if (position_type.compare("2") == 0) order->position = SmPositionType::Sell;

		switch (order_event) {
		case OrderEvent::OE_Accepted: { // 접수확인
			order->order_price = order_info["order_price"];
			order->order_amount = order_info["order_amount"];
			order->order_time = order_info["order_time"];
			order->order_date = order_info["order_date"];

			const std::string order_type = order_info["order_type"];
			if (order_type.compare("1") == 0) order->order_type = SmOrderType::New;
			else if (order_type.compare("2") == 0) order->order_type = SmOrderType::Modify;
			else if (order_type.compare("3") == 0) order->order_type = SmOrderType::Cancel;
		}
		break;
		case OrderEvent::OE_Unfilled: { // 미체결
			order->order_price = order_info["order_price"];
			order->order_amount = order_info["order_amount"];

			order->original_order_no = order_info["original_order_no"];
			order->first_order_no = order_info["first_order_no"];

			order->remain_count = order_info["remain_count"];
			order->modified_count = order_info["modified_count"];
			order->cancelled_count = order_info["cancelled_count"];
			order->filled_count = order_info["filled_count"];
		}
		break;
		case OrderEvent::OE_Filled: { // 체결
			order->filled_price = order_info["filled_price"];
			order->filled_count = order_info["filled_count"];
			order->filled_time = order_info["filled_time"];
			order->filled_date = order_info["filled_date"];
		}
		break;
		}

		return order;
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	return nullptr;
}

void TotalOrderManager::set_order_request_info(const std::string& custom_info, order_p order)
{
	if (!order || custom_info.empty()) return;
	order_request_p order_request = mainApp.order_request_manager()->find_order_request(custom_info);
	if (!order_request) return;
	order->order_amount = order_request->order_amount;
	order->order_type = order_request->order_type;
	order->order_request_id = order_request->request_id;
	order->order_context.order_control_id = order_request->order_context.order_control_id;
	order->order_context.fund_id = order_request->order_context.fund_id;
	order->order_context.parent_account_id = order_request->order_context.parent_account_id;
	order->order_context.order_source_type = order_request->order_context.order_source_type;
	order->account_no = order_request->order_context.sub_account_no;
	order->order_context.sub_account_no = order_request->order_context.sub_account_no;
	order->order_context.parent_account_no = order_request->order_context.parent_account_no;
	order->order_context.fund_name = order_request->order_context.fund_name;
	order->order_context.signal_name = order_request->order_context.signal_name;

	LOGINFO(CMyLogger::getInstance(), "set_order_request_info 계좌[%s],[서브계좌번호[%s], [부모계좌번호[%s], 펀드이름[%s], 시그널이름[%s], 종목[%s], 주문번호[%s], 주문구분[%d], 주문수량[%d], 체결수량[%d], 잔량[%d]", 
		order_request->account_no.c_str(), 
		order->order_context.sub_account_no.c_str(),
		order->order_context.fund_name.c_str(),
		order->order_context.parent_account_no.c_str(),
		order->order_context.signal_name.c_str(),
		order_request->symbol_code.c_str(), 
		order->order_no.c_str(), 
		(int)order->order_type, 
		order->order_amount, 
		order->filled_count, 
		order->remain_count);

}

order_p TotalOrderManager::get_order(const std::string& order_no)
{
	order_p order = find_order(order_no);
	if (order) return order;
	return create_order(order_no);
}
order_p TotalOrderManager::find_order(const std::string& order_no)
{
	auto found = order_map_.find(order_no);
	return (found != order_map_.end()) ? found->second : nullptr;
}
order_p TotalOrderManager::create_order(const std::string& order_no)
{
	order_p order = std::make_shared<Order>();
	order->order_no = order_no;
	order_map_[order_no] = order;
	return order;
}
}