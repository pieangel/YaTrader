#pragma once
#include <string>
/*
CString strCurrency = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "통화구분");
		CString strEntrustTotal = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁금총액");
		CString strBalance = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁금잔액");
		CString strUnsettledMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "미결제증거금");
		CString strOrderMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문증거금");
		CString strEntrustMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "위탁증거금");
		CString strMaintenaceMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "유지증거금");
		CString strSettledProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "청산손익");
		CString strFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "선물옵션수수료");
		CString strOpenProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가손익");
		CString strOpenTrustTotal = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가예탁총액");
		CString strAdditionalMargin = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "추가증거금");
		CString strOrderableAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가능금액");

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
		double EntrustTotal{ 0 }; // 예탁금 총액 // 표시
		double Balance{ 0 }; // 예탁금 잔액
		double OutstandingDeposit{ 0 }; // 미결제 증거금
		double OrderDeposit{ 0 }; // 주문 증거금
		double EntrustDeposit{ 0 }; // 위탁 증거금 // 표시
		double MaintenanceMargin{ 0 }; // 유지 증거금 // 표시
		double TradeProfitLoss{ 0 }; // 매매손익(실현손익, 청산손익) // 표시
		double Fee{ 0 }; // 수수료 // 표시
		double OpenProfitLoss{ 0 }; // 평가손익 // 표시
		double OpenTrustTotal{ 0 }; // 평가예탁 총액 // 표시
		double AdditionalMargin{ 0 }; // 추가 증거금 // 표시
		double OrderMargin{ 0 }; // 주문 가능 총액 // 표시
	};
}

