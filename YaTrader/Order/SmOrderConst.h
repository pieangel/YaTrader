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
		Price, // ������
		Market, // ���尡
		Condition, // ����
		BestPrice // �ֻ��� ������
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
		/// ��������
		/// </summary>
		Ledger,
		/// �ű� Ȯ��
		ConfirmNew,
		/// <summary>
		/// ���� Ȯ��
		/// </summary>
		ConfirmModify,
		/// <summary>
		/// ��� Ȯ��
		/// </summary>
		ConfirmCancel,
		/// <summary>
		/// �ֹ�����
		/// </summary>
		Accepted,
		/// <summary>
		/// ü��
		/// </summary>
		Filled,
		// �κ�ü��
		PartialFilled,
		/// <summary>
		/// �ű��ֹ��ź�
		/// </summary>
		RejectNew,
		/// <summary>
		/// �����ź�
		/// </summary>
		RejectModify,
		/// <summary>
		/// ��� �ź�
		/// </summary>
		RejectCancel,
		// û��� - �ٸ� �ֹ��� ���ؼ� Ȥ�� �ܰ� û���ϴ� �뵵��
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