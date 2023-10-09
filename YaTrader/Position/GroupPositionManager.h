#pragma once
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include "../Order/SmOrderConst.h"
#include "../ViewModel/VmPosition.h"
#include "../ViewModel/VmAccountProfitLoss.h"
namespace DarkHorse {
	class TotalPositionManager;
	struct GroupPosition;
	struct Position;
	using position_p = std::shared_ptr<Position>;
	struct AccountProfitLoss;
	using account_profit_loss_p = std::shared_ptr<AccountProfitLoss>;
	struct SmQuote;
	using quote_p = std::shared_ptr<SmQuote>;
	class GroupPositionManager
	{
	public:
		GroupPositionManager(TotalPositionManager& total_position_manager);
		~GroupPositionManager();
		// update the group position according to the position. the sub account is not allowed to be a group position 
		void update_group_position(const std::shared_ptr<Position>& group_position, const std::shared_ptr<Position>& position);
		void set_fund_name(const std::string& fund_name) {
			fund_name_ = fund_name;
		}
		void set_account_no(const std::string& account_no) {
			account_no_ = account_no;
		}
		const std::map<std::string, std::shared_ptr<Position>>& get_group_position_map() {
			return group_position_map_;
		}
		void update_position(quote_p quote);
		void update_whole_group_position();
		void get_account_profit_loss(VmAccountProfitLoss& dest_account_profit_loss);
		std::shared_ptr<Position> create_account_group_position(const std::string& account_no, const std::string symbol_code);
		std::shared_ptr<Position> create_fund_group_position(const std::string& func_name, const std::string& symbol_code);
		void update_group_position_by_symbol(std::shared_ptr<Position> group_position);
		void update_group_position_by_symbol(std::shared_ptr<Position> group_position, VmPosition& dest_position);
		std::shared_ptr<Position> find_group_position(const std::string& symbol_code);
		void get_active_positions(std::map<std::string, position_p>& position_vector);
	private:

		std::mutex mutex_; // Mutex for thread synchronization
		account_profit_loss_p total_profit_loss_{ nullptr };
		std::string fund_name_;
		std::string account_no_;
		const TotalPositionManager& total_position_manager_;
		// key : symbol code, value : group position object.
		std::map<std::string, std::shared_ptr<Position>> group_position_map_;
	};
}
