#pragma once
#include <string>
#include <memory>
#include <map>
#include <list>
namespace DarkHorse {
	class SmSymbolOrderManager;
	class SmSymbol;
	class SmOrder;
	struct SmFilledInfo;
	class SmAccountOrderManager
	{
	public:
		SmAccountOrderManager(const std::string account_no)
			: _AccountNo(account_no) {};
		~SmAccountOrderManager() {};
		std::shared_ptr<SmSymbolOrderManager> FindAddSymbolOrderManager(const std::string& symbol_code);
		const std::map<std::string, std::shared_ptr<SmSymbolOrderManager>>& GetSymbolOrderMgrMap() {
			return _SymbolOrderManagerMap;
		}
		void CheckStopOrder(std::shared_ptr<SmSymbol> symbol);
		const std::map<std::string, std::shared_ptr<DarkHorse::SmOrder>>& GetFilledMap() {
			return _FilledMap;
		}

		void AddFilledList(std::shared_ptr<DarkHorse::SmOrder> filled_order);
		void AddFilledInfo(std::shared_ptr<DarkHorse::SmOrder> filled_order);
		const std::list<DarkHorse::SmFilledInfo>& GetFilledInfo() {
			return _FilledInfoList;
		}
	private:
		std::list<DarkHorse::SmFilledInfo> _FilledInfoList;
		std::map<std::string, std::shared_ptr<DarkHorse::SmOrder>> _FilledMap;
		std::string _AccountNo;
		// Key : symbol code, value : Symbol Order Manager Object.
		std::map<std::string, std::shared_ptr<SmSymbolOrderManager>> _SymbolOrderManagerMap;
	};
}

