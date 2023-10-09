#include "stdafx.h"
#include "SmQuoteManager.h"
#include "SmQuoteProcessor.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Log/MyLogger.h"
#include "SmQuote.h"

using namespace DarkHorse;

SmQuoteManager::SmQuoteManager()
{
	StartProcess();
}

SmQuoteManager::~SmQuoteManager()
{
	StopProcess();
}

std::shared_ptr<SmQuote> SmQuoteManager::add_quote(const std::string& symbol_code)
{
	std::shared_ptr<SmQuote> quote_ptr = std::make_shared<SmQuote>();
	quote_map_[symbol_code] = quote_ptr;
	return quote_ptr;
}
std::shared_ptr<SmQuote> SmQuoteManager::find_quote(const std::string& symbol_code)
{
	auto found = quote_map_.find(symbol_code);
	if (found != quote_map_.end()) return found->second;
	return nullptr;
}
std::shared_ptr<SmQuote> SmQuoteManager::get_quote(const std::string& symbol_code)
{
	auto quote_ptr = find_quote(symbol_code);
	if (quote_ptr) return quote_ptr;
	return add_quote(symbol_code);
}

void SmQuoteManager::StartProcess() noexcept
{
	start();
}

void SmQuoteManager::StopProcess() noexcept
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

unsigned int SmQuoteManager::ThreadHandlerProc(void)
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

void SmQuoteManager::AddQuote(nlohmann::json&& quote) noexcept
{
	if (!_Enable) return;

	_QuoteQueue.try_add(quote);
}

void SmQuoteManager::ProcessQuote(nlohmann::json&& quote)
{
	try {
		const std::string& symbol_code = quote["symbol_code"];
 		const auto found = _QuoteProcessorMap.find(symbol_code);
		if (found != _QuoteProcessorMap.end()) {
			const std::shared_ptr<SmQuoteProcessor>& quote_processor = found->second;
			quote_processor->AddQuote(std::move(quote));
		}
		else {
			std::shared_ptr<SmQuoteProcessor> quote_processor = std::make_shared<SmQuoteProcessor>();
			_QuoteProcessorMap[symbol_code] = quote_processor;
			quote_processor->StartProcess();
			quote_processor->AddQuote(std::move(quote));
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

bool SmQuoteManager::ProcessQuote(const std::array<nlohmann::json, BulkSize>& arr, const int& taken)
{

	return true;
}

void DarkHorse::SmQuoteManager::SubscribeEvent(const std::string& symbol_code, HWND wnd)
{
	auto it = _QuoteProcessorMap.find(symbol_code);
	if (it != _QuoteProcessorMap.end()) {
		it->second->SubscribeQuoteWndCallback(wnd);
	}
}

void DarkHorse::SmQuoteManager::UnsubscribeEvent(const std::string& symbol_code, HWND wnd)
{
	auto it = _QuoteProcessorMap.find(symbol_code);
	if (it != _QuoteProcessorMap.end()) {
		it->second->UnsubscribeQuoteWndCallback(wnd);
	}
}

void DarkHorse::SmQuoteManager::StopAllQuoteProcess()
{
	for (auto it = _QuoteProcessorMap.begin(); it != _QuoteProcessorMap.end(); it++) {
		it->second->StopProcess();
	}
}

void DarkHorse::SmQuoteManager::ClearTasks()
{
	_QuoteQueue.flush();
}
