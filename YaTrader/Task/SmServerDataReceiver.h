#pragma once
#include <list>
#include <memory>
#include <mutex>
#include <vector>
#include "SmTaskConst.h"
#include "SmTaskInfo.h"
#include "../Time/cpptime.h"
namespace DarkHorse {
	typedef std::map<std::string, std::any> task_arg;
	const int RetryCountLimit = 10;
	// 작업 큐를 운영한다.
	// 큐에 미리 넣을 수도 있고 하나씩 넣을 수도 있다.
	// 하나의 큐를 시작할 때는 일정한 시간 간격으로 실행시킨다.
	// 큐를 실행할때는 타이머로 운영한다.
	class json;
	class IProgressDialog;
	class ViStockClient;
	class SmServerDataReceiver
	{
	public:
		void StartDataRequest();
		void DoGroupTask();
		void RegisterGroupTask(const std::vector<std::shared_ptr<SmTaskInfo>>& group_task_vector);
		void RegisterGroupTask(const std::shared_ptr<SmTaskInfo>& group_task);
		void ProgressDlg(std::shared_ptr<IProgressDialog> val) { _ProgressDlg = val; }
		//void SetClient(std::shared_ptr<ViStockClient> client) { _Client = client; }
		void OnTaskComplete(const int& task_id);
	private:
		void ShowProgressWindow(bool show);
		void CheckGroupTask();
		void CheckFailedRequest();
		void RemoveTopTaskItem();
		void StartTimer(const int& interval);
		void StopTimer();
		void DoNextTask();
		void ExecTask(const SmTaskType& task_type, task_arg&& task);
		void DoPostTask(const std::shared_ptr<SmTaskInfo>& group_task);
		void SetTaskState(const std::shared_ptr<SmTaskInfo>& group_task) const;
		void SetTaskDetail(const std::shared_ptr<SmTaskInfo>& group_task, task_arg& cur_arg) const;
		void OnTimer();
		 
		bool _Working = false;
		std::list<std::shared_ptr<SmTaskInfo>> _TaskGroupQue;
		std::mutex _TaskMutex;
		// 타이머 객체
		CppTime::Timer _Timer;
		// 타이머 아이디
		std::map<int, CppTime::timer_id> _TimerMap;
		//std::shared_ptr<ViStockClient> _Client = nullptr;
		std::shared_ptr<IProgressDialog> _ProgressDlg = nullptr;
		int _TryCount = 0;
	public:
		// Static Members
		static int _TaskId;
		static int GetId() { return _TaskId++; }
	};
}