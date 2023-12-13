#include "stdafx.h"
#include "OrderControl.h"
#include "../Order/Order.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Util/IdGenerator.h"
#include "../Event/EventHub.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Order/OrderProcess/TotalOrderManager.h"
#include "../Order/OrderProcess/AccountOrderManager.h"
#include "../Order/OrderProcess/SymbolOrderManager.h"
#include "../Fund/SmFundManager.h"
#include "../Fund/SmFund.h"
#include "../Account/SmAccountManager.h"

namespace DarkHorse {

	OrderControl::OrderControl()
		: id_(IdGenerator::get_id())
	{
		buy_order_control_.set_control_type( SubOrderControlType::CT_BUY );
		sell_order_control_.set_control_type( SubOrderControlType::CT_SELL );

		mainApp.event_hub()->subscribe_order_event_handler
		(
			id_,
			std::bind(&OrderControl::update_order, this, std::placeholders::_1, std::placeholders::_2)
		);
	}

	OrderControl::~OrderControl()
	{
		mainApp.event_hub()->unsubscribe_order_event_handler(id_);
	}

	void OrderControl::clear()
	{
		buy_order_control_.clear();
		sell_order_control_.clear();
	}


	void OrderControl::load_from_account(const bool is_sub_account, const std::string& account_no, const std::string& symbol_code)
	{
		clear();
		if (is_sub_account) {
			add_order(account_no, symbol_code);
		}
		else {
			const auto& account = mainApp.AcntMgr()->FindAccount(account_no);
			if (!account) return;
			// For parent account
			add_order(account_no, symbol_code);
			const auto& sub_account_vector = account->get_sub_accounts();
			for (auto it = sub_account_vector.begin(); it != sub_account_vector.end(); ++it) {
				auto sub_account = *it;
				add_order(sub_account->No(), symbol_code);
			}
		}

		if (event_handler_) event_handler_();
	}

	void OrderControl::load_from_fund(const std::string& fund_name, const std::string& symbol_code)
	{
		clear();
		auto fund = mainApp.FundMgr()->FindFund(fund_name);
		if (!fund) return;
		const std::vector<std::shared_ptr<SmAccount>>& sub_account_vector = fund->GetAccountVector();
		for (auto it = sub_account_vector.begin(); it != sub_account_vector.end(); ++it) {
			auto sub_account = *it;
			add_order(sub_account->No(), symbol_code);
		}

		if (event_handler_) event_handler_();
	}

	void OrderControl::add_order(const std::map<std::string, std::shared_ptr<Order>>& accepted_order_map)
	{
		for(const auto& account : accepted_order_map) {
			add_order(account.second);
		}
	}

	void OrderControl::add_order(const std::string& account_no, const std::string& symbol_code)
	{
		auto account_order_manager = mainApp.total_order_manager()->get_account_order_manager(account_no);
		auto symbol_order_manager = account_order_manager->get_symbol_order_manager(symbol_->SymbolCode());
		std::vector<order_p> order_vec;
		symbol_order_manager->get_accepted_order_vector(order_vec);
		if (order_vec.empty()) return;

		add_order(order_vec);
	}

	void OrderControl::add_order(const std::vector<std::shared_ptr<Order>>& accepted_order_vector)
	{
		for (const auto& order : accepted_order_vector) {
			add_order(order);
		}
	}

	void OrderControl::set_symbol(std::shared_ptr<SmSymbol> symbol)
	{
		if (!symbol) return;
		symbol_ = symbol;
		if (!account_) return;
		load_from_account(account_->is_subaccount(), account_->No(), symbol_->SymbolCode());
	}

	void OrderControl::set_account(std::shared_ptr<SmAccount> account)
	{
		if (!account) return;
		account_ = account;
		if (account_->is_subaccount()) {
			account_map_.clear();
			account_map_[account_->No()] = account_;
		}
		else {
			account_map_.clear();
			account_map_[account_->No()] = account_;
			const auto& account_vector = account_->get_sub_accounts();
			for (auto it = account_vector.begin(); it != account_vector.end(); ++it) {
				auto sub_account = *it;
				account_map_[sub_account->No()] = sub_account;
			}
		}
		if (!symbol_) return;
		load_from_account(account_->is_subaccount(), account_->No(), symbol_->SymbolCode());
	}

	void OrderControl::set_fund(std::shared_ptr<SmFund> fund)
	{
		if (!fund) return;
		fund_ = fund;
		account_map_.clear();
		const auto& account_vector = fund->GetAccountVector();
		for (auto it = account_vector.begin(); it != account_vector.end(); ++it) {
			auto sub_account = *it;
			account_map_[sub_account->No()] = sub_account;
		}

		load_from_fund(fund->Name(), symbol_->SymbolCode());
	}

	void OrderControl::update_order(std::shared_ptr<Order> order, OrderEvent order_event)
	{
		if (!order || !symbol_) return;
		if (symbol_->SymbolCode() != order->symbol_code) return;
		
		auto it = account_map_.find(order->account_no);
		if (it == account_map_.end()) return;

		if (order_event == OrderEvent::OE_Accepted)
			on_order_accepted(order);
		else if (order_event == OrderEvent::OE_Unfilled)
			on_order_unfilled(order);
		else if (order_event == OrderEvent::OE_Filled)
			remove_order(order);

		if (event_handler_) event_handler_();
	}
	void OrderControl::on_order_unfilled(std::shared_ptr<Order> order)
	{
		if (order->remain_count == 0)
			remove_order(order);
		else
			add_order(order);
	}

	void OrderControl::on_order_accepted(std::shared_ptr<Order> order)
	{
		if (order->order_type == SmOrderType::Modify) {
			remove_order(order->original_order_no);
			add_order(order);
		}
		else if (order->order_type == SmOrderType::Cancel)
			remove_order(order->original_order_no);
		else if (order->order_type == SmOrderType::New)
			add_order(order);
	}

	std::pair<int, int> OrderControl::get_order_count(const SmPositionType& position, const int price)
	{
		if (position == SmPositionType::Buy)
			return get_order_count(buy_order_control_, price);
		else
			return get_order_count(sell_order_control_, price);
	}

	std::pair<int, int> OrderControl::get_order_count(SubOrderControl& order_control, const int price)
	{
		const std::map<int, std::shared_ptr<PriceOrderMap>>& order_map = order_control.get_order_map();
		auto it_price = order_map.find(price);
		if (it_price == order_map.end()) return std::make_pair(0, 0);
		const std::shared_ptr<PriceOrderMap>& price_order_map = it_price->second;
		return std::make_pair(it_price->first, price_order_map->count());
	}

	void OrderControl::add_order(std::shared_ptr<Order> order)
	{
		if (order->position == SmPositionType::Buy)
			buy_order_control_.add_order(order->order_price, order);
		else
			sell_order_control_.add_order(order->order_price, order);
	}



	void OrderControl::remove_order(std::shared_ptr<Order> order)
	{
		if (order->position == SmPositionType::Buy)
			buy_order_control_.remove_order(order->order_price, order->order_no);
		else
			sell_order_control_.remove_order(order->order_price, order->order_no);
	}

	void OrderControl::remove_order(const std::string& order_no)
	{
		auto order = mainApp.total_order_manager()->find_order(order_no);
		if (!order) return;
		remove_order(order);
	}

	void OrderControl::get_order(const SmPositionType& position, const int& price, std::vector<std::shared_ptr<Order>>& order_vector)
	{
		if (position == SmPositionType::Buy)
			buy_order_control_.get_order(price, order_vector);
		else
			sell_order_control_.get_order(price, order_vector);
	}

	void OrderControl::clear_order(const SmPositionType& position, const int& price)
	{
		if (position == SmPositionType::Buy)
			buy_order_control_.clear_order(price);
		else
			sell_order_control_.clear_order(price);
	}

}