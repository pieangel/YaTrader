#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include "../Json/json.hpp"
namespace DarkHorse {
	class SmAccount;
	// ���� ������
	class SmAccountManager
	{
	public:
		// ������
		SmAccountManager();
		// �Ҹ���
		~SmAccountManager();
		// Static Members
		static int _Id;
		static int GetId() { return _Id++; }
		/// <summary>
		/// ���¸� �߰��Ѵ�.
		/// </summary>
		/// <param name="account">���� ��ü</param>
		void AddAccount(std::shared_ptr<SmAccount> account);
		void RemoveAccount(const std::string& account_no);
		/// <summary>
		/// ���� ����� �����ɴϴ�.
		/// </summary>
		/// <returns></returns>
		const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& GetAccountMap() {
			return _AccountMap;
		}
		void get_main_account_vector(std::vector<std::shared_ptr<SmAccount>>& main_acnt_vector);
		void get_main_account_vector(const std::string& account_type, std::vector<std::shared_ptr<SmAccount>>& main_acnt_vector);
		void GetUnusedAccountForFund(const std::string& account_type, std::vector<std::shared_ptr<SmAccount>>& unused_acnt_vector);
		std::shared_ptr<SmAccount> FindAccount(const std::string& account_no);
		std::shared_ptr<SmAccount> FindAccountById(const int& account_id);
		void AddTestAccounts();
		void SetPassword(const std::string& account_no, const std::string& pwd);
		void on_account_asset(nlohmann::json&& arg);
		void register_accounts();
		std::string get_password(const std::string& account_no);
		std::string get_account_no(const std::string& account_no);
		std::shared_ptr<SmAccount> get_parent_account(const std::string& account_no);
		std::shared_ptr<SmAccount> find_main_account(const std::string& account_no);
		int get_account_id(const std::string& account_no);
	private:
		
		/// <summary>
		/// Account map
		/// Key : Account No, Value : Account Object Pointer.
		/// </summary>
		std::unordered_map<std::string, std::shared_ptr<SmAccount>> _AccountMap;
		// key : account id, value : account object
		std::map<int, std::shared_ptr<SmAccount>> _AccountIdMap;
	};
}

