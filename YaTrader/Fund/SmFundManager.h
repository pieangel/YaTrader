#pragma once
#include <memory>
#include <map>
#include <string>
#include "../Xml/pugixml.hpp"
namespace DarkHorse {
	class SmFund;
	class SmFundManager
	{
	public:
		// Static Members
		static int _Id;
		static int GetId() { return _Id++; }

		SmFundManager();
		~SmFundManager() {};
		std::shared_ptr<SmFund> FindAddFund(const std::string& fund_name, const std::string fund_type);
		void RemoveFund(const std::string& fund_name);
		void Clear() {
			_FundMap.clear();
		}
		void Init();
		const std::map<std::string, std::shared_ptr<SmFund>>& GetFundMap() {
			return _FundMap;
		}
		std::shared_ptr<SmFund> FindFundById(const int& fund_id);
		std::shared_ptr<SmFund> FindFund(const std::string& fund_name);
		size_t GetFundCount() {
			return _FundMap.size();
		}
		void SaveToXml(pugi::xml_node& node);
		void LoadFromXml(pugi::xml_node& node);
	private:
		// key : fund name, value : fund object
		std::map<std::string, std::shared_ptr<SmFund>> _FundMap;
		// key : fund id, value : fund object
		std::map<int, std::shared_ptr<SmFund>> _FundIdMap;
	};
}

