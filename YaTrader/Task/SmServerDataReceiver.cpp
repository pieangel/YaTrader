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
	// ��ϵ� �۾��� ���ٸ� ��ȭ���ڸ� ���߰� �ƹ��ϵ� ���� �ʴ´�.
	if (_TaskGroupQue.empty()) {
		ShowProgressWindow(false);
		return;
	}
	// �̹� Ÿ�̸Ӱ� �������̸� �ƹ��ϵ� ���� �ʴ´�.
	//if (!_TimerMap.empty()) return;

	ShowProgressWindow(true);

	// ���� �ֱٿ� ��ϵ� �۾��� �����´�.
	const auto top = _TaskGroupQue.front();
	SetTaskState(top);
	// �½�ũ Ÿ�̸� ��ü�� ����� �۾��� �����Ѵ�.
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
		// Ÿ�̸Ӹ� ���� ��Ų��.
		StopTimer();
		return;
	}

	auto top = _TaskGroupQue.front();
	// ���� �׷��� ���� ������ ���� ó��
	if (top->argMap.empty()) {
		// Ÿ�̸Ӹ� ���� ��Ų��.
		StopTimer();
		// ���� �۾��� �Ѵ�.
		DoPostTask(top);
		// �ֻ��� �׷� �۾��� ���ش�.
		if (!_TaskGroupQue.empty())
			_TaskGroupQue.pop_front();
		// ���� �۾��� �����Ѵ�.
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
	// ��û�� �Ϸ�� ���� ã�� ���� �ʿ��� ���� �ش�.
	const auto it = top->argMap.begin();
	// Ŭ���̾�Ʈ �ʿ��� ���� �������� ������ �����Ѵ�.
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
	
	// �۾��� �����Ѵ�.
	task_arg task_item = top->argMap.begin()->second;
	lock_.unlock();

	if (std::any_cast<bool>(task_item["requested"]))
		return;

	task_arg& req_ref = top->argMap.begin()->second;
	req_ref["requested"] = true;

	_Working = true;
	// ����â�� �����Ѵ�.
	top->RemainTaskCount = top->argMap.size();
	SetTaskDetail(top, task_item);
	SetTaskState(top);
	ExecTask(top->TaskType, std::move(task_item));

}

void DarkHorse::SmServerDataReceiver::OnTaskComplete(const int& task_id)
{
	
	if (_TaskGroupQue.empty()) { _Working = false; return; }

	auto top = _TaskGroupQue.front();

	
	// ��û�� �Ϸ�� ���� ã�� ���� �ʿ��� ���� �ش�.
	{
		std::unique_lock<std::mutex> lock_(_TaskMutex);
		const auto it = top->argMap.find(task_id);
		if (it != top->argMap.end()) {
			top->argMap.erase(it);
		}
	}

	// ����â�� �����Ѵ�.
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

