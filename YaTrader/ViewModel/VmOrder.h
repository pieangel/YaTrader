#pragma once
#include <string>
#include "../Order/SmOrderConst.h"
namespace DarkHorse {
	struct AcceptedOrder {
		std::string order_no;
		std::string account_no;
		std::string symbol_code;
		SmPositionType position{ SmPositionType::None };
		int order_amount{ 0 };
		int order_price{ 0 };
	};
}