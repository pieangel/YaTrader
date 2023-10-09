#pragma once
#include "../Common/BlockingCollection.h"
#include "../Common/common.h"
#include "../Json/json.hpp"
#include "SmTaskDefine.h"
namespace DarkHorse {
	using namespace code_machina;
	const int BulkReqSize = 100;
	class SmTaskRequestManager : public Runnable
	{
	public:
		SmTaskRequestManager();
		~SmTaskRequestManager() override;
		void StartProcess() noexcept;
		void StopProcess() noexcept;
		unsigned int ThreadHandlerProc(void) override;
		void AddTask(DhTaskArg&& task) noexcept;
		bool Enable() const { return _Enable; }
		void Enable(bool val) { _Enable = val; }
	private:
		void ClearTasks() noexcept;
		bool _Enable{ true };
		bool _BatchProcess{ false };
		BlockingCollection<DhTaskArg> _TaskQueue;
		void ProcessTask(DhTaskArg&& task);
		// arr : 데이터가 들어 있는 배열, taken : 실제 데이터가 들어 있는 갯수
		bool ProcessTask(const std::array<DhTaskArg, BulkReqSize>& arr, const int& taken);
	};
}

