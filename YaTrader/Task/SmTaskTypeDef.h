#pragma once

namespace DarkHorse {
	enum class SmTaskType
	{
		None,
		MasterFileDownload, // ������ ���� �ٿ� �ε�
		FileDownload, // �ؿ� ������ ���� �ڵ� �ٿ�ε�.
		Market, // ���� ����
		SymbolCode, // ���� �ڵ�
		SymbolMaster, // ���� ����
		DmSymbolMaster, // ���� ���� ����
		AccountAsset, // �ڻ� ����
		AccountProfitLoss, // ���� ����
		SymbolProfitLoss, // ���� ����
		SymbolQuote, // �ü� 
		SymbolHoga, // ȣ��
		ChartData, // ��Ʈ ������
		AcceptedOrderList, // ����Ȯ�� �ֹ� ���
		FilledOrderList, // ü�� Ȯ�� �ֹ� ���
		SymbolPosition, // ������
		RegisterSymbol,
		RegisterAccount,
		DomesticSymbolCode // ���� ��ǰ �ڵ� ����
	};
}