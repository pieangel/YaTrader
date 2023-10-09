// VtProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VtProgressDlg.h"
#include "../resource.h"

#include "../MainFrm.h"
// VtProgressDlg dialog

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


UINT threadfunc(LPVOID pp)
{
	//VtProgressDlg* dlg = (VtProgressDlg*)pp;
	//HdScheduler* taskMgr = HdScheduler::GetInstance();
	//taskMgr->ProgressDlg(dlg);
	//Sleep(2000);
	//dlg->PostMessage(WM_CLOSE, 0, 0);
	return 0;
}

IMPLEMENT_DYNAMIC(VtProgressDlg, CBCGPDialog)

VtProgressDlg::VtProgressDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_PROGRESS, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
}

VtProgressDlg::~VtProgressDlg()
{
}

void VtProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STATE, _StaticState);
	DDX_Control(pDX, IDC_PROGRESS_CTRL, _PrgCtrl);
	DDX_Control(pDX, IDC_STATIC_TASK, _StaticTask);
}


BEGIN_MESSAGE_MAP(VtProgressDlg, CBCGPDialog)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// VtProgressDlg message handlers


BOOL VtProgressDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableLayout();
	VERIFY(_BrushBackNor.CreateSolidBrush(RGB(45, 45, 48)));
	// TODO:  Add extra initialization here
	//::AfxBeginThread((AFX_THREADPROC)threadfunc, (LPVOID)this);
	_StaticState.SetTextColor(RGB(255, 255, 255));
	_StaticState.SetColor(RGB(45, 45, 48));
	_StaticState.SetGradientColor(RGB(45, 45, 48));
	_StaticState.SetTextAlign(1);
	_StaticState.SetWindowText("작업중입니다.");

	_StaticTask.SetTextColor(RGB(255, 255, 255));
	_StaticTask.SetColor(RGB(45, 45, 48));
	_StaticTask.SetGradientColor(RGB(45, 45, 48));
	_StaticTask.SetTextAlign(1);

	//HdScheduler* taskMgr = HdScheduler::GetInstance();
	//taskMgr->ProgressDlg(this);
	//taskMgr->GetSymbolCode();
	//taskMgr->GetSymbolFile();

	CenterWindow(GetDesktopWindow());

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtProgressDlg::CloseDialog()
{
// 	if (MainFrm) {
// 		MainFrm->OnReceiveComplete();
// 	}
	EndDialog(1);
}

void VtProgressDlg::SetPrgressPos(int pos)
{
	_PrgCtrl.SetPos(pos);
}


void VtProgressDlg::SetTaskInfo(std::string msg)
{
	_StaticState.SetWindowText(msg.c_str());
}


void VtProgressDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	//delete this;
	CBCGPDialog::PostNcDestroy();
}


void VtProgressDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	DestroyWindow();
	CBCGPDialog::OnCancel();
}


HBRUSH VtProgressDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hBrush = NULL;
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_STATE || 
		pWnd->GetDlgCtrlID() == IDC_STATIC_TASK)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(3, 30, 82));
	}
	_StaticState.SetTextAlign(1);
	_StaticTask.SetTextAlign(1);
	hBrush = (HBRUSH)_BrushBackNor;
	return hBrush;
}

void VtProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	//SmTaskManager::GetInstance()->ExecuteTask();
	CBCGPDialog::OnTimer(nIDEvent);
}

void VtProgressDlg::RefreshProgress()
{
	_StaticTask.SetWindowText(_TaskTitle.c_str());
	_StaticState.SetWindowText(_TaskDetail.c_str());
	double top = (double)_TotalCount - (double)_RemainCount;
	double entire = top / (double)_TotalCount;
	int percent = static_cast<int>(entire * 100);
	SetPrgressPos(percent);
}
