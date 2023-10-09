#include "stdafx.h"
#include "SmHogaProcessor.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Log/MyLogger.h"
#include "../Event/SmCallbackManager.h"
#include "../Hoga/SmHoga.h"
#include "../Hoga/SmHogaManager.h"
#include "../Event/EventHub.h"

using namespace DarkHorse;

SmHogaProcessor::SmHogaProcessor()
{
	StartProcess();
}

SmHogaProcessor::~SmHogaProcessor()
{
	StopProcess();
}

void SmHogaProcessor::StartProcess() noexcept
{
	start();
}

void SmHogaProcessor::StopProcess() noexcept
{
	if (!m_runMode) return;
	// 먼저 큐를 비운다.
	ClearTasks();
	m_stop = true;
	nlohmann::json empty_hoga;
	empty_hoga["symbol_code"] = "";
	_HogaQueue.add(std::move(empty_hoga));
	// 쓰레드가 끝날때까지 기다린다.
	if (m_thread.joinable())
		m_thread.join();
	if (m_runMode)
		m_runMode = false;
}

unsigned int SmHogaProcessor::ThreadHandlerProc(void)
{
	while (true) {
		// 종료 신호를 보내면 루프를 나간다.
		if (isStop()) {
			break;
		}
		nlohmann::json hoga;
		_HogaQueue.take(hoga);
		ProcessHoga(std::move(hoga));
	}

	return 1;
}

void SmHogaProcessor::AddHoga(nlohmann::json&& hoga) noexcept
{
	if (!_Enable) return;

	_HogaQueue.try_add(hoga);
}

void SmHogaProcessor::ProcessHoga(nlohmann::json&& hoga)
{
	try {
		const std::string& symbol_code = hoga["symbol_code"];
		std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
		if (!symbol) return;
		std::shared_ptr<SmHoga> hoga_p = mainApp.HogaMgr()->get_hoga(symbol_code);
		hoga_p->symbol_id = symbol->Id();
		for (int i = 0; i < 5; i++) {
			hoga_p->Ary[i].SellPrice = hoga["hoga_items"][i]["sell_price"];
			hoga_p->Ary[i].BuyPrice = hoga["hoga_items"][i]["buy_price"];
			hoga_p->Ary[i].SellQty = hoga["hoga_items"][i]["sell_qty"];
			hoga_p->Ary[i].BuyQty = hoga["hoga_items"][i]["buy_qty"];
			hoga_p->Ary[i].SellCnt = hoga["hoga_items"][i]["sell_cnt"];
			hoga_p->Ary[i].BuyCnt = hoga["hoga_items"][i]["buy_cnt"];
		}
		hoga_p->symbol_code = symbol_code;
		hoga_p->HogaTime = hoga["hoga_time"];
		hoga_p->TotBuyQty = hoga["tot_buy_qty"];
		hoga_p->TotSellQty = hoga["tot_sell_qty"];
		hoga_p->TotBuyCnt = hoga["tot_buy_cnt"];
		hoga_p->TotSellCnt = hoga["tot_sell_cnt"];

		symbol->hoga_requested(true);

		mainApp.event_hub()->process_hoga_event(hoga_p);

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

bool SmHogaProcessor::ProcessHoga(const std::array<nlohmann::json, BulkHogaSize>& arr, const int& taken)
{

	return true;
}

void DarkHorse::SmHogaProcessor::ClearTasks() noexcept
{
	_HogaQueue.flush();
}
