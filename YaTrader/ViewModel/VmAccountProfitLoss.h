#pragma once
namespace DarkHorse {
	struct VmAccountProfitLoss {
		double trade_profit_loss{ 0.0f };       //매매(청산)손익
		double open_profit_loss{ 0.0f };		//평가손익
		double pure_trade_profit_loss{ 0.0f };  // 청산 순손익
		double trade_fee{ 0.0f };               // 청산 수수료
	};
}