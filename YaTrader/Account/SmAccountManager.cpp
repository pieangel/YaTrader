#include "stdafx.h"
#include "SmAccountManager.h"
#include "SmAccount.h"
#include "../Log/MyLogger.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Client/ViClient.h"
namespace DarkHorse {

	int SmAccountManager::_Id = 0;

	SmAccountManager::SmAccountManager()
	{
		//AddTestAccounts();
	}

	SmAccountManager::~SmAccountManager()
	{

	}

	void SmAccountManager::AddAccount(std::shared_ptr<SmAccount> account)
	{
		if (!account) return;
		auto old_account = FindAccount(account->No());
		if (old_account) return;

		// Set the id for the account
		_AccountMap[account->No()] = account;
		_AccountIdMap[account->id()] = account;
	}

	void SmAccountManager::RemoveAccount(const std::string& account_no)
	{
		auto found = _AccountMap.find(account_no);
		if (found != _AccountMap.end()) {
			_AccountMap.erase(found);
		}
	}

	void SmAccountManager::get_main_account_vector(std::vector<std::shared_ptr<SmAccount>>& main_acnt_vector)
	{
		main_acnt_vector.clear();
		for (auto it = _AccountMap.begin(); it != _AccountMap.end(); it++) {
			if (!it->second->is_subaccount())
				main_acnt_vector.push_back(it->second);
		}
	}

	void SmAccountManager::get_main_account_vector(const std::string& account_type, std::vector<std::shared_ptr<SmAccount>>& main_acnt_vector)
	{
		main_acnt_vector.clear();
		for (auto it = _AccountMap.begin(); it != _AccountMap.end(); it++) {
			if (it->second->Type() == account_type && !it->second->is_subaccount())
				main_acnt_vector.push_back(it->second);
		}
	}

	void SmAccountManager::GetUnusedAccountForFund(const std::string& account_type, std::vector<std::shared_ptr<SmAccount>>& unused_acnt_vector)
	{
		std::vector<std::shared_ptr<SmAccount>> main_acnt_vector;
		get_main_account_vector(account_type, main_acnt_vector);
		for (auto it = main_acnt_vector.begin(); it != main_acnt_vector.end(); it++) {
			if (!(*it)->UsedForFund()) unused_acnt_vector.push_back(*it);
			auto sub_acnt_vector = (*it)->get_sub_accounts();
			for (auto it2 = sub_acnt_vector.begin(); it2 != sub_acnt_vector.end(); it2++) {
				if (!(*it2)->UsedForFund()) {
					unused_acnt_vector.push_back(*it2);
				}
			}
		}
	}

	std::shared_ptr<SmAccount> SmAccountManager::FindAccount(const std::string& account_no)
	{
		auto found = _AccountMap.find(account_no);
		if (found != _AccountMap.end())
			return found->second;
		else
			return nullptr;
	}

	std::shared_ptr<SmAccount> SmAccountManager::FindAccountById(const int& account_id)
	{
		auto found = _AccountIdMap.find(account_id);
		if (found != _AccountIdMap.end())
			return found->second;
		else
			return nullptr;
	}

	void DarkHorse::SmAccountManager::AddTestAccounts()
	{
		for (int i = 0; i < 10; i++) {
			std::shared_ptr<SmAccount> account = std::make_shared<SmAccount>();
			std::string account_no("00370");
			account_no.append(std::to_string(i));
			account->No(account_no);
			std::string user("user");
			user.append(std::to_string(i + 1));
			account->Name(user);
			account->Type("1");
			AddAccount(account);
		}
	}

	void DarkHorse::SmAccountManager::SetPassword(const std::string& account_no, const std::string& pwd)
	{
		auto found = _AccountMap.find(account_no);
		if (found == _AccountMap.end()) return;

		found->second->Pwd(pwd);
	}

	void SmAccountManager::on_account_asset(nlohmann::json&& arg)
	{
		const std::string& account_no = arg["account_no"];
		const auto account = FindAccount(account_no);
		if (!account) return;

		try {
			account->Asset.EntrustTotal = arg["entrust_total"];
			account->Asset.Balance = arg["balance"];
			account->Asset.OutstandingDeposit = arg["outstanding_deposit"];
			account->Asset.OrderDeposit = arg["order_deposit"];
			account->Asset.EntrustDeposit = arg["entrust_deposit"];
			account->Asset.MaintenanceMargin = arg["maintenance_margin"];
			account->Asset.TradeProfitLoss = arg["settled_profit_loss"];
			account->Asset.Fee = arg["fee"];
			account->Asset.OpenProfitLoss = arg["open_profit_loss"];
			account->Asset.OpenTrustTotal = arg["open_trust_total"];
			account->Asset.AdditionalMargin = arg["additional_margin"];
			account->Asset.OrderMargin = arg["order_margin"];
		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	void SmAccountManager::register_accounts()
	{
		for (auto it = _AccountMap.begin(); it != _AccountMap.end(); it++) {
			if (it->second->is_subaccount()) continue;
			mainApp.Client()->RegisterAccount(it->second->No());
		}
	}

	std::string SmAccountManager::get_password(const std::string& account_no)
	{
		std::string pwd;

		auto found = _AccountMap.find(account_no);
		if (found != _AccountMap.end()) {
			if (found->second->is_subaccount()) {
				auto parent_account = found->second->parent_account().lock();
				if (parent_account)
					pwd = parent_account->Pwd();
			}
			else
				pwd = found->second->Pwd();
		}

		return pwd;
	}

	std::string SmAccountManager::get_account_no(const std::string& account_no)
	{
		std::string real_account_no;

		auto found = _AccountMap.find(account_no);
		if (found != _AccountMap.end()) {
			if (found->second->is_subaccount()) {
				auto parent_account = found->second->parent_account().lock();
				if (parent_account)
					real_account_no = parent_account->No();
			}
			else
				real_account_no = found->second->No();
		}

		return real_account_no;
	}

	std::shared_ptr<DarkHorse::SmAccount> SmAccountManager::get_parent_account(const std::string& account_no)
	{
		auto found = _AccountMap.find(account_no);
		if (found != _AccountMap.end()) {
			if (found->second->is_subaccount()) {
				return found->second->parent_account().lock();
			}
			else
				return nullptr;
		}
		return nullptr;
	}

	std::shared_ptr<DarkHorse::SmAccount> SmAccountManager::find_main_account(const std::string& account_no)
	{
		auto found = _AccountMap.find(account_no);
		if (found == _AccountMap.end()) return nullptr;
		if (found->second->is_subaccount() || !found->second->is_server_side()) {
			return nullptr;
		}
		else
			return found->second;
	}

	int SmAccountManager::get_account_id(const std::string& account_no)
	{
		auto found = _AccountMap.find(account_no);
		if (found == _AccountMap.end()) return -1;
		return found->second->id();
	}

}
