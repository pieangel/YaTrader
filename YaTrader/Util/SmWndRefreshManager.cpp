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
	// ���� ť�� ����.
	ClearTasks();
	m_stop = true;
	
	_CmdQueue.add(0);
	// �����尡 ���������� ��ٸ���.
	if (m_thread.joinable())
		m_thread.join();
	if (m_runMode)
		m_runMode = false;
}

unsigned int SmWndRefreshManager::ThreadHandlerProc(void)
{
	while (true) {
		// ���� ��ȣ�� ������ ������ ������.
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