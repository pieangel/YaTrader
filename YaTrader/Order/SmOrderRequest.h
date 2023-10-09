#pragma once
#include <string>
#include "SmOrderConst.h"
namespace DarkHorse {
	struct SmOrderRequest {
		int RequestId{ 0 };
		// �ݴ� ������ ��ž ��û ���̵�. ��ž�� ������ �� �ݵ�� �̰͵� ���־� �Ѵ�.
		int CounterRequestId{ 0 };
		std::string Password{ "" };
		std::string SymbolCode{ "" };
		std::string AccountNo{ "" };
		int OrderPrice{ 0 };
		int OrderAmount{ 0 };
		std::string OriOrderNo{ "" };
		// 0 : none, 1 : profit cut, 2 : loss cut , 3 : both(profit & loss) cut
		int CutMode = 0;
		// for loss and profit cut value
		//int CutTick = 0;
		// order with slippage for loss and profit cut 
		int CutSlip = 2;
		int LossCutTick = 2;
		int ProfitCutTick = 2;
		SmPriceType CutPriceType = SmPriceType::Price;
		SmPositionType PositionType = SmPositionType::None;
		SmOrderType OrderType = SmOrderType::None;
		SmPriceType PriceType = SmPriceType::None;
		SmFilledCondition FilledCond = SmFilledCondition::Fas;
	};
}