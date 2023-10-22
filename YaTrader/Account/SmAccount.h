#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "SmAccountAsset.h"
namespace DarkHorse {
	/// <summary>
	/// 계좌 클래스
	/// 계좌 정보를 가지고 있습니다.
	/// </summary>
	struct Position;
	class SmAccount : public std::enable_shared_from_this<SmAccount>
	{
	private:
		/// <summary>
		/// 고유 아이디
		/// </summary>
		int _id{ -1 };
		int parent_id_{ -1 };
		/// <summary>
		/// 계좌 이름
		/// </summary>
		std::string _Name;
		/// <summary>
		/// 계좌 번호
		/// </summary>
		std::string _No;
		/// <summary>
		/// 계좌 타입
		/// 1 : abroad, 9 : domestic
		/// </summary>
		std::string _Type;
		/// <summary>
		/// 비밀 번호
		/// </summary>
		std::string _pwd;
		
		// 0 : not confirmed, 1 : confirmed.
		int _Confirm{ 0 };
		// used for the fund
		bool _UsedForFund = false;
		int  _SeungSu = 1;
		double _Ratio = 0;

		bool is_sub_account_ = false;
		std::vector<std::shared_ptr<SmAccount>> sub_accounts_;
		

		std::shared_ptr<Position> position_{nullptr};
		std::weak_ptr<SmAccount> parent_account_;
		std::string fund_name_;
		bool is_server_side_{ false };
		bool skip_confirm_{ false };
	public:
		bool skip_confirm() const { return skip_confirm_; }
		void skip_confirm(bool val) { skip_confirm_ = val; }
		bool is_server_side() const { return is_server_side_; }
		void is_server_side(bool val) { is_server_side_ = val; }

		std::weak_ptr<DarkHorse::SmAccount> parent_account() const { return parent_account_; }
		void parent_account(std::weak_ptr<DarkHorse::SmAccount> val) { parent_account_ = val; }

		// Function to check if a given account number already exists in the sub-accounts
		bool IsAccountNumberUnique(const std::string& account_no)
		{
			std::shared_ptr<SmAccount> found_account = find_sub_account_by_no(account_no);
			return found_account ? false : true;
		}

		// Function to find an account by its No
		std::shared_ptr<SmAccount> find_sub_account_by_no( const std::string& accountNo) {
			auto it = std::find_if(sub_accounts_.begin(), sub_accounts_.end(), [accountNo](const std::shared_ptr<SmAccount>& account) {
				return account->No() == accountNo;
				});

			if (it != sub_accounts_.end()) {
				return *it; // Return the found shared pointer to SmAccount
			}

			return nullptr; // Return nullptr if the account is not found
		}

		void move_up_sub_account(const int& target_index);
		void move_down_sub_account(const int& target_index);
		void remove_sub_account(const std::string& account_no);
		std::shared_ptr<Position> position() const { return position_; }
		int parent_id() const { return parent_id_; }
		void parent_id(int val) { parent_id_ = val; }
		// Function to get the list of sub-accounts
		const std::vector<std::shared_ptr<SmAccount>>& get_sub_accounts() const { return sub_accounts_; }
		int get_sub_account_count() const { return sub_accounts_.size(); }
		// Function to add a new sub-account to the list
		void AddSubAccount(const std::shared_ptr<SmAccount>& subAccount);
		std::shared_ptr<SmAccount> CreateSubAccount(const std::string& acntNo, const std::string& acntName, int parent_id, const std::string& type);
		
		void make_default_sub_account();
		SmAccount();
		~SmAccount();
		int Confirm() const { return _Confirm; }
		void Confirm(int val) { _Confirm = val; }
		std::string Name() const { return _Name; }
		void Name(std::string val) { _Name = val; }
		std::string Type() const { return _Type; }
		void Type(std::string val) { _Type = val; }
		std::string No() const { return _No; }
		void No(std::string val) { _No = val; }
		std::string Pwd() const { return _pwd; }
		void Pwd(std::string val) { _pwd = val; }

		SmAccountAsset Asset;
		int id() const { return _id; }
		bool UsedForFund() const { return _UsedForFund; }
		void UsedForFund(bool val) { _UsedForFund = val; }
		int SeungSu() const { return _SeungSu; }
		void SeungSu(int val) { _SeungSu = val; }
		double Ratio() const { return _Ratio; }
		void Ratio(double val) { _Ratio = val; }
		bool is_subaccount() const { return is_sub_account_; }
		void is_subaccount(bool val) { is_sub_account_ = val; }
		std::string fund_name() const { return fund_name_; }
		void fund_name(std::string val) { fund_name_ = val; }
	};
}

