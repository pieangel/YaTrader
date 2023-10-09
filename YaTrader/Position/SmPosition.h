#pragma once
#include <string>
#include "../Order/SmOrderConst.h"
namespace DarkHorse {
	// position->OpenPL = total_filled_order_qty * (symbol->Qoute.close - avg_price) * symbol->SeungSu();
	/*
	CString strSymbolSettledProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "청산손익");
		CString strSymbolSettledPureProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "청산순손익");
		CString strSymbolFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "청산수수료");
		CString strSymbolOpenProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평가손익");
		CString strSymbolUnsettledFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "미결제수수료");
		CString strSymbolUnsettledPureProfitLoss = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "미결제순손익");
	*/
	struct SmPosition
	{
		// 심볼 코드
		std::string SymbolCode;
		// 계좌 번호
		std::string AccountNo;
		// 포지션
		SmPositionType Position = SmPositionType::None;
		// 포지션의 잔고
		// 포지션의 잔고는 매수는 일때는 양수, 매도일 때는 음수로 표현한다.
		// 포지션의 잔고가 0이면 포지션이 없는 것이다.
		int OpenQty{ 0 };
		double TradePL{ 0.0f }; //	매매손익
		double AvgPrice{ 0.0f };	// 평균가*/
		double OpenPL{ 0.0f };		/*평가손익*/
		double PureTradePL{ 0.0f }; // 청산 순손익
		double TradeFee{ 0.0f }; // 청산 수수료
		double UnsettledFee{ 0.0f }; // 미결제 수수료
		double UnsettledPurePL{ 0.0f }; // 미결제 순손익
	};

}
