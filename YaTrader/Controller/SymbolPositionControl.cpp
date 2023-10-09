#include "stdafx.h"
#include "SymbolPositionControl.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../Position/Position.h"
#include "../Log/MyLogger.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Position/TotalPositionManager.h"
#include "../Fund/SmFund.h"
#include "../Position/GroupPositionManager.h"
#include "../Position/AccountPositionManager.h"
#include "../Event/SmCallbackManager.h"
namespace DarkHorse {
	SymbolPositionControl::SymbolPositionControl()
		: id_(IdGenerator::get_id())
	{
		mainApp.CallbackMgr()->subscribe_position_event_handler
		(
			id_,
			std::bind(&SymbolPositionControl::update_position, this, std::placeholders::_1)
		);
		mainApp.event_hub()->subscribe_quote_event_handler
		(
			id_,
			std::bind(&SymbolPositionControl::update_profit_loss, this, std::placeholders::_1)
		);

		//LOGINFO(CMyLogger::getInstance(), "SymbolPositionControl contructor id = %d", id_);
	}

	SymbolPositionControl::~SymbolPositionControl()
	{
		mainApp.event_hub()->unsubscribe_quote_event_handler(id_);
		mainApp.CallbackMgr()->unsubscribe_position_event_handler(id_);

		//LOGINFO(CMyLogger::getInstance(), "SymbolPositionControl destructor id = %d", id_);
	}

	void SymbolPositionControl::update_profit_loss(std::shared_ptr<SmQuote> quote)
	{
		if (symbol_id_ != 0 && quote->symbol_id != symbol_id_) return;
		TotalPositionManager::calculate_symbol_open_profit_loss(position_);
		if (event_handler_) event_handler_();
	}

	void SymbolPositionControl::update_position(std::shared_ptr<Position> position)
	{
		try {
			//LOGINFO(CMyLogger::getInstance(), "SymbolPositionControl update_position id = %d , account_no = %s, fund_name = %s", id_, position->account_no.c_str(), position->fund_name.c_str());
			if (!position || position->order_source_type != position_type_) return;
			if (symbol_ && symbol_->SymbolCode() != position->symbol_code) return;
			
			if (position_type_ == OrderType::SubAccount) {
				if (!account_  || account_->No() != position->account_no) return;
				mainApp.total_position_manager()->get_position_from_account(position->account_no, position->symbol_code, position_);
			}
			else if (position_type_ == OrderType::Fund) {
				if (!fund_ && fund_->Name() != position->fund_name) return;
				mainApp.total_position_manager()->get_position_from_fund(position->fund_name, position->symbol_code, position_);
			}
			else {
				if (!account_ || account_->No() != position->account_no) return;
				mainApp.total_position_manager()->get_position_from_parent_account(position->account_no, position->symbol_code, position_);
			}

			if (event_handler_) event_handler_();
			// For the option view and future view
			if (vm_option_event_handler_) {
				position_.symbol_code = position->symbol_code;
				position_.symbol_id = mainApp.SymMgr()->get_symbol_id(position->symbol_code);
				position_.account_no = position->account_no;
				position_.account_id = mainApp.AcntMgr()->get_account_id(position->account_no);
				position_.position_type = position_type_;
				vm_option_event_handler_(position_);
			}
			if (vm_fund_event_handler_) {
				position_.symbol_code = position->symbol_code;
				position_.symbol_id = mainApp.SymMgr()->get_symbol_id(position->symbol_code);
				position_.account_no = position->account_no;
				position_.account_id = mainApp.AcntMgr()->get_account_id(position->account_no);
				position_.position_type = position_type_;
				vm_fund_event_handler_(position_);
			}
			if (out_system_event_handler_) out_system_event_handler_(out_system_id_);
		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	void SymbolPositionControl::set_symbol(std::shared_ptr<SmSymbol> symbol)
	{
		if (!symbol) return;
		symbol_ = symbol;
		symbol_id_ = symbol->Id();
		symbol_decimal_ = symbol->decimal();
		symbol_seung_su_ = symbol->seung_su();

		position_.symbol_id = symbol->Id();
		position_.symbol_code = symbol->SymbolCode();

		reset_position();
	}

	void SymbolPositionControl::set_account(std::shared_ptr<SmAccount> account)
	{
		if (!account) return;
		account_ = account;
		account_id_ = account_->id();
		position_.account_id = account_->id();
		if (account_->is_subaccount()) {
			position_type_ = OrderType::SubAccount;
		}
		else {
			position_type_ = OrderType::MainAccount;
		}
		reset_position();
	}

	void SymbolPositionControl::set_fund(std::shared_ptr<SmFund> fund)
	{
		if (!fund) return;
		fund_ = fund;
		position_.fund_id = fund_->Id();
		position_type_ = OrderType::Fund;
		reset_position();
	}

	bool SymbolPositionControl::is_account_exist(const std::shared_ptr<Position>& position)
	{
		if (!position) return false;
		auto it = account_map_.find(position->account_no);
		if (it == account_map_.end()) return false;
		return true;
	}

	void SymbolPositionControl::reset_position()
	{
		clear_position();
		if (position_type_ == OrderType::SubAccount) {
			if (!account_ || !symbol_) return;
			mainApp.total_position_manager()->get_position_from_account(account_->No(), symbol_->SymbolCode(), position_);
		}
		else if (position_type_ == OrderType::Fund) {
			if (!fund_ || !symbol_) return;
			mainApp.total_position_manager()->get_position_from_fund(fund_->Name(), symbol_->SymbolCode(), position_);
		}
		else {
			if (!account_ || !symbol_) return;
			mainApp.total_position_manager()->get_position_from_parent_account(account_->No(), symbol_->SymbolCode(), position_);
		}

		if (event_handler_) event_handler_();
		if (vm_option_event_handler_) vm_option_event_handler_(position_);
		if (out_system_event_handler_) out_system_event_handler_(out_system_id_);
	}

	void SymbolPositionControl::subscribe_position_control()
	{
		mainApp.CallbackMgr()->subscribe_position_event_handler
		(
			id_,
			std::bind(&SymbolPositionControl::update_position, this, std::placeholders::_1)
		);
	}

	void SymbolPositionControl::clear_position()
	{
		position_map_.clear();

		position_.average_price = 0.0;
		position_.open_quantity = 0;
		position_.open_profit_loss = 0.0;
		position_.trade_profit_loss = 0.0;
	}

}