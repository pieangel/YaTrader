#pragma once
#include <string>
#include "../Order/SmOrderConst.h"
struct VmPosition {
	// 계좌 번호
	std::string account_no;
	std::string symbol_code;
	std::string fund_name;
	int symbol_id{ 0 };
	int account_id{ 0 };
	int fund_id{ 0 };
	// 포지션의 잔고
	// 포지션의 잔고는 매수는 일때는 양수, 매도일 때는 음수로 표현한다.
	// 포지션의 잔고가 0이면 포지션이 없는 것이다.
	int open_quantity{ 0 };
	double trade_profit_loss{ 0.0f }; //	매매손익
	// 소수점 반영 안한 값임. 
	double average_price{ 0.0f };	// 평균가*/
	double open_profit_loss{ 0.0f };		/*평가손익*/
	double pure_trade_profit_loss{ 0.0f }; // 청산 순손익
	double trade_fee{ 0.0f }; // 청산 수수료
	DarkHorse::OrderType position_type{ DarkHorse::OrderType::MainAccount };
};