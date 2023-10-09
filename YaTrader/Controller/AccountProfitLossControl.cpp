#include "stdafx.h"
#include "AccountProfitLossControl.h"
#include "../Position/Position.h"
#include "../Position/AccountPositionManager.h"
#include "../Position/TotalPositionManager.h"
#include "../Global/SmTotalManager.h"
#include "../Util/IdGenerator.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Account/SmAccount.h"
#include "../Fund/SmFund.h"
#include "../Position/GroupPositionManager.h"
#include "../Event/SmCallbackManager.h"
namespace DarkHorse {

AccountProfitLossControl::AccountProfitLossControl()
	: id_(IdGenerator::get_id())
{
	mainApp.CallbackMgr()->subscribe_position_event_handler
	(
		id_,
		std::bind(&AccountProfitLossControl::update_position, this, std::placeholders::_1)
	);

	mainApp.event_hub()->subscribe_quote_event_handler
	(
		id_,
		std::bind(&AccountProfitLossControl::update_profit_loss, this, std::placeholders::_1)
	);

	mainApp.event_hub()->subscribe_account_profit_loss_event_handler
	(
		id_,
		std::bind(&AccountProfitLossControl::update_account_profit_loss, this)
	);
}

AccountProfitLossControl::~AccountProfitLossControl()
{
	mainApp.CallbackMgr()->unsubscribe_position_event_handler(id_);
	mainApp.event_hub()->unsubscribe_quote_event_handler(id_);
	mainApp.event_hub()->unsubscribe_account_profit_loss_event_handler(id_);
}

void AccountProfitLossControl::reset_account_profit_loss()
{
	account_profit_loss_.open_profit_loss = 0;
	account_profit_loss_.pure_trade_profit_loss = 0;
	account_profit_loss_.trade_fee = 0;
	account_profit_loss_.trade_profit_loss = 0;
}

position_p AccountProfitLossControl::get_position(const std::string& symbol_code)
{
	auto it = position_map_.find(symbol_code);
	if (it == position_map_.end()) return nullptr;
	return it->second;
}

void AccountProfitLossControl::load_profit_loss()
{
	if (position_type_ == OrderType::None) return ;
	if (position_type_ == OrderType::SubAccount) {
		if (!account_) return;
		auto position_manager = mainApp.total_position_manager()->find_position_manager(account_->No());
		if (!position_manager) { reset_account_profit_loss();  return; }
		position_manager->get_account_profit_loss(account_profit_loss_);
	}
	else if (position_type_ == OrderType::Fund) {
		if (!fund_) return;
		auto position_manager = mainApp.total_position_manager()->find_fund_group_position_manager(fund_->Name());
		if (!position_manager) { reset_account_profit_loss();  return; }
		position_manager->get_account_profit_loss(account_profit_loss_);
	}
	else {
		if (!account_) return;
		auto position_manager = mainApp.total_position_manager()->find_account_group_position_manager(account_->No());
		if (!position_manager) { reset_account_profit_loss();  return; }
		position_manager->get_account_profit_loss(account_profit_loss_);
	}
	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::update_position(position_p position)
{
	if (!position) return;
	if (position_type_ == OrderType::None) return;
	if (position_type_ == OrderType::SubAccount) {
		if (!account_ || account_->No() != position->account_no) return;
		auto position_manager = mainApp.total_position_manager()->find_position_manager(position->account_no);
		if (!position_manager) { reset_account_profit_loss();  return; }
		position_manager->get_account_profit_loss(account_profit_loss_);
	}
	else if (position_type_ == OrderType::Fund) {
		if (!fund_ || fund_->Name() != position->fund_name) return;
		auto position_manager = mainApp.total_position_manager()->find_fund_group_position_manager(position->fund_name);
		if (!position_manager) { reset_account_profit_loss();  return; }
		position_manager->get_account_profit_loss(account_profit_loss_);
	}
	else {
		if (!account_ || account_->No() != position->account_no) return;
		auto position_manager = mainApp.total_position_manager()->find_account_group_position_manager(position->account_no);
		if (!position_manager) { reset_account_profit_loss();  return; }
		position_manager->get_account_profit_loss(account_profit_loss_);
	}

	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::update_profit_loss(quote_p quote)
{
	if (!quote) return;
	auto symbol = mainApp.SymMgr()->FindSymbol(quote->symbol_code);
	if (!symbol) return;
	position_p position = get_position(quote->symbol_code);
	if (!position) return;
	
	if (event_handler_) event_handler_();
}

void AccountProfitLossControl::set_account(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account) return;
	account_ = account;
	account_id_ = account->id();
	if (account_->is_subaccount()) {
		position_type_ = OrderType::SubAccount;
	}
	else {
		position_type_ = OrderType::MainAccount;
	}
	load_profit_loss();
}

void AccountProfitLossControl::set_fund(std::shared_ptr<SmFund> fund)
{
	if (!fund) return;
	fund_ = fund;
	position_type_ = OrderType::Fund;
	load_profit_loss();
}

void AccountProfitLossControl::update_account_profit_loss()
{
// 	if (!account_) return;
// 	account_position_manager_p acnt_position_mgr = mainApp.total_position_manager()->get_account_position_manager(account_->No());
// 	const std::map<std::string, position_p>& position_map = acnt_position_mgr->get_position_map();
// 
// 	if (position_map_.size() != position_map.size())
// 		return load_position_from_account(account_->No());
// 	refresh_account_profit_loss();
// 	if (event_handler_) event_handler_();
}
}