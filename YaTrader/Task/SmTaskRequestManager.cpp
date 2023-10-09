#include "stdafx.h"
#include "SmTaskRequestManager.h"
#include "../Global/SmTotalManager.h"
#include "../Client/ViStockClient.h"
namespace DarkHorse {

	SmTaskRequestManager::SmTaskRequestManager()
	{
		StartProcess();
	}

	SmTaskRequestManager::~SmTaskRequestManager()
	{
		StopProcess();
	}

	void SmTaskRequestManager::StartProcess() noexcept
	{
		start();
	}

	void SmTaskRequestManager::StopProcess() noexcept
	{
		if (!m_runMode) return;
		// ���� ť�� ����.
		ClearTasks();
		m_stop = true;
		DhTaskArg empty_arg;
		empty_arg.task_type = DhTaskType::None;
		_TaskQueue.add(std::move(empty_arg));
		// �����尡 ���������� ��ٸ���.
		if (m_thread.joinable())
			m_thread.join();
		if (m_runMode)
			m_runMode = false;
	}

	unsigned int SmTaskRequestManager::ThreadHandlerProc(void)
	{
		while (true) {
			// ���� ��ȣ�� ������ ������ ������.
			if (isStop()) {
				break;
			}
			DhTaskArg task;
			_TaskQueue.take(task);
			ProcessTask(std::move(task));
		}

		return 1;
	}

	void SmTaskRequestManager::AddTask(DhTaskArg&& task) noexcept
	{
		if (!_Enable) return;

		_TaskQueue.add(task);
	}

	void SmTaskRequestManager::ClearTasks() noexcept
	{
		_TaskQueue.flush();
	}

	void SmTaskRequestManager::ProcessTask(DhTaskArg&& task)
	{
		mainApp.Client()->ExecTask(std::move(task));
	}

	bool SmTaskRequestManager::ProcessTask(const std::array<DhTaskArg, BulkReqSize>& arr, const int& taken)
	{
		return true;
	}
}
