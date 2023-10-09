#include "stdafx.h"
#include "SmQuoteProcessor.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Log/MyLogger.h"
#include "../MessageDefine.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Event/EventHub.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Position/TotalPositionManager.h"
using namespace DarkHorse;

SmQuoteProcessor::SmQuoteProcessor() 
{
	StartProcess();
}

SmQuoteProcessor::~SmQuoteProcessor()
{
	StopProcess();
}

void SmQuoteProcessor::StartProcess() noexcept
{
	start();
}

void SmQuoteProcessor::StopProcess() noexcept
{
	if (!m_runMode) return;
	// 먼저 큐를 비운다.
	ClearTasks();
	m_stop = true;
	nlohmann::json empty_quote;
	empty_quote["symbol_code"] = "";
	_QuoteQueue.add(std::move(empty_quote));
	// 쓰레드가 끝날때까지 기다린다.
	if (m_thread.joinable())
		m_thread.join();
	if (m_runMode)
		m_runMode = false;
}

unsigned int SmQuoteProcessor::ThreadHandlerProc(void)
{
	while (true) {
		// 종료 신호를 보내면 루프를 나간다.
		if (isStop()) {
			break;
		}
		nlohmann::json quote;
		_QuoteQueue.take(quote);
		ProcessQuote(std::move(quote));
	}

	return 1;
}

void SmQuoteProcessor::AddQuote(nlohmann::json&& quote) noexcept
{
	if (!_Enable) return;

	_QuoteQueue.try_add(quote);
}

void SmQuoteProcessor::ProcessQuote(nlohmann::json&& quote)
{
	try {
		const std::string& symbol_code = quote["symbol_code"];
		std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
		if (!symbol) return;
		std::shared_ptr<SmQuote> quote_p = mainApp.QuoteMgr()->get_quote(symbol_code);
		quote_p->symbol_code = symbol_code;
		quote_p->symbol_id = symbol->Id();
		quote_p->open = quote["open"];
		quote_p->high = quote["high"];
		quote_p->low = quote["low"];
		quote_p->close = quote["close"];
		//quote_p->pre_day_close = quote["pre_day_close"];

		mainApp.total_position_manager()->update_position(quote_p);

		mainApp.event_hub()->process_quote_event(quote_p);

		SmTick tick;
		tick.symbol_id = symbol->Id();
		tick.close = quote["close"];
		tick.time = quote["time"];
		tick.qty = quote["volume"];
		tick.updown = quote["up_down"];

		symbol->quote_requested(true);

		mainApp.event_hub()->process_tick_event(tick);
		
	}
	catch (const std::exception & e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

bool SmQuoteProcessor::ProcessQuote(const std::array<nlohmann::json, BulkQuoteSize> & arr, const int& taken)
{
	
	return true;
}

void DarkHorse::SmQuoteProcessor::ClearTasks()
{
	_QuoteQueue.flush();
}

void DarkHorse::SmQuoteProcessor::SendQuoteChangeEvent()
{
	for (auto it = _WndQuoteSet.begin(); it != _WndQuoteSet.end(); ++it) {
		HWND hWnd = *it;
		::SendMessage(hWnd, WM_QUOTE_CHANGED, 0, 0);
	}
}
