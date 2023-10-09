#pragma once
#include <string>
#include <memory>
#include "SmOrderConst.h"
namespace DarkHorse {
	
	class SmOrder
	{
	public:
		// 주문번호
		std::string OrderNo;
		// 처리 코드
		int ResultCode{ 0 };
		// 처리 메시지
		std::string ResultMessage;
		// 계좌 번호
		std::string AccountNo;
		// 계좌 이름
		std::string AccountName;
		// 종목 코드
		std::string SymbolCode;
		// 1 : 매수, 2 : 매도
		SmPositionType PositionType = SmPositionType::None;
		// 주문 가격
		int OrderPrice{ 0 };
		// 주문 수량
		int OrderAmount{ 0 };
		// 주문 날짜
		std::string OrderDate;
		// 주문 시간
		std::string OrderTime;
		// 1 : 신규, 2 : 정정, 3 : 취소
		SmOrderType OrderType = SmOrderType::None;
		// 0 : 정상, 1 : 거부
		int ResultState{ 0 };
		// 원주문 번호
		std::string OriOrderNo;
		// 최초 원주문 번호
		std::string FirstOrderNo;
		// 사용자정의 
		std::string UserDefined;
		// 체결 주문 가격
		int FilledPrice{ 0 };
		// 체결량
		int FilledCount{ 0 };
		// 미결제량 - 최초에는 체결량과 같다.
		int UnsettledQty{ 0 };
		// 가격 유형. 1 : 시장가, 2 : 지정가, 3 : STOP, 4 : STOP 지정가
		SmPriceType PriceType = SmPriceType::None;
		// 체결 날짜
		std::string FilledDate;
		// 체결 시간
		std::string FilledTime;
		// 주문 상태
		SmOrderState State = SmOrderState::None;
		// 시스템 이름
		std::string SystemName;

		std::string Custom;

		int RemainCount{ 0 };
		int ModifiedCount{ 0 };
		int CanceledCount{ 0 };

		// 1 : new, 2 : modified or cancelled
		int OrderSequence{ 1 };
		// Order Request Id.
		int OrderReqId = 0;
		int account_id{ 0 };
		// SmOrder Contstructor
		explicit SmOrder(const int& order_id) : _id(order_id) {};
		~SmOrder() {};
		int Id() const { return _id; }
		void Id(int val) { _id = val; }
	private:
		int _id{ -1 };
	};


	struct SmFilledInfo {
		std::shared_ptr<SmOrder> Order;
		int FilledPrice;
		int FilledAmount;
		std::string FilledTime;
	};
}

