#include "stdafx.h"
#include "SmWndRefreshManager.h"
#include "../Log/MyLogger.h"
using namespace DarkHorse;

SmWndRefreshManager::SmWndRefreshManager()
{
	StartProcess();
}

SmWndRefreshManager::~SmWndRefreshManager()
{
	StopProcess();
}

void SmWndRefreshManager::StartProcess() noexcept
{
	start();
}

void SmWndRefreshManager::StopProcess() noexcept
{
	if (!m_runMode) return;
	// 먼저 큐를 비운다.
	ClearTasks();
	m_stop = true;
	
	_CmdQueue.add(0);
	// 쓰레드가 끝날때까지 기다린다.
	if (m_thread.joinable())
		m_thread.join();
	if (m_runMode)
		m_runMode = false;
}

unsigned int SmWndRefreshManager::ThreadHandlerProc(void)
{
	while (true) {
		// 종료 신호를 보내면 루프를 나간다.
		if (isStop()) {
			break;
		}
		int cmd;
		_CmdQueue.take(cmd);
		ProcessCmd(cmd);
	}

	return 1;
}

void SmWndRefreshManager::AddCmd(const int& cmd) noexcept
{
	if (!_Enable) return;

	_CmdQueue.try_add(cmd);
}

void SmWndRefreshManager::ProcessCmd(const int& cmd)
{
	try {
		

	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

bool SmWndRefreshManager::ProcessCmd(const std::array<int, BulkCmdSize>& arr, const int& taken)
{

	return true;
}

void DarkHorse::SmWndRefreshManager::ClearTasks() noexcept
{
	_CmdQueue.flush();
}