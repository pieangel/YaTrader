#pragma once
#include "SmOrderConst.h"
#include <string>
namespace DarkHorse {
struct OrderContext {
	std::string parent_account_no;
	std::string sub_account_no;
	std::string fund_name;
	// yes chart name
	std::string signal_name;
	// Order Type. 1 : Sub Account, 2 : Main Account, 3 : Fund.
	OrderType order_source_type = OrderType::None;
	/// <summary>
		/// Order Source ID
		/// 주문이 시작된 곳의 ID. 보통은 주문창의 아이디를 가리키며, 시스템일 경우 시스템의 아이디,
		/// 다른 소스일 경우 그 소스도 아이디를 갖는다. 아이디는 겹치지 않으며
		/// IdGenerator에서 생성하여 겹치지 않게 전달한다. 
		/// 이 아이디는 프로그램 초기에 설정되며 프로그램이 종료하면 모두 사라지고 
		/// 시작될 때 다시 생성되어 각 소스에 매칭이 된다. 
		/// </summary>
	int order_control_id{ 0 };
	int fund_id{ -1 };
	int parent_account_id{ -1 };
	int close;
	int virtual_filled_price;
};
}