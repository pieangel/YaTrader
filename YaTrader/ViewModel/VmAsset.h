#pragma once
#include <string>
namespace DarkHorse {
	struct VmAsset {
		double entrust_total; // ��Ź���Ѿ�
		double balance; // ��Ź���ܾ�
		double outstanding_deposit; // �̰������ű�
		double order_deposit; // �ֹ����ű�
		double entrust_deposit; // ��Ź���ű�
		double maintenance_margin; // �������ű�
		double trade_profit_loss; // û�����
		double trade_fee; // �����ɼǼ�����
		double open_profit_loss; // �򰡼���
		double open_trust_toal; // �򰡿�Ź�Ѿ�
		double additional_margin; // �߰����ű�
		double order_margin; // �ֹ����ɱݾ�
		std::string currency; // ��ȭ �ڵ� 
	};
}