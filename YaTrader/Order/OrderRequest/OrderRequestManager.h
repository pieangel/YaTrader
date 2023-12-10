#pragma once
#include <memory>
#include <map>
#include <array>
#include <mutex>
#include "../../Common/common.h"
#include "../../Common/BlockingCollection.h"
#include "../SmOrderConst.h"
using namespace code_machina;
namespace DarkHorse {
	constexpr int BulkOrderRequestSize = 500;
	struct OrderRequest;
	using order_request_p = std::shared_ptr<OrderRequest>;
	class OrderRequestManager : public Runnable
	{
	public:
		OrderRequestManager() {
			start_handle_order_request();
		};
		virtual ~OrderRequestManager() {
			stop_handle_order_request();
		};

		// Static Members
		static int id_;
		static int get_id() { return id_++; }

		order_request_p find_order_request(const int order_request_id);
		void add_order_request(order_request_p order_request);
		void remove_order_request(const int order_request_id);
		void start_handle_order_request() noexcept;
		void stop_handle_order_request() noexcept;
		unsigned int ThreadHandlerProc(void) override;
		bool enable() const { return enable_; }
		void enable(bool val) { enable_ = val; }

		static order_request_p make_order_request(
			const int order_price,
			const int order_amount,
			const std::string& symbol_code,
			const SmPositionType& position_type,
			const SmOrderType& order_type,
			const SmPriceType& price_type,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_order_request(
			const std::string& account_no,
			const std::string& password,
			const int order_price,
			const int order_amount,
			const std::string& symbol_code,
			const SmPositionType& position_type,
			const SmOrderType& order_type,
			const SmPriceType& price_type,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_default_sell_order_request(
			const std::string& symbol_code,
			const int order_price,
			const SmPositionType& position_type = SmPositionType::Sell,
			const int order_amount = 1,
			const SmOrderType& order_type = SmOrderType::New,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_default_sell_order_request(
			const std::string& account_no,
			const std::string& password,
			const std::string& symbol_code,
			const int order_price,
			const int order_amount = 1,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmPositionType& position_type = SmPositionType::Sell,
			const SmOrderType& order_type = SmOrderType::New,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_default_buy_order_request(
			const std::string& symbol_code,
			const int order_price,
			const SmPositionType& position_type = SmPositionType::Buy,
			const int order_amount = 1,
			const SmOrderType& order_type = SmOrderType::New,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_default_buy_order_request(
			const std::string& account_no,
			const std::string& password,
			const std::string& symbol_code,
			const int order_price,
			const int order_amount = 1,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmPositionType& position_type = SmPositionType::Buy,
			const SmOrderType& order_type = SmOrderType::New,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p  make_change_order_request(
			const std::string& account_no,
			const std::string& password,
			const std::string& symbol_code,
			const std::string& ori_order_no,
			const int order_price,
			const SmPositionType& position_type = SmPositionType::Buy,
			const int order_amount = 1,
			const SmOrderType& order_type = SmOrderType::Modify,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_cancel_order_request(
			const std::string& account_no,
			const std::string& password,
			const std::string& symbol_code,
			const std::string& ori_order_no,
			const int order_price,
			const SmPositionType& position_type = SmPositionType::Buy,
			const int order_amount = 1,
			const SmOrderType& order_type = SmOrderType::Cancel,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);
	private:
		bool bulk_operation_{ false };
		void clear_order_requests() noexcept;
		bool enable_{ true };
		std::mutex order_request_map_mutex_;
		BlockingCollection<order_request_p> order_request_q;
		// key : request id, value : request object.
		// 여러 쓰레드에서 동시에 접근하므로 변수를 접근하거나 변경할 때, mutex로 lock을 수행해야 한다. 
		std::map<int, order_request_p> order_request_map;
		// arr : 데이터가 들어 있는 배열, taken : 실제 데이터가 들어 있는 갯수
		bool handle_order_request(std::array<order_request_p, BulkOrderRequestSize>& arr, int taken);
		bool handle_order_request(order_request_p order_request);
		bool handle_order_simulation(order_request_p order_request);
		bool handle_order_simulation_ya(order_request_p order_request);
		order_request_p make_dummy_order_request();
		void add_order_request_map(order_request_p order_request);
		bool simulation_{ false };
		bool enable_simulation_filled_order_{ false };
		void dm_make_new_order_event(order_request_p order_request);
		void dm_make_change_order_event(order_request_p order_request);
		void dm_make_cancel_order_event(order_request_p order_request);
		void dm_make_filled_order_event(order_request_p order_request);
		void ab_make_new_order_event(order_request_p order_request);

		void ya_make_ab_accepted_order_event(order_request_p order_request);
		void ya_make_ab_filled_order_event(order_request_p order_request);

		void ab_make_change_order_event(order_request_p order_request);
		void ab_make_cancel_order_event(order_request_p order_request);
		void ab_make_filled_order_event(order_request_p order_request);
		void on_new_order(order_request_p order_request);
		void on_change_order(order_request_p order_request);
		void on_cancel_order(order_request_p  order_request);
	public:
		void set_enable_simulation_filled_order(const bool flag) {
			enable_simulation_filled_order_ = flag;
		}
		bool get_enable_simulation_filled_order() {
			return enable_simulation_filled_order_;
		}
		order_request_p find_order_request(const std::string& custom_info); 
		void set_simulation(const bool mode) {
			simulation_ = mode;
		}
		bool get_simulation() {
			return simulation_;
		}
		int order_no_{ 0 };
		int get_order_no() {
			return ++order_no_;
		}
	};
}
