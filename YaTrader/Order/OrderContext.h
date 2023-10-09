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
		/// �ֹ��� ���۵� ���� ID. ������ �ֹ�â�� ���̵� ����Ű��, �ý����� ��� �ý����� ���̵�,
		/// �ٸ� �ҽ��� ��� �� �ҽ��� ���̵� ���´�. ���̵�� ��ġ�� ������
		/// IdGenerator���� �����Ͽ� ��ġ�� �ʰ� �����Ѵ�. 
		/// �� ���̵�� ���α׷� �ʱ⿡ �����Ǹ� ���α׷��� �����ϸ� ��� ������� 
		/// ���۵� �� �ٽ� �����Ǿ� �� �ҽ��� ��Ī�� �ȴ�. 
		/// </summary>
	int order_control_id{ 0 };
	int fund_id{ -1 };
	int parent_account_id{ -1 };
	int close;
	int virtual_filled_price;
};
}