// VtAutoSignalManagerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmAutoSignalManagerDialog.h"
#include "afxdialogex.h"
#include "SmAddConnectSignalDlg.h"
#include "SmAddOutSigDefDlg.h"
#include "../Global/SmConst.h"

#define WND_ID20 0x00000020
#define WND_ID21 0x00000021
#define WND_ID22 0x00000022
// VtAutoSignalManagerDialog dialog

IMPLEMENT_DYNAMIC(SmAutoSignalManagerDialog, CBCGPDialog)

SmAutoSignalManagerDialog::SmAutoSignalManagerDialog(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_SYS_AUTO_CONNECT, pParent)
{

}

SmAutoSignalManagerDialog::~SmAutoSignalManagerDialog()
{
}

void SmAutoSignalManagerDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ALL, _CheckAll);
	//DDX_Control(pDX, IDC_STATIC_ACTIVE_OUT_SYSTEM, active_out_system_view_);
	//DDX_Control(pDX, IDC_STATIC_OUT_SYSTEM, out_system_view_);
	//DDX_Control(pDX, IDC_STATIC_OUT_SYSTEM_DEF, out_system_def_view_);
}


void SmAutoSignalManagerDialog::PostNcDestroy()
{
	//delete this;
	CBCGPDialog::PostNcDestroy();
}

BEGIN_MESSAGE_MAP(SmAutoSignalManagerDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_CONNECT, &SmAutoSignalManagerDialog::OnBnClickedBtnAddConnect)
	ON_BN_CLICKED(IDC_BTN_ADD_SIGNAL, &SmAutoSignalManagerDialog::OnBnClickedBtnAddSignal)
	ON_BN_CLICKED(IDC_BTN_DEL_CONNECT, &SmAutoSignalManagerDialog::OnBnClickedBtnDelConnect)
	ON_BN_CLICKED(IDC_BTN_DEL_SIGNAL, &SmAutoSignalManagerDialog::OnBnClickedBtnDelSignal)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_ORDER_CONFIG, &SmAutoSignalManagerDialog::OnBnClickedBtnOrderConfig)
	ON_BN_CLICKED(IDC_CHECK_ALL, &SmAutoSignalManagerDialog::OnBnClickedCheckAll)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// VtAutoSignalManagerDialog message handlers


BOOL SmAutoSignalManagerDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_OUT_SYSTEM);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	// Create the Windows control and attach it to the Grid object
	out_system_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID20);
	out_system_view_.parent_dlg(this);

	pWnd = GetDlgItem(IDC_STATIC_ACTIVE_OUT_SYSTEM);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	// Create the Windows control and attach it to the Grid object
	active_out_system_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID21);

	pWnd = GetDlgItem(IDC_STATIC_OUT_SYSTEM_DEF);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	// Create the Windows control and attach it to the Grid object
	out_system_def_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID22);
	out_system_def_view_.init_grid();

	SetTimer(RefTimer, 100, NULL);
	Resize();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmAutoSignalManagerDialog::RefreshOrder()
{
// 	_TotalSigGrid.RefreshOrders();
// 	_ConnectGrid.RefreshOrders();
}

void SmAutoSignalManagerDialog::Resize()
{
	CRect rcDlg;
	this->GetClientRect(rcDlg);

	int horizontal_position = rcDlg.Height() / 2;
	int vertical_position = rcDlg.Width() * 77 / 96;
	int controlHeight = 32;

	// 위쪽 타이틀
	CRect rcCtrl;
	CWnd* pWnd = this->GetDlgItem(IDC_STATIC_RUN_LIST);
	if (!pWnd->GetSafeHwnd())
		return;

	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP + 7;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 주문설정 버튼
	pWnd = this->GetDlgItem(IDC_BTN_ORDER_CONFIG);

	int left = rcDlg.right - STD_GAP - 100;
	rcCtrl.left = left;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 포지션 청산 버튼
	pWnd = this->GetDlgItem(IDC_BUTTON_LIQ_POSITION);

	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 신호 합계 버튼
	pWnd = this->GetDlgItem(IDC_BUTTON_SUM_SIGNAL);


	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 상세표시 체크 버튼
	pWnd = this->GetDlgItem(IDC_CHECK_DETAIL);


	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP + 2;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 자동주문 체크 표시 버튼
	pWnd = this->GetDlgItem(IDC_CHECK_AUTO_ORDER);

	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP + 2;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;

	// 실행 목록 그리드
	pWnd = this->GetDlgItem(IDC_STATIC_ACTIVE_OUT_SYSTEM);


	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = rcDlg.right - STD_GAP;
	rcCtrl.top = rcDlg.top + STD_GAP + controlHeight;
	rcCtrl.bottom = horizontal_position - STD_GAP;
	pWnd->MoveWindow(rcCtrl, TRUE);
	active_out_system_view_.MoveWindow(rcCtrl, TRUE);

	// 분할 왼쪽 타이틀
	rcCtrl;
	pWnd = this->GetDlgItem(IDC_STATIC_CONNECT_SIGNAL_CHART);

	int top = horizontal_position + STD_GAP;
	rcCtrl.top = top + 7;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.right;

	// 모두 선택 버튼 
	pWnd = this->GetDlgItem(IDC_CHECK_ALL);
	rcCtrl.top = top + 2;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = left + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 왼쪽 삭제 버튼 
	pWnd = this->GetDlgItem(IDC_BTN_DEL_CONNECT);
	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;

	left = vertical_position - STD_GAP - 60;

	rcCtrl.left = left;
	rcCtrl.right = rcCtrl.left + 60;
	pWnd->MoveWindow(rcCtrl, TRUE);


	// 왼쪽 추가 버튼
	pWnd = this->GetDlgItem(IDC_BTN_ADD_CONNECT);

	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = left - STD_GAP - 60;
	rcCtrl.right = rcCtrl.left + 60;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 왼쪽 그리드 목록
	pWnd = this->GetDlgItem(IDC_STATIC_OUT_SYSTEM);


	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = vertical_position - STD_GAP;
	rcCtrl.top = horizontal_position + STD_GAP + controlHeight;
	rcCtrl.bottom = rcDlg.bottom - STD_GAP;
	pWnd->MoveWindow(rcCtrl, TRUE);
	out_system_view_.MoveWindow(rcCtrl, TRUE);




	// 오른쪽 타이틀
	pWnd = this->GetDlgItem(IDC_STATIC_SIGNAL_CHART);

	rcCtrl.top = top + 7;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = vertical_position + STD_GAP;
	rcCtrl.right = rcCtrl.left + 100;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 오른쪽 삭제 버튼
	pWnd = this->GetDlgItem(IDC_BTN_DEL_SIGNAL);

	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = rcDlg.right - STD_GAP - 60;
	rcCtrl.right = rcCtrl.left + 60;
	pWnd->MoveWindow(rcCtrl, TRUE);

	left = rcDlg.right - STD_GAP - 60;

	// 오른쪽 추가 버튼
	pWnd = this->GetDlgItem(IDC_BTN_ADD_SIGNAL);

	rcCtrl.top = top;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	rcCtrl.left = left - STD_GAP - 60;
	rcCtrl.right = rcCtrl.left + 60;
	pWnd->MoveWindow(rcCtrl, TRUE);

	// 오른쪽 그리드 목록
	pWnd = this->GetDlgItem(IDC_STATIC_OUT_SYSTEM_DEF);


	rcCtrl.left = vertical_position + STD_GAP;
	rcCtrl.right = rcDlg.right - STD_GAP;
	rcCtrl.top = horizontal_position + STD_GAP + controlHeight;
	rcCtrl.bottom = rcDlg.bottom - STD_GAP;
	pWnd->MoveWindow(rcCtrl, TRUE);
	out_system_def_view_.MoveWindow(rcCtrl, TRUE);

	Invalidate(TRUE);
}

void SmAutoSignalManagerDialog::add_active_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system)
{
	active_out_system_view_.add_out_system(out_system);
}

void SmAutoSignalManagerDialog::remove_active_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system)
{
	active_out_system_view_.remove_out_system(out_system);
}

void SmAutoSignalManagerDialog::add_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system)
{
	if (!out_system) return;

	out_system_view_.add_out_system(out_system);
}

void SmAutoSignalManagerDialog::OnBnClickedBtnAddConnect()
{
	SmAddConnectSignalDlg dlg;
	dlg.source_dialog(this);
	dlg.DoModal();
}


void SmAutoSignalManagerDialog::OnBnClickedBtnAddSignal()
{
	SmAddOutSigDefDlg dlg;
	//dlg.SigDefGrid(&_DefineGrid);
	dlg.DoModal();
}


void SmAutoSignalManagerDialog::OnBnClickedBtnDelConnect()
{
	auto out_system = out_system_view_.get_selected_out_system();
	if (out_system) active_out_system_view_.remove_out_system(out_system);
	out_system_view_.remove_out_system();
}


void SmAutoSignalManagerDialog::OnBnClickedBtnDelSignal()
{
	// TODO: Add your control notification handler code here
}


void SmAutoSignalManagerDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == RefTimer) {
		RefreshOrder();
	}
	CBCGPDialog::OnTimer(nIDEvent);
}


void SmAutoSignalManagerDialog::OnClose()
{
	KillTimer(RefTimer);
	CBCGPDialog::OnClose();
}

void SmAutoSignalManagerDialog::OnBnClickedBtnOrderConfig()
{
// 	VtSystemOrderConfig orderCfgDlg;
// 	orderCfgDlg._PriceType = VtOutSystemManager::PriceType;
// 	orderCfgDlg._OrderTick = VtOutSystemManager::OrderTick;
// 	int result = orderCfgDlg.DoModal();
// 	if (result == IDOK) {
// 		VtOutSystemManager::PriceType = orderCfgDlg._PriceType;
// 		VtOutSystemManager::OrderTick = orderCfgDlg._OrderTick;
// 
// 		VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
// 		SharedSystemVec& outSysMap = outSysMgr->GetSysMap();
// 		for (auto it = outSysMap.begin(); it != outSysMap.end(); ++it) {
// 			SharedSystem sys = *it;
// 			sys->PriceType(orderCfgDlg._PriceType);
// 			sys->OrderTick(orderCfgDlg._OrderTick);
// 		}
// 	}
}


void SmAutoSignalManagerDialog::OnBnClickedCheckAll()
{
// 	if (_CheckAll.GetCheck() == BST_CHECKED) {
// 		_ConnectGrid.SetCheck(true);
// 	}
// 	else {
// 		_ConnectGrid.SetCheck(false);
// 	}
}


void SmAutoSignalManagerDialog::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	this->Resize();
}
