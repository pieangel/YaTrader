#pragma once
#include <string>
namespace DarkHorse {
	const int kospi_option_turning_point = 1000;
	const int mini_kospi_option_turning_point = 300;
struct VmProduct {
	// �Ҽ���
	int decimal;
	// ���� ƽũ��
	int int_tick_size;
	// ƽũ�� 
	double tick_size;
	// ƽ��ġ - ����
	int tick_value;
	// �¼�
	int seung_su;
	// ȣ�� ���� ����
	int hoga_unit;
	// �ɼ� ���� ��ȯ��.
	int option_value_turning_point;
};
}