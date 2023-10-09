#pragma once
#include <map>
#include <string>
#include <memory>
#include <set>
#include <functional>
#include "../Order/SmOrderConst.h"
#include <mutex>
#include <vector>
namespace DarkHorse {
struct Order;
class SmAccount;
class SmFund;
using order_p = std::shared_ptr<Order>;
using account_p = std::shared_ptr<SmAccount>;
class AccountOrderControl
{
public:
	AccountOrderControl();
	~AccountOrderControl();
	
	void update_order(order_p order, OrderEvent order_event);
	void set_event_handler(std::function<void()> event_handler) {
		event_handler_ = event_handler;
	}
	const std::map<std::string, order_p>& get_accepted_order_map() {
		std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
		return accepted_order_map_;
	}
	void get_accepted_order_vector(std::vector<order_p>& order_vec) {
		std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
		for (auto& order : accepted_order_map_) {
			order_vec.push_back(order.second);
		}
	}
	void set_account(account_p account);
	void set_fund(std::shared_ptr<SmFund> fund);
private:
	std::mutex mutex_; // Mutex for thread synchronization
	void load_order_from_account(account_p account);
	void load_order_from_fund(std::shared_ptr<SmFund> fund);
	void load_order_from_account(const std::string& account_no);
	void on_order_unfilled(std::shared_ptr<Order> order);
	void on_order_accepted(std::shared_ptr<Order> order);
	void add_accepted_order(const std::map<std::string, order_p>& accepted_order_map);
	order_p get_order(const std::string& order_no);
	void add_order(order_p order);
	void remove_order(order_p order);
	// key : order no, value : order object.
	std::map<std::string, order_p> accepted_order_map_;
	// key : account no
	std::set<std::string> account_set_;
	std::function<void()> event_handler_;
	int id_{ 0 };
	account_p account_{ nullptr };
	std::shared_ptr<SmFund> fund_{ nullptr };
};
}
