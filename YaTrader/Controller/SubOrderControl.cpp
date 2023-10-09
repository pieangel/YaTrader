#include "stdafx.h"
#include "SubOrderControl.h"
#include "../Order/SmOrder.h"
#include <algorithm>
namespace DarkHorse {

	void SubOrderControl::get_order(const int& order_price, std::vector<std::shared_ptr<Order>>& order_vector)
	{
		auto it = order_map_.find(order_price);
		if (it == order_map_.end()) return;
		const std::shared_ptr<PriceOrderMap>& price_order_map = it->second;
		price_order_map->get_order(order_vector);
	}

	void SubOrderControl::clear_order(const int& order_price)
	{
		auto it = order_map_.find(order_price);
		if (it == order_map_.end()) return;
		const std::shared_ptr<PriceOrderMap>& price_order_map = it->second;
		price_order_map->clear();
	}

}