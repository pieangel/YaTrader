#include "stdafx.h"
#include "HogaControl.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../View/SymbolOrderView.h"
#include "../Hoga/SmHoga.h"
#include "../Log/MyLogger.h"

namespace DarkHorse {
	HogaControl::HogaControl()
		: id_(IdGenerator::get_id())
	{
		subscribe_hoga_control();
	}
	HogaControl::~HogaControl()
	{
		mainApp.event_hub()->unsubscribe_hoga_event_handler( id_ );
	}
	void HogaControl::subscribe_hoga_control()
	{
		mainApp.event_hub()->subscribe_hoga_event_handler
		(
			id_,
			std::bind(&HogaControl::update_hoga, this, std::placeholders::_1)
		);
	}
	void HogaControl::update_hoga(std::shared_ptr<SmHoga> hoga)
	{
		try {
			if (!hoga) return;
			if (hoga->symbol_id != 0 && hoga->symbol_id != symbol_id_) return;

			for (int i = 0; i < 5; i++) {
				hoga_.Ary[i].BuyCnt = hoga->Ary[i].BuyCnt;
				hoga_.Ary[i].BuyPrice = hoga->Ary[i].BuyPrice;
				hoga_.Ary[i].BuyQty = hoga->Ary[i].BuyQty;
				hoga_.Ary[i].SellCnt = hoga->Ary[i].SellCnt;
				hoga_.Ary[i].SellPrice = hoga->Ary[i].SellPrice;
				hoga_.Ary[i].SellQty = hoga->Ary[i].SellQty;
			}

			hoga_.TotSellQty = hoga->TotSellQty;
			hoga_.TotBuyQty = hoga->TotBuyQty;
			hoga_.TotSellCnt = hoga->TotSellCnt;
			hoga_.TotBuyCnt = hoga->TotBuyCnt;
			hoga_.HogaTime = hoga->HogaTime;

			if (event_handler_) event_handler_();
		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}
}