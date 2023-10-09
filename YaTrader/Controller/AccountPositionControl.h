#pragma once
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <set>
#include <mutex>
#include <vector>
#include "../Order/SmOrderConst.h"
namespace DarkHorse {
struct Position;
struct SmQuote;
using position_p = std::shared_ptr<Position>;
using quote_p = std::shared_ptr<SmQuote>;
class SmFund;
class SmAccount;
class AccountPositionControl
{
public:
	AccountPositionControl();
	~AccountPositionControl();
	void update_position(position_p position);
	void update_profit_loss(quote_p quote);
	void set_event_handler(std::function<void(const int result)> event_handler) {
		event_handler_ = event_handler;
	}
	void set_single_position_event_handler(std::function<void(const int)> event_handler) {
		single_position_event_handler_ = event_handler;
	}
	const std::map<std::string, position_p>& get_active_position_map();
	void set_account(std::shared_ptr<SmAccount> account);
	void set_fund(std::shared_ptr<SmFund> fund);
private:
	void reset_account_position();
	std::mutex mutex_; // Mutex for thread synchronization
	void add_position(position_p position);
	void trigger_event(const int result);
	void load_position();
	position_p get_position(const std::string& symbol_code);
	// key : symbol code, value : position object.
	std::map<std::string, position_p> position_map_;
	//std::vector<position_p> active_position_vector_;
	std::function<void(const int result)> event_handler_;
	std::function<void(const int)> single_position_event_handler_;
	int id_{ 0 };
	std::string account_no_;
	std::set<std::string> account_no_set_;
	std::shared_ptr<SmAccount> account_;
	std::shared_ptr<SmFund> fund_;
	OrderType position_type_{ OrderType::None };
};
}
