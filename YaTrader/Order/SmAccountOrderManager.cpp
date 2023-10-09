#include "stdafx.h"
#include "SmAccountOrderManager.h"
#include "SmSymbolOrderManager.h"
#include "../Order/SmOrder.h"
using namespace DarkHorse;
std::shared_ptr<DarkHorse::SmSymbolOrderManager> SmAccountOrderManager::FindAddSymbolOrderManager(const std::string& symbol_code)
{
	auto found = _SymbolOrderManagerMap.find(symbol_code);
	if (found != _SymbolOrderManagerMap.end())
		return found->second;
	else {
		auto symbol_order_mgr = std::make_shared<DarkHorse::SmSymbolOrderManager>(_AccountNo, symbol_code);
		_SymbolOrderManagerMap[symbol_code] = symbol_order_mgr;
		return symbol_order_mgr;
	}
}

void DarkHorse::SmAccountOrderManager::CheckStopOrder(std::shared_ptr<SmSymbol> symbol)
{
	for (auto it = _SymbolOrderManagerMap.begin(); it != _SymbolOrderManagerMap.end(); it++) {
		it->second->CheckStopOrder(symbol);
	}
}

void DarkHorse::SmAccountOrderManager::AddFilledList(std::shared_ptr<DarkHorse::SmOrder> filled_order)
{
	if (!filled_order) return;

	_FilledMap[filled_order->OrderNo] = filled_order;
}

void DarkHorse::SmAccountOrderManager::AddFilledInfo(std::shared_ptr<DarkHorse::SmOrder> filled_order)
{
	if (!filled_order) return;

	SmFilledInfo filled_info;
	filled_info.Order = filled_order;
	filled_info.FilledPrice = filled_order->FilledPrice;
	filled_info.FilledAmount = filled_order->FilledCount;
	filled_info.FilledTime = filled_order->FilledTime;

	_FilledInfoList.push_front(std::move(filled_info));
}
