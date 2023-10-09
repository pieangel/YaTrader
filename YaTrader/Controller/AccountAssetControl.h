#pragma once

#pragma once
#include <map>
#include <string>
#include <memory>
#include <functional>
#include "../ViewModel/VmAsset.h"
namespace DarkHorse {
	class SmAccount;
	class SmFund;
	class AccountAssetControl
	{
	public:
		AccountAssetControl();
		~AccountAssetControl();
		void load_position_from_parent_account(const std::string& account_no);
		void load_position_from_fund(const std::string& fund_name);
		void load_asset_from_account(const std::string& account_no);
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
		const VmAsset& get_asset() {
			return asset_;
		}
		void set_account(std::shared_ptr<SmAccount> account);
		void set_fund(std::shared_ptr<SmFund> fund);
	private:
		void reset();
		void load_account_asset(std::shared_ptr<DarkHorse::SmAccount> account);
		void load_asset_from_account(std::shared_ptr<DarkHorse::SmAccount> account);
		std::shared_ptr<DarkHorse::SmAccount> account_{ nullptr };
		std::shared_ptr<SmFund> fund_;

		std::function<void()> event_handler_;
		int id_{ 0 };
		VmAsset asset_;
	};
}
