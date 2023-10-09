#include "stdafx.h"
#include <memory>
#include "OrderProcessor.h"
#include "../../Log/MyLogger.h"
#include "../../Global/SmTotalManager.h"
#include "TotalOrderManager.h"
namespace DarkHorse {
using total_order_manager_p = std::shared_ptr<TotalOrderManager>;

unsigned int OrderProcessor::ThreadHandlerProc(void)
{
	while (true) {
		// ���� ��ȣ�� ������ ������ ������.
		if (isStop()) break;
		if (bulk_operation_) {
			std::array<order_event, BulkOrderProcessSize> order_event_arr;
			size_t taken{ 0 };
			auto status = order_event_q.take_bulk(order_event_arr.begin(), order_event_arr.size(), taken);
			if (status != BlockingCollectionStatus::Ok) continue;
			handle_order_event(order_event_arr, taken);
		}
		else {
			order_event order_event_t;
			auto status = order_event_q.take(order_event_t);
			if (status != BlockingCollectionStatus::Ok) continue;
			if (order_event_t.is_null()) return 1;
			handle_order_event(std::move(order_event_t));
		}
	}

	return 1;
}

void OrderProcessor::start_handle_order_event() noexcept
{
	start();
}

void OrderProcessor::stop_handle_order_event() noexcept
{
	if (!m_runMode) return;
	// ���� ť�� ����.
	clear_order_event();
	m_stop = true;
	order_event empty_order_event = make_dummy_order_event();
	// �����带 �����. 
	order_event_q.add(std::move(empty_order_event));
	// �����尡 ���������� ��ٸ���.
	if (m_thread.joinable()) m_thread.join();
	// ������ ����带 �����Ѵ�. 
	if (m_runMode) m_runMode = false;
}

void OrderProcessor::add_order_event(order_event&& order_event_t) noexcept
{
	order_event_q.add(std::move(order_event_t));
}

bool OrderProcessor::handle_order_event(const std::array<order_event, BulkOrderProcessSize>& arr, int taken)
{
	if (taken == 0) return true;
	for (auto order_event_t : arr)
		handle_order_event(order_event_t);
	return true;
}

bool OrderProcessor::handle_order_event(order_event&& order_info_item)
{
	try {
		total_order_manager_p total_order_manager = mainApp.total_order_manager();
		total_order_manager->on_order_event(std::move(order_info_item));
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		return false;
	}
	return true;
}

bool OrderProcessor::handle_order_event(order_event& order_info_item)
{
	try {
		total_order_manager_p total_order_manager = mainApp.total_order_manager();
		total_order_manager->on_order_event(std::move(order_info_item));
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		return false;
	}
	return true;
}

void OrderProcessor::clear_order_event() noexcept
{
	order_event_q.flush();
}

int OrderProcessor::get_order_request_id(const std::string& custom_info)
{
	if (custom_info.length() == 0) return -1;
	try {
		const int zero_pos = custom_info.find_first_of('0');
		const int order_request_id_pos = custom_info.find_first_of('k');
		const std::string order_request_id_t = custom_info.substr(zero_pos, order_request_id_pos - zero_pos);
		return std::stoi(order_request_id_t);
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
	return -1;
}

order_event OrderProcessor::make_dummy_order_event()
{
	order_event dummy_order;
	return std::move(dummy_order);
}

}
