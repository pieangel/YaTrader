#pragma once
#include <string>
#include <memory>
#include "SmOrderConst.h"
namespace DarkHorse {
	
	class SmOrder
	{
	public:
		// �ֹ���ȣ
		std::string OrderNo;
		// ó�� �ڵ�
		int ResultCode{ 0 };
		// ó�� �޽���
		std::string ResultMessage;
		// ���� ��ȣ
		std::string AccountNo;
		// ���� �̸�
		std::string AccountName;
		// ���� �ڵ�
		std::string SymbolCode;
		// 1 : �ż�, 2 : �ŵ�
		SmPositionType PositionType = SmPositionType::None;
		// �ֹ� ����
		int OrderPrice{ 0 };
		// �ֹ� ����
		int OrderAmount{ 0 };
		// �ֹ� ��¥
		std::string OrderDate;
		// �ֹ� �ð�
		std::string OrderTime;
		// 1 : �ű�, 2 : ����, 3 : ���
		SmOrderType OrderType = SmOrderType::None;
		// 0 : ����, 1 : �ź�
		int ResultState{ 0 };
		// ���ֹ� ��ȣ
		std::string OriOrderNo;
		// ���� ���ֹ� ��ȣ
		std::string FirstOrderNo;
		// ��������� 
		std::string UserDefined;
		// ü�� �ֹ� ����
		int FilledPrice{ 0 };
		// ü�ᷮ
		int FilledCount{ 0 };
		// �̰����� - ���ʿ��� ü�ᷮ�� ����.
		int UnsettledQty{ 0 };
		// ���� ����. 1 : ���尡, 2 : ������, 3 : STOP, 4 : STOP ������
		SmPriceType PriceType = SmPriceType::None;
		// ü�� ��¥
		std::string FilledDate;
		// ü�� �ð�
		std::string FilledTime;
		// �ֹ� ����
		SmOrderState State = SmOrderState::None;
		// �ý��� �̸�
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

