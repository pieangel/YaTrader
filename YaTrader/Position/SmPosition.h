#pragma once
#include <string>
#include "../Order/SmOrderConst.h"
namespace DarkHorse {
	// position->OpenPL = total_filled_order_qty * (symbol->Qoute.close - avg_price) * symbol->SeungSu();
	/*
	CString strSymbolSettledProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "û�����");
		CString strSymbolSettledPureProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "û�������");
		CString strSymbolFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "û�������");
		CString strSymbolOpenProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "�򰡼���");
		CString strSymbolUnsettledFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "�̰���������");
		CString strSymbolUnsettledPureProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "�̰���������");
	*/
	struct SmPosition
	{
		// �ɺ� �ڵ�
		std::string SymbolCode;
		// ���� ��ȣ
		std::string AccountNo;
		// ������
		SmPositionType Position = SmPositionType::None;
		// �������� �ܰ�
		// �������� �ܰ�� �ż��� �϶��� ���, �ŵ��� ���� ������ ǥ���Ѵ�.
		// �������� �ܰ� 0�̸� �������� ���� ���̴�.
		int OpenQty{ 0 };
		double TradePL{ 0.0f }; //	�Ÿż���
		double AvgPrice{ 0.0f };	// ��հ�*/
		double OpenPL{ 0.0f };		/*�򰡼���*/
		double PureTradePL{ 0.0f }; // û�� ������
		double TradeFee{ 0.0f }; // û�� ������
		double UnsettledFee{ 0.0f }; // �̰��� ������
		double UnsettledPurePL{ 0.0f }; // �̰��� ������
	};

}
