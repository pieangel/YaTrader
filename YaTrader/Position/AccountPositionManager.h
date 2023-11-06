#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <mutex>
#include "../ViewModel/VmPosition.h"
#include "../ViewModel/VmAccountProfitLoss.h"
namespace DarkHorse {
	struct Position;
	struct SmQuote;
	struct AccountProfitLoss;
	using quote_p = std::shared_ptr<SmQuote>;
	using position_p = std::shared_ptr<Position>;
	using account_profit_loss_p = std::shared_ptr<AccountProfitLoss>;
	struct Order;
	struct OrderRequest;
	using order_p = std::shared_ptr<Order>;
	using order_request_p = std::shared_ptr<OrderRequest>;
	class TotalPositionManager;
	//struct GroupPosition;
class AccountPositionManager
{
public:
	AccountPositionManager(TotalPositionManager& total_position_manager, const std::string& account_no);
	~AccountPositionManager();
	position_p get_position(const std::string& symbol_code);
	void update_position(order_p order);
	void update_position(quote_p quote);
// 	const std::map<std::string, position_p>& get_position_map() {
// 		return position_map_;
// 	}
	void get_account_profit_loss(VmAccountProfitLoss& dest_account_profit_loss);
	void update_account_profit_loss();
	position_p find_position(const std::string& symbol_code);
	void update_position(position_p position);
	void update_position(position_p position, VmPosition& dest_position);
	void get_active_positions(std::map<std::string, position_p>& position_vector);
	void update_trade_profit_loss(const double trade_profit_loss, const double trade_fee);
private:
	std::mutex mutex_; // Mutex for thread synchronization
	void set_symbol_id(position_p position, const std::string& symbol_code);
	void set_account_id(position_p position, const std::string& account_no);
	int calculate_position_count(order_p order, position_p position);
	int calculate_unsettled_count(order_p order, position_p position);
	int calculate_traded_count(order_p order, position_p position);
	double calculate_traded_profit_loss(order_p order, position_p position, const int& symbol_decimal, const int& symbol_seungsu);
	double calculate_average_price(order_p order, position_p position, const int& position_count, const int& unsettled_count);
	void update_open_profit_loss(position_p position);
	position_p create_position(const std::string& symbol_code);
	// key : symbol coode, value : position object.
	std::map<std::string, position_p> position_map_;
	account_profit_loss_p account_profit_loss_{ nullptr };
	int id_{ 0 };
	std::string account_no_;
	TotalPositionManager& total_position_manager_;
};
}

