#pragma once
#include <string>
/*
CString strCurrency = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "��ȭ����");
		CString strEntrustTotal = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "��Ź���Ѿ�");
		CString strBalance = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "��Ź���ܾ�");
		CString strUnsettledMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "�̰������ű�");
		CString strOrderMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "�ֹ����ű�");
		CString strEntrustMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "��Ź���ű�");
		CString strMaintenaceMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "�������ű�");
		CString strSettledProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "û�����");
		CString strFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "�����ɼǼ�����");
		CString strOpenProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "�򰡼���");
		CString strOpenTrustTotal = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "�򰡿�Ź�Ѿ�");
		CString strAdditionalMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "�߰����ű�");
		CString strOrderableAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "�ֹ����ɱݾ�");

		nlohmann::json account_asset;
		account_asset["account_no"] = account_no;
		account_asset["currency"] = static_cast<const char*>(strCurrency.Trim());
		account_asset["entrust_total"] = _ttof(strEntrustTotal.Trim());
		account_asset["balance"] = _ttof(strBalance.Trim());
		account_asset["outstanding_deposit"] = _ttof(strUnsettledMargin.Trim());
		account_asset["order_deposit"] = _ttof(strOrderMargin.Trim());
		account_asset["entrust_deposit"] = _ttof(strEntrustMargin.Trim());
		account_asset["maintenance_margin"] = _ttof(strMaintenaceMargin.Trim());
		account_asset["settled_profit_loss"] = _ttof(strSettledProfitLoss.Trim());
		account_asset["fee"] = _ttof(strFee.Trim());
		account_asset["open_profit_loss"] = _ttof(strOpenProfitLoss.Trim());
		account_asset["open_trust_total"] = _ttof(strOpenTrustTotal.Trim());
		account_asset["additional_margin"] = _ttof(strAdditionalMargin.Trim());
		account_asset["orderable_margin"] = _ttof(strOrderableAmount.Trim());
		
		*/
namespace DarkHorse {
	class SmAccountAsset
	{
	public:
		std::string Currency;
		double EntrustTotal{ 0 }; // ��Ź�� �Ѿ� // ǥ��
		double Balance{ 0 }; // ��Ź�� �ܾ�
		double OutstandingDeposit{ 0 }; // �̰��� ���ű�
		double OrderDeposit{ 0 }; // �ֹ� ���ű�
		double EntrustDeposit{ 0 }; // ��Ź ���ű� // ǥ��
		double MaintenanceMargin{ 0 }; // ���� ���ű� // ǥ��
		double TradeProfitLoss{ 0 }; // �Ÿż���(��������, û�����) // ǥ��
		double Fee{ 0 }; // ������ // ǥ��
		double OpenProfitLoss{ 0 }; // �򰡼��� // ǥ��
		double OpenTrustTotal{ 0 }; // �򰡿�Ź �Ѿ� // ǥ��
		double AdditionalMargin{ 0 }; // �߰� ���ű� // ǥ��
		double OrderMargin{ 0 }; // �ֹ� ���� �Ѿ� // ǥ��
	};
}

