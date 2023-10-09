#pragma once
#include <string>
#include <vector>

namespace DarkHorse {
	enum Hoga_Type {
		SELL = 0,
		BUY
	};

	enum OrderHeader 
	{
		SELL_STOP = 0,
		SELL_ORDER,
		SELL_CNT,
		SELL_QTY,
		//SELL_FILLED,
		QUOTE,
		//BUY_FILLED,
		BUY_QTY,
		BUY_CNT,
		BUY_ORDER,
		BUY_STOP
	};

	typedef struct _OrderGridHeaderInfo 
	{
		OrderHeader index;
		std::string title;
		int width;
	} OrderGridHeaderInfo;

	static std::vector<OrderGridHeaderInfo> OrderGridHeaderVector 
	{
		{OrderHeader::SELL_STOP, "STOP", 40},
		{OrderHeader::SELL_ORDER, "�ֹ�", 55},
		{OrderHeader::SELL_CNT, "�Ǽ�", 0},
		{OrderHeader::SELL_QTY, "�ܷ�", 35},
		//{OrderGridHeader::SELL_FILLED, "ü��", 35},
		{OrderHeader::QUOTE, "�ü�", 80},
		//{OrderGridHeader::BUY_FILLED, "ü��", 35},
		{OrderHeader::BUY_QTY, "�ܷ�", 35},
		{OrderHeader::BUY_CNT, "�Ǽ�", 0},
		{OrderHeader::BUY_ORDER, "�ֹ�", 55},
		{OrderHeader::BUY_STOP, "STOP", 40}
	};
}