#include "stdafx.h"
#include "SmStopOrderManager.h"
#include "SmOrderRequest.h"
#include "../Symbol/SmSymbol.h"
#include "../Client/ViStockClient.h"
#include "../Global/SmTotalManager.h"

void DarkHorse::SmStopOrderManager::RemoveOrderRequest(const int& request_id)
{
	auto found = _OrderReqMap.find(request_id);
	if (found == _OrderReqMap.end()) return;
	_OrderReqMap.erase(found);
}

void DarkHorse::SmStopOrderManager::AddOrderRequest(std::shared_ptr<SmOrderRequest> order_req)
{
	if (!order_req) return;

	_OrderReqMap[order_req->RequestId] = order_req;
}

size_t DarkHorse::SmStopOrderManager::FindCount(const int& price)
{
	if (_OrderMap.empty()) return 0;

	return _OrderMap.count(price);
}

void DarkHorse::SmStopOrderManager::CheckStopOrder(const int& close)
{

}

void DarkHorse::SmStopOrderManager::CheckStopOrder(std::shared_ptr<SmSymbol> symbol)
{
	if (_ProcessStop) return;
	_ProcessStop = true;
	int counter_req_id = -1;
	const int int_tick_size = static_cast<int>(symbol->TickSize() * pow(10, symbol->decimal()));
	for (auto it = _OrderReqMap.begin(); it != _OrderReqMap.end(); it++) {
		auto order_req = it->second;
		if (order_req->OrderPrice == symbol->Qoute.close) {
			int slip_order_price = order_req->OrderPrice;
			if (order_req->PositionType == SmPositionType::Buy) {
				slip_order_price += order_req->CutSlip * int_tick_size;
			}
			else {
				slip_order_price -= order_req->CutSlip * int_tick_size;
			}
			order_req->OrderPrice = slip_order_price;
			
			mainApp.Client()->NewOrder(order_req);
			counter_req_id = order_req->CounterRequestId;
			_OrderReqMap.erase(it);
			break;
		}
	}

	// 짝을 이루는 스탑 주문을 없애준다.
	auto found = _OrderReqMap.find(counter_req_id);
	if (found != _OrderReqMap.end()) {
		_OrderReqMap.erase(found);
	}

	_ProcessStop = false;
}
