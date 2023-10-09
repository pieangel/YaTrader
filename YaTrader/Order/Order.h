#pragma once
#include <string>
#include <memory>
#include "SmOrderConst.h"
#include "OrderContext.h"
namespace DarkHorse {
struct Order {
	// �ֹ���ȣ
	std::string order_no;
	// ���� ��ȣ
	std::string account_no;
	// ���� �ڵ�
	std::string symbol_code;
	// �ֹ� ��¥ : �ؿ� ������ ��� �߿��ϴ�.
	std::string order_date;
	// �ֹ� ����
	int order_price{ 0 };
	// �ֹ� ����
	int order_amount{ 0 };
	// �ֹ� �ð�
	std::string order_time;
	// ���ֹ� ��ȣ
	std::string original_order_no;
	// ���� ���ֹ� ��ȣ
	std::string first_order_no;
	// ü�� �ֹ� ����
	int filled_price{ 0 };
	// ü�ᷮ - ��ȣ�� ���� ����. 
	int filled_count{ 0 };
	// �̰����� - ���ʿ��� ü�ᷮ�� ����.
	int unsettled_count{ 0 };
	// ü�� ��¥ : �ؿ� ������ ��� ��¥�� �ſ� �߿�. 
	std::string filled_date;
	// ü�� �ð�
	std::string filled_time;
	// �ܷ� : ü����� ���� ����. �ֹ� ������ ��ġ�ϳ� �κ� ü��� �ֹ� ������ ��ġ���� ���� �� �ִ�. 
	int remain_count{ 0 };
	// ���� ����
	int modified_count{ 0 };
	// ��� ����
	int cancelled_count{ 0 };
	// Order Request Id.
	int order_request_id{ 0 };
	// Order Source - Ex) the id of the object that ordered.
	int order_source{ 0 };
	// 1 : �ż�, 2 : �ŵ�
	SmPositionType position = SmPositionType::None;
	// 1 : �ű�, 2 : ����, 3 : ���
	SmOrderType order_type = SmOrderType::None;
	// ���� ����. 1 : ���尡, 2 : ������, 3 : STOP, 4 : STOP ������
	SmPriceType price_type = SmPriceType::None;
	// �ֹ� ����
	SmOrderState order_state = SmOrderState::None;
	// �ֹ��� ���� ��Ȳ�� ���� ������ ������ �ִ�.
	OrderContext order_context;
};
}