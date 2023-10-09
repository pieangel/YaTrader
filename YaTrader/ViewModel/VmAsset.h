#pragma once
#include <string>
namespace DarkHorse {
	struct VmAsset {
		double entrust_total; // 예탁금총액
		double balance; // 예탁금잔액
		double outstanding_deposit; // 미결제증거금
		double order_deposit; // 주문증거금
		double entrust_deposit; // 위탁증거금
		double maintenance_margin; // 유지증거금
		double trade_profit_loss; // 청산손익
		double trade_fee; // 선물옵션수수료
		double open_profit_loss; // 평가손익
		double open_trust_toal; // 평가예탁총액
		double additional_margin; // 추가증거금
		double order_margin; // 주문가능금액
		std::string currency; // 통화 코드 
	};
}