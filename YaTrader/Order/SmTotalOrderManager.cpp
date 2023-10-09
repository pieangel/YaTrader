#include "stdafx.h"
#include "SmTotalOrderManager.h"
#include "SmAccountOrderManager.h"
using namespace DarkHorse;

int SmTotalOrderManager::_Id = 0;

std::shared_ptr<DarkHorse::SmAccountOrderManager> SmTotalOrderManager::FindAddAccountOrderManager(const std::string& account_no)
{
	auto found = _AccountOrderMgr.find(account_no);
	if (found != _AccountOrderMgr.end())
		return found->second;
	else {
		auto account_order_mgr = std::make_shared<DarkHorse::SmAccountOrderManager>(account_no);
		
		_AccountOrderMgr[account_no] = account_order_mgr;
		return account_order_mgr;
	}
}

std::shared_ptr<DarkHorse::SmOrder> SmTotalOrderManager::FindAddOrder(const std::string& order_no)
{
	auto found = _OrderMap.find(order_no);
	if (found != _OrderMap.end())
		return found->second;
	else {
		// 오직 여기에서만 주문을 생성할 수 있다.
		// 주문은 고유 번호와 함께 생성된다.
		auto order = std::make_shared<SmOrder>(GetId());
		order->OrderNo = order_no;
		_OrderMap[order_no] = order;
		return order;
	}
}

std::shared_ptr<DarkHorse::SmSymbolOrderManager> SmTotalOrderManager::FindAddSymbolOrderManager(const std::string& account_no, const std::string& symbol_code)
{
	auto account_order_mgr = FindAddAccountOrderManager(account_no);
	return account_order_mgr->FindAddSymbolOrderManager(symbol_code);
}

std::shared_ptr<DarkHorse::SmAccountOrderManager> SmTotalOrderManager::FindAccountOrderManager(const std::string& account_no)
{
	auto found = _AccountOrderMgr.find(account_no);
	if (found != _AccountOrderMgr.end())
		return found->second;
	else
		return nullptr;
}

void DarkHorse::SmTotalOrderManager::CheckStopOrder(std::shared_ptr<SmSymbol> symbol)
{
	for (auto it = _AccountOrderMgr.begin(); it != _AccountOrderMgr.end(); ++it) {
		it->second->CheckStopOrder(symbol);
	}
}
