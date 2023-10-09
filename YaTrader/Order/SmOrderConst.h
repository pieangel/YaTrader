#pragma once

namespace DarkHorse {


	enum MouseMode {
		Normal,
		OrderMove
	};

	enum OrderEvent {
		None,
		DM_Accepted,
		DM_Unfilled,
		DM_Filled,
		AB_Accepted,
		AB_Unfilled,
		AB_Filled,
		OE_Accepted,
		OE_Unfilled,
		OE_Filled
	};

	enum class SmOrderError {
		None,
		BadAccount,
		BadSymbol,
		BadPriceType,
	};

	enum class SmPositionType
	{
		None = 0,
		Buy,
		Sell,
		ExitBuy,
		ExitSell
	};

	enum class SmOrderType
	{
		None = 0,
		New,
		Modify,
		Cancel
	};

	enum class SmPriceType
	{
		None = 0,
		Price, // 지정가
		Market, // 시장가
		Condition, // 조건
		BestPrice // 최상위 유리가
	};

	//1:FAS, 2:FOK, 3:FAK
	enum class SmFilledCondition
	{
		None = 0,
		Fas = 1, //
		Fok = 2, //
		Fak = 3,
		Day = 4
	};

	enum class SmCutMode
	{
		None = 0,
		ProfitCut,
		LossCut,
		BothCut
	};
	enum class SmOrderState
	{
		None,
		/// <summary>
		/// 원장접수
		/// </summary>
		Ledger,
		/// 신규 확인
		ConfirmNew,
		/// <summary>
		/// 정정 확인
		/// </summary>
		ConfirmModify,
		/// <summary>
		/// 취소 확인
		/// </summary>
		ConfirmCancel,
		/// <summary>
		/// 주문접수
		/// </summary>
		Accepted,
		/// <summary>
		/// 체결
		/// </summary>
		Filled,
		// 부분체결
		PartialFilled,
		/// <summary>
		/// 신규주문거부
		/// </summary>
		RejectNew,
		/// <summary>
		/// 정정거부
		/// </summary>
		RejectModify,
		/// <summary>
		/// 취소 거부
		/// </summary>
		RejectCancel,
		// 청산됨 - 다른 주문에 의해서 혹은 잔고를 청산하는 용도로
		Settled
	};

	enum class OrderType {
		None = 0,
		MainAccount,
		SubAccount,
		Fund
	};

	enum SubOrderControlType {
		CT_NONE = 0, 
		CT_BUY, 
		CT_SELL 
	};

	enum class OrderBackGround {
		OB_NONE = 0,
		OB_HAS_BEEN,
		OB_PRESENT
	};
}