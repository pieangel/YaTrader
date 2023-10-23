#pragma once
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include "../Json/json.hpp"
#include "../ViewModel/VmPosition.h"
namespace DarkHorse {
class AccountPositionManager;
class GroupPositionManager;
struct Order;
struct SmQuote;
using order_p = std::shared_ptr<Order>;
using quote_p = std::shared_ptr<SmQuote>;
struct Position;
using account_position_manager_p = std::shared_ptr<AccountPositionManager>;
using group_position_manager_p = std::shared_ptr<GroupPositionManager>;
using position_p = std::shared_ptr<Position>;

class TotalPositionManager
{
public:
	position_p get_position(const std::string& account_no, const std::string& symbol_code);

	void get_position_from_fund(const std::string& fund_name, const std::string& symbol_code, VmPosition& position);
	void get_position_from_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position);
	void get_position_from_parent_account(const std::string& account_no, const std::string& symbol_code, VmPosition& position);

	std::shared_ptr<Position> get_position_from_account(const std::string& account_no, const std::string& symbol_code);
	account_position_manager_p get_account_position_manager(const std::string& account_no);
	

	/// <summary>
	/// 순서는 update_position->update_account_position->update_fund_position로 진행한다. 
	/// 반드시 이 순서를 지켜야 한다. 그리고 모든 주문에 대하여 이 세개의 함수를 모두 불러야 하는지 
	/// 확인하고 불러야 된다면 반드시 순서를 지켜서 불러야 한다.
	/// </summary>
	/// <param name="order"></param>
	void update_position(order_p order);
	void update_position(quote_p quote);
	void on_symbol_position(nlohmann::json&& arg);
	void on_symbol_profit_loss(nlohmann::json&& arg);
	void on_trade_profit_loss(nlohmann::json&& arg);
	static double calculate_symbol_open_profit_loss
	(
		const int& position_open_quantity,
		const int& symbol_close,
		const double& position_average_price,
		const int& symbol_seungsu,
		const int& symbol_decimal
	);
	static void calculate_symbol_open_profit_loss(const std::shared_ptr<Position>& position);
	static void calculate_symbol_open_profit_loss(VmPosition& position);
	static void set_symbol_id(position_p position, const std::string& symbol_code);
	static void set_account_id(position_p position, const std::string& account_no);
	void update_account_profit_loss(const std::string& account_no);
	position_p find_position_by_id(const int& position_id);
	void update_group_position(std::shared_ptr<Position> position);
	group_position_manager_p find_fund_group_position_manager(const std::string& fund_name);
	group_position_manager_p find_account_group_position_manager(const std::string& account_no);
	account_position_manager_p find_position_manager(const std::string& account_no);
	void update_trade_profit_loss(const std::string& account_no, const double trade_profit_loss, const double trade_fee);
private:
	//std::mutex mutex_; // Mutex for thread synchronization
	group_position_manager_p find_add_account_group_position_manager(const std::string& account_no);
	group_position_manager_p create_account_group_position_manager(const std::string& account_no);
	group_position_manager_p find_add_fund_group_position_manager(const std::string& fund_name);
	group_position_manager_p create_fund_group_position_manager(const std::string& fund_name);
	group_position_manager_p find_add_group_position_manager(std::shared_ptr<Position> position);
	group_position_manager_p create_group_position_manager(std::shared_ptr<Position> position);
	// key : position id, value : position object.
	std::map<int, position_p> position_map_;
	
	account_position_manager_p create_position_manager(const std::string& account_no);
	// key : account_no, value : account position manager object.
	std::map<std::string, account_position_manager_p> account_position_manager_map_;
	// key : account no, value : group position manager object.
	std::map<std::string, group_position_manager_p> account_group_position_manager_map_;
	// key : fund name, value : group position manager object.
	std::map<std::string, group_position_manager_p> fund_group_position_manager_map_;
};
}

