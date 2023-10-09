#include "stdafx.h"
#include "SmServerDataReceiver.h"
#include "SmTaskConst.h"
#include "../Json/json.hpp"
#include "IProgressDialog.h"
#include <chrono>
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "SmTaskRequestMaker.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../MainFrm.h"
#include "../Log/MyLogger.h"
#include "../Symbol/SymbolManager.h"

using namespace std::chrono;
class CMainFrame;
using namespace DarkHorse;

int SmServerDataReceiver::_TaskId = 0;

void DarkHorse::SmServerDataReceiver::StartDataRequest()
{
	//SetClient(mainApp.Client());
	std::string file_name = "chocode.cod";
	RegisterGroupTask(SmTaskRequestMaker::MakeMasterFileDownloadRequest(file_name));
	RegisterGroupTask(SmTaskRequestMaker::MakeTaskRequest(SmTaskType::FileDownload));
	DoGroupTask();
}

void DarkHorse::SmServerDataReceiver::DoGroupTask()
{
	// 등록된 작업이 없다면 대화상자를 감추고 아무일도 하지 않는다.
	if (_TaskGroupQue.empty()) {
		ShowProgressWindow(false);
		return;
	}
	// 이미 타이머가 동작중이면 아무일도 하지 않는다.
	//if (!_TimerMap.empty()) return;

	ShowProgressWindow(true);

	// 가장 최근에 등록된 작업을 가져온다.
	const auto top = _TaskGroupQue.front();
	SetTaskState(top);
	// 태스크 타이머 객체를 만들고 작업을 시작한다.
	StartTimer(top->TimerInterval);
}

void DarkHorse::SmServerDataReceiver::RegisterGroupTask(const std::vector<std::shared_ptr<SmTaskInfo>>& group_task_vector)
{
	for (const auto& task : group_task_vector)
		_TaskGroupQue.push_back(task);
}

void DarkHorse::SmServerDataReceiver::RegisterGroupTask(const std::shared_ptr<SmTaskInfo>& group_task)
{
	_TaskGroupQue.push_back(group_task);
}

void DarkHorse::SmServerDataReceiver::ShowProgressWindow(bool show)
{
	if (!_ProgressDlg) return;

	_ProgressDlg->ShowHide(show);
}

void DarkHorse::SmServerDataReceiver::CheckGroupTask()
{
	if (_TaskGroupQue.empty()) {
		// 타이머를 정지 시킨다.
		StopTimer();
		return;
	}

	auto top = _TaskGroupQue.front();
	// 현재 그룹의 일이 끝났을 때의 처리
	if (top->argMap.empty()) {
		// 타이머를 정지 시킨다.
		StopTimer();
		// 사후 작업을 한다.
		DoPostTask(top);
		// 최상위 그룹 작업을 없앤다.
		if (!_TaskGroupQue.empty())
			_TaskGroupQue.pop_front();
		// 다음 작업을 진행한다.
		DoGroupTask();
		return;
	}
}

void DarkHorse::SmServerDataReceiver::CheckFailedRequest()
{
	if (_TryCount > RetryCountLimit) {
		RemoveTopTaskItem();
	}
}

void DarkHorse::SmServerDataReceiver::RemoveTopTaskItem()
{
	auto top = _TaskGroupQue.front();
	if (top->argMap.size() == 0) {
		_TryCount = 0;
		_Working = false;

		if (_TaskGroupQue.empty()) {
			ShowProgressWindow(false);
			return;
		}
		return;
	}
	// 요청이 완료된 일은 찾아 내어 맵에서 없애 준다.
	const auto it = top->argMap.begin();
	// 클라이언트 맵에서 현재 진행중인 업무를 제거한다.
	mainApp.Client()->RemoveTask(it->first);
	std::unique_lock<std::mutex> lock_(_TaskMutex);
	top->argMap.erase(it);
	lock_.unlock();
	_TryCount = 0;
	_Working = false;
}

void DarkHorse::SmServerDataReceiver::StartTimer(const int& interval)
{
	if (!_TimerMap.empty()) StopTimer();
	
	_TimerMap[1] = _Timer.add(milliseconds(interval), std::bind(&SmServerDataReceiver::OnTimer, this), milliseconds(interval));
}

void DarkHorse::SmServerDataReceiver::StopTimer()
{
	const auto it = _TimerMap.find(1);
	if (it == _TimerMap.end()) return;
	
	_Timer.remove(it->second);
}

void DarkHorse::SmServerDataReceiver::DoNextTask()
{
	
	CheckFailedRequest();

	CheckGroupTask();

	if (_TaskGroupQue.empty()) return;

	auto top = _TaskGroupQue.front();
	if (top->argMap.empty()) return;

	std::unique_lock<std::mutex> lock_(_TaskMutex);
	
	// 작업을 복사한다.
	task_arg task_item = top->argMap.begin()->second;
	lock_.unlock();

	if (std::any_cast<bool>(task_item["requested"]))
		return;

	task_arg& req_ref = top->argMap.begin()->second;
	req_ref["requested"] = true;

	_Working = true;
	// 정보창을 설정한다.
	top->RemainTaskCount = top->argMap.size();
	SetTaskDetail(top, task_item);
	SetTaskState(top);
	ExecTask(top->TaskType, std::move(task_item));

}

void DarkHorse::SmServerDataReceiver::OnTaskComplete(const int& task_id)
{
	
	if (_TaskGroupQue.empty()) { _Working = false; return; }

	auto top = _TaskGroupQue.front();

	
	// 요청이 완료된 일은 찾아 내어 맵에서 없애 준다.
	{
		std::unique_lock<std::mutex> lock_(_TaskMutex);
		const auto it = top->argMap.find(task_id);
		if (it != top->argMap.end()) {
			top->argMap.erase(it);
		}
	}

	// 정보창을 설정한다.
	top->RemainTaskCount = top->argMap.size();
	SetTaskState(top);

	_Working = false;
	_TryCount = 0;

	CString msg;
	msg.Format("title = %s, arg count = %d\n", top->TaskTitle.c_str(), top->argMap.size());
	//TRACE(msg);
	
	CheckGroupTask();
}

void DarkHorse::SmServerDataReceiver::ExecTask(const SmTaskType& task_type, task_arg&& task)
{	
	mainApp.Client()->ExecTask(task_type, std::move(task));
}

void DarkHorse::SmServerDataReceiver::DoPostTask(const std::shared_ptr<SmTaskInfo>& group_task)
{
	switch(group_task->TaskType)
	{
	case SmTaskType::FileDownload: 
	{
		mainApp.SymMgr()->MakeDomesticMarket();
		mainApp.SymMgr()->ReadAbroadSymbols();
		mainApp.SymMgr()->read_domestic_productfile();
		mainApp.SymMgr()->read_domestic_masterfile();
		mainApp.SymMgr()->sort_dm_option_symbol_vector();
		//((CMainFrame*)AfxGetMainWnd())->SetMarketTree();
		std::vector<std::shared_ptr<SmTaskInfo>> task_list;
		SmTaskRequestMaker::MaketInitialBatchTask(task_list);
		RegisterGroupTask(task_list);
	}
	break;
	case SmTaskType::AcceptedOrderList:
	{
		if (_ProgressDlg) _ProgressDlg->ShowHide(false);
		mainApp.SymMgr()->MakeAbFavorite();
		//auto req = SmTaskRequestMaker::MakeDefaultChartDataRequest(1500);
		//_TaskGroupQue.push_back(req);
		((CMainFrame*)AfxGetMainWnd())->StartLoad();
	}
	break;
	case SmTaskType::ChartData: 
	{
		if (_ProgressDlg) _ProgressDlg->ShowHide(false);
		((CMainFrame*)AfxGetMainWnd())->StartLoad();
	}
	case SmTaskType::FilledOrderList:
	{
		//if (_ProgressDlg) _ProgressDlg->ShowHide(false);
		//mainApp.SymMgr()->MakeFavorite();
		//((CMainFrame*)AfxGetMainWnd())->StartLoad();
	}
	break;
	}

}

void DarkHorse::SmServerDataReceiver::SetTaskState(const std::shared_ptr<SmTaskInfo>& group_task) const
{
	if (!_ProgressDlg) return;

	_ProgressDlg->SetTaskTitle(group_task->TaskTitle);
	_ProgressDlg->SetTaskDetail(group_task->TaskDetail);
	_ProgressDlg->SetTotalCount(group_task->TotalTaskCount);
	_ProgressDlg->SetRemainCount(group_task->RemainTaskCount);
	_ProgressDlg->RefreshProgress();
}

void DarkHorse::SmServerDataReceiver::SetTaskDetail(const std::shared_ptr<SmTaskInfo>& group_task, task_arg& cur_arg) const
{
	try {
		switch (group_task->TaskType)
		{
		case SmTaskType::DomesticSymbolCode:
			group_task->TaskDetail = std::any_cast<std::string>(cur_arg["product_code"]);
			break;
		case SmTaskType::FileDownload:
			group_task->TaskDetail = std::any_cast<std::string>(cur_arg["range"]);
			break;
		case SmTaskType::Market:
			break;
		case SmTaskType::SymbolCode:
			break;
		case SmTaskType::SymbolMaster:
			group_task->TaskDetail = std::any_cast<std::string>(cur_arg["symbol_name_kr"]) + " : " + std::any_cast<std::string>(cur_arg["symbol_code"]);
			break;
		case SmTaskType::AccountAsset:
			group_task->TaskDetail = std::any_cast<std::string>(cur_arg["account_name"]) + " : " + std::any_cast<std::string>(cur_arg["account_no"]);
			break;
		case SmTaskType::AccountProfitLoss:
			group_task->TaskDetail = std::any_cast<std::string>(cur_arg["account_name"]) + " : " + std::any_cast<std::string>(cur_arg["account_no"]);
			break;
		case SmTaskType::SymbolProfitLoss:
			group_task->TaskDetail = std::any_cast<std::string>(cur_arg["account_name"]) + " : " + std::any_cast<std::string>(cur_arg["account_no"]);
			break;
		case SmTaskType::SymbolQuote:
			group_task->TaskDetail = std::any_cast<std::string>(cur_arg["symbol_name_kr"]) + " : " + std::any_cast<std::string>(cur_arg["symbol_code"]);
			break;
		case SmTaskType::SymbolHoga:
			group_task->TaskDetail = std::any_cast<std::string>(cur_arg["symbol_name_kr"]) + " : " + std::any_cast<std::string>(cur_arg["symbol_code"]);
			break;
		case SmTaskType::ChartData:
			group_task->TaskDetail = std::any_cast<std::string>(cur_arg["symbol_code"]);
			break;
		case SmTaskType::AcceptedOrderList:
			group_task->TaskDetail = std::any_cast<std::string>(cur_arg["account_name"]) + " : " + std::any_cast<std::string>(cur_arg["account_no"]);
			break;
		case SmTaskType::FilledOrderList:
			group_task->TaskDetail = std::any_cast<std::string>(cur_arg["account_name"]) + " : " + std::any_cast<std::string>(cur_arg["account_no"]);
			break;
		case SmTaskType::SymbolPosition:
			break;
		}
	}
	catch (const std::exception & e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void DarkHorse::SmServerDataReceiver::OnTimer()
{
	_TryCount++;
	
	CString msg;
	msg.Format("TryCount = %d\n", _TryCount);
	//TRACE(msg);

	DoNextTask();
}

