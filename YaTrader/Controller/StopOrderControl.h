#pragma once
#include <map>
#include <string>
#include <memory>
#include <mutex>
#include <functional>
#include "../Order/SmOrderConst.h"

namespace DarkHorse {
struct OrderRequest;
class SmAccount;
class SmSymbol;
struct SmQuote;
using account_p = std::shared_ptr<SmAccount>;
using symbol_p = std::shared_ptr<SmSymbol>;
using order_request_p = std::shared_ptr<OrderRequest>;

class PriceOrderRequestMap {
private:
	int price_{ 0 };
	size_t count_{ 0 };
	// key : request id, value : accepted order object.
	std::map<int, order_request_p> order_request_map_;
public:
	void clear() {
		order_request_map_.clear();
		count_ = 0;
	}
	int get_price() const { return price_; }
	void set_price(const int price) {
		price_ = price;
	}
	size_t count() const { return count_; }
	const std::map<int, order_request_p>& get_order_request_map() {
		return order_request_map_;
	}
	void add_order_request(order_request_p order_request);
	void remove_order_request(const int request_id);
};

using price_order_request_map_p = std::shared_ptr<PriceOrderRequestMap>;
struct Order;
using order_p = std::shared_ptr<Order>;

class StopOrderControl
{
public:
	void clear();
	void set_stop_as_real_order(bool enable) {
		stop_as_real_order_ = enable;
	}
	StopOrderControl();
	~StopOrderControl();
	void set_control_type(const SmPositionType control_type) {
		control_type_ = control_type;
	}
	int total_count() const { return total_count_; }
	const std::map<int, price_order_request_map_p>& get_order_req_map() {
		return order_req_map_;
	}
	void add_stop_order_request(const int order_price, order_request_p order_request);
	void set_event_handler(std::function<void()> event_handler) {
		event_handler_ = event_handler;
	}
	void add_stop_order_request
	(
		account_p account,
		symbol_p symbol,
		const DarkHorse::SmPositionType& type, 
		const int price,
		const int order_amount,
		const int cut_slip
	);
	void set_symbol_id(const int symbol_id);
	void remove_stop_order_request(const int order_price);
	std::pair<int, int> get_order_count(const int order_price);
	price_order_request_map_p get_price_order_req_map(const int price);
	void set_order_control_id(const int id) {
		order_control_id_ = id;
	}
private:
	bool stop_as_real_order_{false};
	int symbol_id_{ 0 };
	int symbol_int_tick_size_{ 1 };
	int order_control_id_{ 0 };
	int id_{ 0 };
	std::mutex mutex_;
	SmPositionType control_type_{ CT_NONE };
	size_t total_count_{ 0 };
	// key : price as integer, value : order request list on the price. 
	std::map<int, price_order_request_map_p> order_req_map_;
	void calculate_total_count();
	order_request_p make_profit_cut_order_request(order_p order);
	order_request_p make_loss_cut_order_request(order_p order);
	void on_cut_stop_order_request(order_p order);
	void update_quote(std::shared_ptr<SmQuote> quote);
	void check_stop_order_request(std::shared_ptr<SmQuote> quote);
	void remove_stop_order_request(const int order_price, const int& request_id);
	std::function<void()> event_handler_;
};
}

