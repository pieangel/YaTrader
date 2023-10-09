#include "stdafx.h"
#include "SmOrderViewer.h"
#include "../Order/SmOrder.h"

void DarkHorse::SmOrderViewer::Clear()
{
	_OrderMap.clear();
}

void DarkHorse::SmOrderViewer::AddOrder(std::shared_ptr<SmOrder> order)
{
	const std::lock_guard<std::mutex> lock(_m);
	if (!order) return;


	auto found = _OrderMap.find(order->OrderPrice);
	if (found == _OrderMap.end()) {
		std::map<std::string,  std::shared_ptr<SmOrder>> req_vec;
		req_vec[order->OrderNo] = order;
		_OrderMap[order->OrderPrice] = std::move(req_vec);
	}
	else {
		std::map<std::string,  std::shared_ptr<SmOrder>>& req_vec = found->second;
		req_vec[order->OrderNo] = order;
	}
}

void DarkHorse::SmOrderViewer::RemoveOrder(std::shared_ptr<SmOrder> order)
{
	const std::lock_guard<std::mutex> lock(_m);

	if (!order) return;

	auto found = _OrderMap.find(order->OrderPrice);
	if (found == _OrderMap.end()) return;

	std::map<std::string, std::shared_ptr<SmOrder>>& order_vec = found->second;
	auto found_order = order_vec.find(order->OrderNo);
	if (found_order != order_vec.end()) {
		order_vec.erase(found_order);
	}
	// ∫Û∞°∞›¿∫ ∏ ¿ª æ¯æ÷¡ÿ¥Ÿ.
	if (order_vec.empty()) _OrderMap.erase(found);
}

void DarkHorse::SmOrderViewer::UpdateOrder(std::map<int, int>& order_map)
{
	const std::lock_guard<std::mutex> lock(_m);
	for (auto it = _OrderMap.begin(); it != _OrderMap.end(); ++it) {
		order_map[it->first] = it->second.size();
	}

}

void DarkHorse::SmOrderViewer::GetOrderVec(const int& price, std::map< std::string, std::shared_ptr<SmOrder>>& order_vec_tgt)
{
	const std::lock_guard<std::mutex> lock(_m);

	auto found = _OrderMap.find(price);
	if (found == _OrderMap.end()) return;

	const std::map< std::string, std::shared_ptr<SmOrder>>& order_vec_src = found->second;

	for (auto it = order_vec_src.begin(); it != order_vec_src.end(); ++it) {
		order_vec_tgt[it->second->OrderNo] = it->second;
	}
}


size_t DarkHorse::SmOrderViewer::GetOrderCount(const int& price)
{
	const std::lock_guard<std::mutex> lock(_m);

	auto found = _OrderMap.find(price);
	if (found == _OrderMap.end()) return 0;

	return found->second.size();
}

int DarkHorse::SmOrderViewer::GetTotalCount()
{
	int sum = 0;
	for (auto it = _OrderMap.begin(); it != _OrderMap.end(); ++it) {
		sum += it->second.size();
	}
	return sum;
}

void DarkHorse::SmOrderViewer::RemoveOrder(const int& price)
{
	const std::lock_guard<std::mutex> lock(_m);

	auto found = _OrderMap.find(price);
	if (found == _OrderMap.end()) return;

	_OrderMap.erase(found);
}

