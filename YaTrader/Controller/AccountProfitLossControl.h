#pragma once
#include <map>
#include <string>
#include <memory>
#include <set>
#include <functional>
#include "../ViewModel/VmAccountProfitLoss.h"
#include "../Account/SmAccount.h"
#include "../Order/SmOrderConst.h"
namespace DarkHorse {

	struct AccountProfitLoss;
	using account_profit_loss_p = std::shared_ptr<AccountProfitLoss>;

	struct Position;
	struct SmQuote;
	class SmAccount;
	class SmFund;
	using position_p = std::shared_ptr<Position>;
	using quote_p = std::shared_ptr<SmQuote>;
	class AccountProfitLossControl
	{
	public:
		AccountProfitLossControl();
		~AccountProfitLossControl();
		void update_position(position_p position);
		void update_profit_loss(quote_p quote);
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
		
		const VmAccountProfitLoss& get_account_profit_loss() {
			return account_profit_loss_;
		}

		void set_account(std::shared_ptr<DarkHorse::SmAccount> account);
		void set_fund(std::shared_ptr<SmFund> fund);
		void load_profit_loss();
	private:
		void reset_account_profit_loss();
		position_p get_position(const std::string& symbol_code);
		void update_account_profit_loss();
		std::set<std::string> account_no_set_;
		std::function<void()> event_handler_;
		int id_{ 0 };
		int account_id_{ 0 };
		std::shared_ptr<DarkHorse::SmAccount> account_{nullptr};
		VmAccountProfitLoss account_profit_loss_;
		std::shared_ptr<SmFund> fund_;
		// key : symbol code, value : position object.
		std::map<std::string, position_p> position_map_;
		OrderType position_type_{ OrderType::None };
	};
}
