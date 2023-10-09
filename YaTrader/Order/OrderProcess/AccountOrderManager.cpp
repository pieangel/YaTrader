#include "stdafx.h"
#include "AccountOrderManager.h"
#include "SymbolOrderManager.h"
#include "../Order.h"
namespace DarkHorse {
using symbol_order_manager_p = std::shared_ptr<SymbolOrderManager>;
void AccountOrderManager::dispatch_order(const OrderEvent order_event, order_p order)
{
	symbol_order_manager_p symbol_order_manager = get_symbol_order_manager(order->symbol_code);
	symbol_order_manager->dispatch_order(order_event, order);
}

symbol_order_manager_p AccountOrderManager::find_symbol_order_manager(const std::string& symbol_code)
{
	auto it = symbol_order_manager_map_.find(symbol_code);
	return it != symbol_order_manager_map_.end() ? it->second : nullptr;
}
symbol_order_manager_p AccountOrderManager::create_order_manager(const std::string& symbol_code)
{
	symbol_order_manager_p order_manager = std::make_shared<SymbolOrderManager>();
	order_manager->set_symbol_code(symbol_code);
	symbol_order_manager_map_[symbol_code] = order_manager;
	return order_manager;
}
symbol_order_manager_p AccountOrderManager::get_symbol_order_manager(const std::string& symbol_code)
{
	symbol_order_manager_p order_manager = find_symbol_order_manager(symbol_code);
	if (order_manager) return order_manager;
	return create_order_manager(symbol_code);
}
}
