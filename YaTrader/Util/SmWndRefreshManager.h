#pragma once


#include "../Common/BlockingCollection.h"
#include "../Common/common.h"
namespace DarkHorse {
	using namespace code_machina;
	const int BulkCmdSize = 100;
	class SmWndRefreshManager : public Runnable
	{
	public:
		SmWndRefreshManager();
		~SmWndRefreshManager() override;
		void StartProcess() noexcept;
		void StopProcess() noexcept;
		virtual unsigned int ThreadHandlerProc(void);
		void AddCmd(const int& cmd) noexcept;
		bool Enable() const { return _Enable; }
		void Enable(bool val) { _Enable = val; }
	private:
		void ClearTasks() noexcept;
		bool _Enable{ true };
		bool _BatchProcess{ false };
		BlockingCollection<int> _CmdQueue;
		void ProcessCmd(const int& cmd);
		// arr : �����Ͱ� ��� �ִ� �迭, taken : ���� �����Ͱ� ��� �ִ� ����
		bool ProcessCmd(const std::array<int, BulkCmdSize>& arr, const int& taken);
	};
}

