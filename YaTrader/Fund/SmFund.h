#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <set>
#include "../Xml/pugixml.hpp"
namespace DarkHorse {
	class SmAccount;
	struct Position;
	class SmFund
	{
	public:
		SmFund();
		 SmFund(const int& id, const std::string& name) : _id(id), _Name(name) {};
		~SmFund() {};
		std::string Name() const { return _Name; }
		void Name(std::string val) { _Name = val; }
		int Id() const { return _id; }
		void Id(int val) { _id = val; }
		std::shared_ptr<SmAccount> FindAccount(const std::string& account_no);
		void AddAccount(const std::shared_ptr<SmAccount>& account);
		void RemoveAccount(const std::string& account_no);
		void RemoveAllAccounts();
		const std::vector<std::shared_ptr<SmAccount>>& GetAccountVector() {
			return _AccountVector;
		}
		size_t GetAccountCount() {
			return _AccountVector.size();
		}
		int GetRatioPercent(const std::string account_no);
		int GetTotalSeungSu();

		void MoveUp(const int& target_index);
		void MoveDown(const int& target_index);
		int GetPositionCount(const std::string& symbol_code);
		int GetFilledOrderCount(const std::string& symbol_code);
		void SaveToXml(pugi::xml_node& node);
		void LoadFromXml(pugi::xml_node& node);
		std::string fund_type() const { return fund_type_; }
		void fund_type(std::string val) { fund_type_ = val; }
		bool is_account_exist(const int& account_id);
		std::shared_ptr<Position> position() const { return position_; }
		std::shared_ptr<SmAccount> get_first_account();
	private:
		std::shared_ptr<Position> position_{ nullptr };
		// key : account no, value : account object
		std::vector<std::shared_ptr<SmAccount>> _AccountVector;
		std::string _Name;
		int _id{ -1 };
		/// "1" : abroad, "9" : domestic
		std::string fund_type_{ "9" };
		std::set<int> account_id_set_;
	};
}

