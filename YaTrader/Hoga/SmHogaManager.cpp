#include "stdafx.h"
#include "SmHogaManager.h"
#include "SmHogaProcessor.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmTotalManager.h"
#include "../Log/MyLogger.h"

using namespace DarkHorse;

SmHogaManager::SmHogaManager()
{
	StartProcess();
}

SmHogaManager::~SmHogaManager()
{
	StopProcess();
}

void SmHogaManager::StartProcess() noexcept
{
	start();
}

std::shared_ptr<SmHoga> SmHogaManager::add_hoga(const std::string& symbol_code)
{
	std::shared_ptr<SmHoga> hoga_ptr = std::make_shared<SmHoga>();
	hoga_map_[symbol_code] = hoga_ptr;
	return hoga_ptr;
}
std::shared_ptr<SmHoga> SmHogaManager::find_hoga(const std::string& symbol_code)
{
	auto found = hoga_map_.find(symbol_code);
	if (found != hoga_map_.end()) return found->second;
	return nullptr;
}
std::shared_ptr<SmHoga> SmHogaManager::get_hoga(const std::string& symbol_code)
{
	auto hoga_ptr = find_hoga(symbol_code);
	if (hoga_ptr) return hoga_ptr;
	return add_hoga(symbol_code);
}

void SmHogaManager::StopProcess() noexcept
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

unsigned int SmHogaManager::ThreadHandlerProc(void)
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

void SmHogaManager::AddHoga(nlohmann::json&& hoga) noexcept
{
	if (!_Enable) return;

	_HogaQueue.try_add(hoga);
}

void SmHogaManager::ProcessHoga(nlohmann::json&& hoga)
{
	try {
		const std::string& symbol_code = hoga["symbol_code"];
		const auto found = _HogaProcessorMap.find(symbol_code);
		if (found != _HogaProcessorMap.end()) {
			const std::shared_ptr<SmHogaProcessor>& hoga_processor = found->second;
			hoga_processor->AddHoga(std::move(hoga));
		}
		else {
			std::shared_ptr<SmHogaProcessor> hoga_processor = std::make_shared<SmHogaProcessor>();
			_HogaProcessorMap[symbol_code] = hoga_processor;
			hoga_processor->StartProcess();
			hoga_processor->AddHoga(std::move(hoga));
		}
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

bool SmHogaManager::ProcessHoga(const std::array<nlohmann::json, BulkHogaTaskSize>& arr, const int& taken)
{

	return true;
}

void DarkHorse::SmHogaManager::StopAllHogaProcess()
{
	for (auto it = _HogaProcessorMap.begin(); it != _HogaProcessorMap.end(); it++) {
		it->second->StopProcess();
	}
}

void DarkHorse::SmHogaManager::ClearTasks()
{
	_HogaQueue.flush();
}