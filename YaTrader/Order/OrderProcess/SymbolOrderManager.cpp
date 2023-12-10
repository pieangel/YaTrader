#include "stdafx.h"
#include "SymbolOrderManager.h"
#include "../Order.h"
#include "../SmOrderConst.h"
#include "../../Global/SmTotalManager.h"
#include "../../Position/TotalPositionManager.h"
#include "../../Event/EventHub.h"
#include "../SmOrder.h"
#include "../../Log/MyLogger.h"
#include "../../Position/Position.h"


namespace DarkHorse {
	using total_position_manager_p = std::shared_ptr<TotalPositionManager>;
void SymbolOrderManager::dispatch_order(const OrderEvent order_event, order_p order)
{
	// Set the flag to true to indicate that the order has been placed before.
	set_ordered_before(true);
	switch (order_event) {
	case OrderEvent::OE_Accepted:
		on_order_accepted(order, order_event);
		break;
	case OrderEvent::OE_Unfilled:
		on_order_unfilled(order, order_event);
		break;
	case OrderEvent::OE_Filled:
		on_order_filled(order, order_event);
		break;
	}
}


DarkHorse::OrderBackGround SymbolOrderManager::get_order_background(const int position_open_qty)
{
	if (accepted_order_map_.size() > 0 || position_open_qty != 0) {
		return OrderBackGround::OB_PRESENT;
	}
	else if (accepted_order_map_.empty() && position_open_qty == 0) {
		if (ordered_before_) {
			return OrderBackGround::OB_HAS_BEEN;
		}
		else {
			return OrderBackGround::OB_NONE;
		}
	}
	else {
		return OrderBackGround::OB_NONE;
	}
}

void SymbolOrderManager::on_order_accepted(order_p order, OrderEvent order_event)
{
	order->order_state = SmOrderState::Accepted;
	if (order->order_type == SmOrderType::New) {
		add_accepted_order(order);
	}
	else if (order->order_type == SmOrderType::Modify) {
		add_accepted_order(order);
		remove_accepted_order(order->original_order_no);
	}
	else if (order->order_type == SmOrderType::Cancel)
	{
		remove_accepted_order(order->original_order_no);
	}
}
void SymbolOrderManager::on_order_unfilled(order_p order, OrderEvent order_event)
{
	if (order->remain_count == 0)
		update_accepted_order(order);
	else
		add_accepted_order(order);
	//mainApp.event_hub()->process_order_event(order, order_event);
}
void SymbolOrderManager::on_order_filled(order_p order, OrderEvent order_event)
{
	update_accepted_order(order);
	order->order_state = SmOrderState::Filled;
	total_position_manager_p total_position_manager = mainApp.total_position_manager();
	total_position_manager->update_position(order);
}

void SymbolOrderManager::add_accepted_order(order_p order)
{
	std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
	auto it = accepted_order_map_.find(order->order_no);
	if (it != accepted_order_map_.end()) return;
	accepted_order_map_[order->order_no] = order;

	mainApp.event_hub()->process_order_event(order, OrderEvent::OE_Accepted);
}

void SymbolOrderManager::update_accepted_order(order_p order)
{
	if (order->filled_count > 0) {
		if (order->order_amount == order->filled_count) // 완전 체결
			remove_accepted_order(order);
		else if (order->filled_count < order->order_amount) { // 부분체결
			CString message;
			message.Format("부분 체결 : 주문번호[%s], 주문 개수[%d], 체결개수[%d], 잔량[%d]", 
				order->order_no.c_str(), 
				order->order_amount, 
				order->filled_count, 
				order->remain_count);
			LOGINFO(CMyLogger::getInstance(), "주문 정보 = %s", message);
		}
	}
	else
		remove_accepted_order(order);
	mainApp.event_hub()->process_order_event(order, OrderEvent::OE_Unfilled);
}

void SymbolOrderManager::remove_accepted_order(order_p order)
{
	std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
	auto it = accepted_order_map_.find(order->order_no);
	if (it == accepted_order_map_.end()) return;
	accepted_order_map_.erase(it);
}

void SymbolOrderManager::remove_accepted_order(const std::string& order_no)
{
	std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex
	auto it = accepted_order_map_.find(order_no);
	if (it == accepted_order_map_.end()) return;
	auto order = it->second;
	accepted_order_map_.erase(it);

	mainApp.event_hub()->process_order_event(order, OrderEvent::OE_Unfilled);
}

}