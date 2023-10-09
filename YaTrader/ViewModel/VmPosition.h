#pragma once
#include <string>
#include "../Order/SmOrderConst.h"
struct VmPosition {
	// ���� ��ȣ
	std::string account_no;
	std::string symbol_code;
	std::string fund_name;
	int symbol_id{ 0 };
	int account_id{ 0 };
	int fund_id{ 0 };
	// �������� �ܰ�
	// �������� �ܰ�� �ż��� �϶��� ���, �ŵ��� ���� ������ ǥ���Ѵ�.
	// �������� �ܰ� 0�̸� �������� ���� ���̴�.
	int open_quantity{ 0 };
	double trade_profit_loss{ 0.0f }; //	�Ÿż���
	// �Ҽ��� �ݿ� ���� ����. 
	double average_price{ 0.0f };	// ��հ�*/
	double open_profit_loss{ 0.0f };		/*�򰡼���*/
	double pure_trade_profit_loss{ 0.0f }; // û�� ������
	double trade_fee{ 0.0f }; // û�� ������
	DarkHorse::OrderType position_type{ DarkHorse::OrderType::MainAccount };
};