#include "stdafx.h"
#include "SmAccount.h"
#include "../Util/IdGenerator.h"
#include "../Global/SmTotalManager.h"
#include "SmAccountManager.h"
#include "../Position/Position.h"
namespace DarkHorse {

	void SmAccount::move_up_sub_account(const int& target_index)
	{

	}

	void SmAccount::move_down_sub_account(const int& target_index)
	{

	}

	void SmAccount::remove_sub_account(const std::string& account_no)
	{
		auto it = std::find_if(sub_accounts_.begin(), sub_accounts_.end(), [&](const std::shared_ptr<SmAccount>& acnt) {
			return acnt->No() == account_no;
		});
		if (it != sub_accounts_.end()) {
			sub_accounts_.erase(it);
		}
	}

	void SmAccount::AddSubAccount(const std::shared_ptr<SmAccount>& subAccount)
	{
		// Check if the account number is unique before adding
		if (!IsAccountNumberUnique(subAccount->No()))
		{
			// Handle the duplication case (e.g., throw an exception or return an error status)
			AfxMessageBox("Account number already exists.");
		}

		sub_accounts_.push_back(subAccount);

		mainApp.AcntMgr()->AddAccount(subAccount);
	}

	/*
	sub_account->Name(_Name);
			std::string sub_account_no = _No + "_" + std::to_string(i+1);
			sub_account->No(sub_account_no);
			sub_account->Type(_Type);
			sub_account->Pwd(_pwd);
			sub_account->is_subaccount(true);
			sub_account->parent_id(_id);
	*/

	std::shared_ptr<SmAccount> SmAccount::CreateSubAccount(const std::string& acntNo, const std::string& acntName, int parent_id, const std::string& type)
	{
		std::shared_ptr<SmAccount> acnt = std::make_shared<SmAccount>();
		acnt->parent_id(parent_id);
		acnt->Type(type);
		acnt->No(acntNo);
		acnt->Name(acntName);
		acnt->is_subaccount(true);
		acnt->parent_id(_id);
		acnt->UsedForFund(false);
		acnt->parent_account(shared_from_this());
		sub_accounts_.push_back(acnt);
		mainApp.AcntMgr()->AddAccount(acnt);
		return acnt;
	}

	void SmAccount::make_default_sub_account()
	{
		if (!sub_accounts_.empty()) return;
		for (int i = 0; i < 10; i++) {
			const std::string& sub_account_no = _No + "_" + std::to_string(i+1);
			auto sub_account = CreateSubAccount(sub_account_no, _Name, _id, _Type);
		}
	}

	SmAccount::SmAccount()
		: _id(IdGenerator::get_id())
	{
		position_ = std::make_shared<Position>();
	}
	SmAccount::~SmAccount()
	{

	}
}


