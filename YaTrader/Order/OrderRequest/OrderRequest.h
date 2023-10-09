#pragma once
#include <string>
#include "../SmOrderConst.h"
#include "../OrderContext.h"
namespace DarkHorse {
	enum class OrderRequestType {
		None, Domestic, Abroad 
	};
	struct OrderRequest {
		OrderRequestType request_type{ OrderRequestType::None };
		int request_id{ 0 };
		// 반대 포지션 스탑 요청 아이디. 스탑이 없어질 때 반드시 이것도 없애야 한다.
		int counter_request_id{ 0 };
		int counter_request_price{ 0 };
		std::string password{ "" };
		std::string symbol_code{ "" };
		std::string account_no{ "" };
		int order_price{ 0 };
		int order_amount{ 0 };
		std::string original_order_no{ "" };
		// for the loss and profit of the cut value
		// order with the slippage for the loss and profit cut 
		int cut_slip{ 2 };
		int loss_cut_tick{ 2 };
		int profit_cut_tick{ 2 };
		// 0 : none, 1 : profit cut, 2 : loss cut , 3 : both(profit & loss) cut
		SmCutMode cut_mode{ SmCutMode::None };
		SmPriceType cut_price_type{ SmPriceType::Price };
		SmPositionType position_type{ SmPositionType::None };
		SmOrderType order_type{ SmOrderType::None };
		SmPriceType price_type{ SmPriceType::None };
		SmFilledCondition fill_condition{ SmFilledCondition::Fas };

		// 주문이 나온 상황에 대한 정보를 가지고 있다.
		OrderContext order_context;
	};
}