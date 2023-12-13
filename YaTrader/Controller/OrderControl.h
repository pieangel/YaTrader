#pragma once
#include <map>
#include <set>
#include <string>
#include <memory>
#include <functional>
#include "SubOrderControl.h"
#include "../Order/SmOrderConst.h"
#include "../ViewModel/VmOrder.h"
namespace DarkHorse {
	struct Order;
	class SmAccount;
	class SmSymbol;
	class SmFund;
	class OrderControl
	{
	public:
		OrderControl();
		~OrderControl();
		void update_order(std::shared_ptr<Order> order, OrderEvent order_event);
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
		SubOrderControl& get_buy_order_control() {
			return buy_order_control_;
		}
		SubOrderControl& get_sell_order_control() {
			return sell_order_control_;
		}
		void clear();
		//void add_account_id(const int account_id);
		void set_account(std::shared_ptr<SmAccount> account);
		void set_fund(std::shared_ptr<SmFund> fund);
		void set_symbol(std::shared_ptr<SmSymbol> symbol);
		std::pair<int, int> get_order_count(const SmPositionType& position, const int price);
		//std::shared_ptr<PriceOrderMap> get_order_map(const SmPositionType& position, const int price);
		void get_order(const SmPositionType& position, const int& price, std::vector<std::shared_ptr<Order>>& order_vector);
		void clear_order(const SmPositionType& position, const int& price);
		void load_from_account(const bool is_sub_account, const std::string& account_no, const std::string& symbol_code);
		void load_from_fund(const std::string& fund_name, const std::string& symbol_code);
	private:
		void add_order(const std::map<std::string, std::shared_ptr<Order>>& accepted_order_map);
		void add_order(const std::vector<std::shared_ptr<Order>>& accepted_order_vector);
		void add_order(const std::string& account_no, const std::string& symbol_code);
		void add_order(std::shared_ptr<Order> order);
		void on_order_unfilled(std::shared_ptr<Order> order);
		void on_order_accepted(std::shared_ptr<Order> order);
		void remove_order(std::shared_ptr<Order> order);
		void remove_order(const std::string& order_no);
		std::pair<int, int> get_order_count(SubOrderControl& order_control, const int price);
		SubOrderControl buy_order_control_;
		SubOrderControl sell_order_control_;
		//int symbol_id_{ 0 };
		std::shared_ptr<SmSymbol> symbol_{nullptr};
		std::shared_ptr<SmAccount> account_{ nullptr };
		std::shared_ptr<SmFund> fund_{ nullptr };
		int id_{ 0 };
		// the map of account no of which belongs to the account.
		// key : account no, value : account object
		std::map<std::string, std::shared_ptr<SmAccount>> account_map_;
		std::function<void()> event_handler_;
	};
}

