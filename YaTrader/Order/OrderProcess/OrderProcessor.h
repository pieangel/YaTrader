#pragma once
#include "../../Json/json.hpp"
#include "../../Common/common.h"
#include "../../Common/BlockingCollection.h"
using namespace code_machina;
namespace DarkHorse {
constexpr int BulkOrderProcessSize = 500;
using order_event = nlohmann::json;

class OrderProcessor : public Runnable
{
public:
	OrderProcessor() {
		start_handle_order_event();
	};
	virtual ~OrderProcessor() {
		stop_handle_order_event();
	};
	unsigned int ThreadHandlerProc(void) override;
	bool enable() const { return enable_; }
	void enable(bool val) { enable_ = val; }

	void start_handle_order_event() noexcept;
	void stop_handle_order_event() noexcept;
	void add_order_event(order_event&& order_event_t) noexcept;
private:
	bool bulk_operation_{ false };
	bool enable_{ true };
	BlockingCollection<order_event> order_event_q;
	bool handle_order_event(const std::array<order_event, BulkOrderProcessSize>& arr, int taken);
	bool handle_order_event(order_event&& order_info_item);
	bool handle_order_event(order_event& order_info_item);
	void clear_order_event() noexcept;
	int get_order_request_id(const std::string& custom_info);
	order_event make_dummy_order_event();
};
}

