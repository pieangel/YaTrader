#pragma once
#include <string>
#include <memory>
#include "SmOrderConst.h"
#include "OrderContext.h"
namespace DarkHorse {
struct Order {
	// 주문번호
	std::string order_no;
	// 계좌 번호
	std::string account_no;
	// 종목 코드
	std::string symbol_code;
	// 주문 날짜 : 해외 선물일 경우 중요하다.
	std::string order_date;
	// 주문 가격
	int order_price{ 0 };
	// 주문 수량
	int order_amount{ 0 };
	// 주문 시간
	std::string order_time;
	// 원주문 번호
	std::string original_order_no;
	// 최초 원주문 번호
	std::string first_order_no;
	// 체결 주문 가격
	int filled_price{ 0 };
	// 체결량 - 부호가 붙지 않음. 
	int filled_count{ 0 };
	// 미결제량 - 최초에는 체결량과 같다.
	int unsettled_count{ 0 };
	// 체결 날짜 : 해외 선물일 경우 날짜가 매우 중요. 
	std::string filled_date;
	// 체결 시간
	std::string filled_time;
	// 잔량 : 체결되지 않은 수량. 주문 수량과 일치하나 부분 체결시 주문 수량과 일치하지 않을 수 있다. 
	int remain_count{ 0 };
	// 정정 수량
	int modified_count{ 0 };
	// 취소 수량
	int cancelled_count{ 0 };
	// Order Request Id.
	int order_request_id{ 0 };
	// Order Source - Ex) the id of the object that ordered.
	int order_source{ 0 };
	// 1 : 매수, 2 : 매도
	SmPositionType position = SmPositionType::None;
	// 1 : 신규, 2 : 정정, 3 : 취소
	SmOrderType order_type = SmOrderType::None;
	// 가격 유형. 1 : 시장가, 2 : 지정가, 3 : STOP, 4 : STOP 지정가
	SmPriceType price_type = SmPriceType::None;
	// 주문 상태
	SmOrderState order_state = SmOrderState::None;
	// 주문이 나온 상황에 대한 정보를 가지고 있다.
	OrderContext order_context;
};
}