#pragma once
namespace DarkHorse {
	struct VmAccountProfitLoss {
		double trade_profit_loss{ 0.0f };       //�Ÿ�(û��)����
		double open_profit_loss{ 0.0f };		//�򰡼���
		double pure_trade_profit_loss{ 0.0f };  // û�� ������
		double trade_fee{ 0.0f };               // û�� ������
	};
}