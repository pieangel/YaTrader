#include "stdafx.h"
#include "AccountPositionManager.h"
#include "Position.h"
#include "../Order/Order.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Order/OrderRequest/OrderRequest.h"
#include "../Order/OrderRequest/OrderRequestManager.h"
#include "../Order/SmOrderConst.h"
#include <functional>
#include "../Util/IdGenerator.h"
#include "../Log/MyLogger.h"
#include "TotalPositionManager.h"
#include "../Event/SmCallbackManager.h"
namespace DarkHorse {

AccountPositionManager::AccountPositionManager(TotalPositionManager& total_position_manager, const std::string& account_no)
	: total_position_manager_(total_position_manager)
	, account_no_(account_no)
{
	id_ = IdGenerator::get_id();
	account_profit_loss_ = std::make_shared<AccountProfitLoss>();
}

AccountPositionManager::~AccountPositionManager()
{
}

position_p AccountPositionManager::get_position(const std::string& symbol_code)
{
	position_p position = find_position(symbol_code);
	if (position) return position;
	return create_position(symbol_code);
}
position_p AccountPositionManager::find_position(const std::string& symbol_code)
{
	auto it = position_map_.find(symbol_code);
	return it != position_map_.end() ? it->second : nullptr;
}

position_p AccountPositionManager::create_position(const std::string& symbol_code)
{
	std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
	position_p position = std::make_shared<Position>();
	position->symbol_code = symbol_code;
	position->account_no = account_no_;
	position->id = IdGenerator::get_id();
	position_map_[symbol_code] = position;
	return position;
}

void AccountPositionManager::get_account_profit_loss(VmAccountProfitLoss& dest_account_profit_loss)
{
	if (!account_profit_loss_) return;
	dest_account_profit_loss.open_profit_loss = account_profit_loss_->open_profit_loss;
	dest_account_profit_loss.pure_trade_profit_loss = account_profit_loss_->pure_trade_profit_loss;
	dest_account_profit_loss.trade_fee = account_profit_loss_->trade_fee;
	dest_account_profit_loss.trade_profit_loss = account_profit_loss_->trade_profit_loss;
}

void AccountPositionManager::update_account_profit_loss()
{
	double trade_profit_loss{ 0.0f };       //매매(청산)손익
	double open_profit_loss{ 0.0f };		//평가손익
	double pure_trade_profit_loss{ 0.0f };  // 청산 순손익
	double trade_fee{ 0.0f };               // 청산 수수료
	for (auto it = position_map_.begin(); it != position_map_.end(); it++) {
		auto position = it->second;
		update_open_profit_loss(position);
		trade_profit_loss += position->trade_profit_loss;
		open_profit_loss += position->open_profit_loss;
		trade_fee += position->trade_fee;
		pure_trade_profit_loss += position->pure_trade_profit_loss;
	}
	account_profit_loss_->trade_profit_loss = trade_profit_loss;
	account_profit_loss_->open_profit_loss = open_profit_loss;
	account_profit_loss_->trade_fee = trade_fee;
	account_profit_loss_->pure_trade_profit_loss = pure_trade_profit_loss;

	//LOGINFO(CMyLogger::getInstance(), "update_account_profit_loss :: account_no[%s], 당일매매손익[%.2f], 당일평가손익[%.2f], 수수료[%.2f], 당일총손익[%.2f]", account_no_.c_str(), trade_profit_loss, open_profit_loss, trade_fee, pure_trade_profit_loss);

}
// update the position with the order.
void AccountPositionManager::update_position(order_p order)
{
	if (!order) return;

	position_p position = get_position(order->symbol_code);
	position->account_no = order->account_no;
	set_symbol_id(position, order->symbol_code);
	set_account_id(position, order->account_no);
	position->fund_name = order->order_context.fund_name;
	position->fund_id = order->order_context.fund_id;
	// 펀드 일때는 서브 계좌들을 먼저 처리하고 펀드는 그룹 포지션에서 처리하도록 한다. 
	if (order->order_context.order_source_type == OrderType::Fund)
		position->order_source_type = OrderType::SubAccount;
	else
		position->order_source_type = order->order_context.order_source_type;
	position->parent_account_no = order->order_context.parent_account_no;
	auto symbol = mainApp.SymMgr()->FindSymbol(order->symbol_code);
	if (!symbol) return;
	//LOGINFO(CMyLogger::getInstance(), "position_count = [%d], filled_count = [%d], average_price = [%.2f], filled_price = [%d]", position->open_quantity, order->filled_count, position->average_price, order->filled_price);

	const int new_position_count = calculate_position_count(order, position);
	const int unsettled_count = calculate_unsettled_count(order, position);
	const double traded_profit_loss = calculate_traded_profit_loss(order, position, symbol->decimal(), symbol->seung_su());
	const double new_average_price = calculate_average_price(order, position, new_position_count, unsettled_count);

	LOGINFO(CMyLogger::getInstance(), "position_count = [%d], unsettled_count = [%d], trade_profit_loss = [%.2f], average_price = [%.2f]", new_position_count, unsettled_count, traded_profit_loss, new_average_price);
	
	position->trade_profit_loss += traded_profit_loss;
	position->open_quantity = new_position_count;
	position->average_price = new_average_price;

	update_open_profit_loss(position);

	// update the involved order.
	order->unsettled_count = unsettled_count;
	if (order->unsettled_count == 0)
		order->order_state = SmOrderState::Settled;
	else
		mainApp.event_hub()->process_stop_order_event(order);
	LOGINFO(CMyLogger::getInstance(), "symbol position  open qty = [%d], average_price = [%.2f], open pl = [%.2f]", position->open_quantity, position->average_price, position->open_profit_loss);

	update_account_profit_loss();
	mainApp.CallbackMgr()->process_position_event(position);

	total_position_manager_.update_group_position(position);
	
}

void AccountPositionManager::update_position(quote_p quote)
{
	if (!quote) return;
	position_p position = find_position(quote->symbol_code);
	if (!position) return;
	TotalPositionManager::calculate_symbol_open_profit_loss(position);
	//LOGINFO(CMyLogger::getInstance(), "open_quantity = [%d], position->average_price = [%.2f], open_profit_loss = [%.2f]", position->open_quantity, position->average_price, position->open_profit_loss);

	update_account_profit_loss();

	mainApp.CallbackMgr()->process_position_event(position);
}

void AccountPositionManager::update_position(position_p position)
{
	if (!position) return;
	update_open_profit_loss(position);
	update_account_profit_loss();
}

void AccountPositionManager::update_position(position_p position, VmPosition& dest_position)
{
	if (!position) return;
	update_open_profit_loss(position);
	dest_position.open_profit_loss = position->open_profit_loss;

	dest_position.trade_profit_loss = position->trade_profit_loss;
	dest_position.open_profit_loss = position->open_profit_loss;
	dest_position.trade_fee = position->trade_fee;
	dest_position.pure_trade_profit_loss = position->pure_trade_profit_loss;
	dest_position.average_price = position->average_price;
	dest_position.open_quantity = position->open_quantity;
	if (position->open_quantity == 0)
		dest_position.average_price = 0.0f;

	update_account_profit_loss();
}

void AccountPositionManager::get_active_positions(std::map<std::string, position_p>& position_vector)
{
	std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
	for (auto it = position_map_.begin(); it != position_map_.end(); it++) {
		auto position = it->second;
		if (position->open_quantity != 0)
			position_vector[it->first] = it->second;
	}
}

void AccountPositionManager::update_trade_profit_loss(const double trade_profit_loss, const double trade_fee)
{
	account_profit_loss_->trade_profit_loss = trade_profit_loss;
	account_profit_loss_->trade_fee = trade_fee;
}

void AccountPositionManager::set_symbol_id(position_p position, const std::string& symbol_code)
{
	if (!position || position->symbol_id != 0) return;

	auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (!symbol) return;
	position->symbol_id = symbol->Id();
}

void AccountPositionManager::set_account_id(position_p position, const std::string& account_no)
{
	if (!position || position->account_id != 0) return;

	auto account = mainApp.AcntMgr()->FindAccount(account_no);
	if (!account) return;

	position->account_id = account->id();
}

int AccountPositionManager::calculate_position_count(order_p order, position_p position)
{
	const int order_filled_sign = order->position == SmPositionType::Buy ? 1 : -1;
	const int signed_filled_count = order->filled_count * order_filled_sign;
	return signed_filled_count + position->open_quantity;
}
int AccountPositionManager::calculate_unsettled_count(order_p order, position_p position)
{
	const int order_filled_sign = order->position == SmPositionType::Buy ? 1 : -1;
	const int signed_filled_count = order->filled_count * order_filled_sign;
	if (position->open_quantity == 0) return signed_filled_count;
	if (position->open_quantity * signed_filled_count > 0) return signed_filled_count;
	if (abs(position->open_quantity) >= order->filled_count) return 0;
	return position->open_quantity + signed_filled_count;
}
int AccountPositionManager::calculate_traded_count(order_p order, position_p position)
{
	const int order_filled_sign = position->open_quantity > 0 ? 1 : -1;
	const int signed_filled_count = order->filled_count * order_filled_sign;
	if (position->open_quantity * signed_filled_count >= 0) return 0;
	return min(abs(position->open_quantity), abs(signed_filled_count));
}
double AccountPositionManager::calculate_traded_profit_loss(order_p order, position_p position, const int& symbol_decimal, const int& symbol_seungsu)
{
	const int traded_count = calculate_traded_count(order, position);
	const double price_gap = (order->filled_price - position->average_price) / pow(10, symbol_decimal);
	double trade_profit_loss = price_gap * traded_count * symbol_seungsu; 
	// 매도는 계산이 반대로 이루어짐. 
	if (position->open_quantity < 0) trade_profit_loss *= -1;
	return trade_profit_loss;
}
double AccountPositionManager::calculate_average_price(order_p order, position_p position, const int& new_open_quantity, const int& unsettled_count)
{
	if (new_open_quantity == 0) return 0;
	if (abs(new_open_quantity) >  abs(position->open_quantity)) {
		const double position_average_price = position->average_price * abs(position->open_quantity);
		const double filled_price = order->filled_price * abs(unsettled_count);
		return (position_average_price + filled_price) / (abs(new_open_quantity));
	}
	else
		return position->average_price;
}
void AccountPositionManager::update_open_profit_loss(position_p position)
{
	TotalPositionManager::calculate_symbol_open_profit_loss(position);
}

}