#pragma once
#include <map>
#include <string>
#include <memory>
#include "SmOrder.h"
namespace DarkHorse {
	class SmAccountOrderManager;
	class SmSymbolOrderManager;
	class SmSymbol;
	class SmTotalOrderManager
	{
	public:
		// Static Members
		static int _Id;
		static int GetId() { return ++_Id; }

		SmTotalOrderManager() {};
		~SmTotalOrderManager() {};
		std::shared_ptr<SmOrder> FindAddOrder(const std::string& order_no);
		std::shared_ptr<SmSymbolOrderManager> FindAddSymbolOrderManager(const std::string& account_no, const std::string& symbol_code);
		std::shared_ptr<SmAccountOrderManager> FindAddAccountOrderManager(const std::string& account_no);
		std::shared_ptr<SmAccountOrderManager> FindAccountOrderManager(const std::string& account_no);
		// Message from server
		std::string ServerMsg;
		void CheckStopOrder(std::shared_ptr<SmSymbol> symbol);
	private:
		/// <summary>
		/// Order map
		/// Key : order no. value : Order object.
		/// </summary>
		std::map<std::string, std::shared_ptr<SmOrder>> _OrderMap;
		// Key : account no, value : account order manager object
		std::map < std::string, std::shared_ptr<SmAccountOrderManager>> _AccountOrderMgr;
	};
}