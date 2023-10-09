#pragma once
#include <string>
#include <map>
#include <memory>
#include "../../Json/json.hpp"
#include "../SmOrderConst.h"
namespace DarkHorse {
using order_event = nlohmann::json;
struct Order;
struct OrderRequest;

using order_request_p = std::shared_ptr<OrderRequest>;
using order_p = std::shared_ptr<Order>;

class AccountOrderManager;
class TotalOrderManager
{
	using account_order_manager_p = std::shared_ptr<AccountOrderManager>;
public:
	//void on_order_event(const order_event& order_info);
	void on_order_event(order_event&& order_info);
	account_order_manager_p get_account_order_manager(const std::string& account_no);
	const std::map<std::string, account_order_manager_p>& get_account_order_manager_map() {
		return account_order_manager_map_;
	}
	std::pair<bool, int> get_init_and_acpt_order_count_from_account(const std::string& account_no, const std::string& symbol_code);
	std::pair<bool, int> get_init_and_acpt_order_count_from_fund(const std::string& fund_name, const std::string& symbol_code);
	std::pair<bool, int> get_init_and_acpt_order_count_from_parent_account(const std::string& account_no, const std::string& symbol_code);
private:
	void write_order_history(const OrderEvent order_event, order_p order) ;
	void dispatch_order(const OrderEvent order_event, order_p order) ;
	order_p get_order(const std::string& order_no) ;
	order_p find_order(const std::string& order_no) ;
	order_p create_order(const std::string& order_no);
	order_p make_order(const order_event& order_info);
	void set_order_request_info(const std::string& custom_info, order_p order);
	account_order_manager_p find_account_order_manager(const std::string& account_no);
	account_order_manager_p create_account_order_manager(const std::string& account_no);
	/// <summary>
	/// key : order_no, value : order_object.
	/// </summary>
	std::map<std::string, order_p> order_map_;
	/// <summary>
	/// key : account_no, value : account_order_manager object.
	/// </summary>
	std::map<std::string, account_order_manager_p> account_order_manager_map_;
};
}
