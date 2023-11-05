// SmUSDSystemDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "../DarkHorse.h"
#include "SmUSDSystemDialog.h"
#include "afxdialogex.h"
#include "../Global/SmConst.h"
#include "../Global/SmTotalManager.h"
#include "../Dialog/SmAddConnectSignalDlg.h"
#include "../Dialog/SmAddOutSigDefDlg.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../Dialog/SmSystemOrderConfig.h"
#include "SmAddUsdSystemDlg.h"
#include "../OutSystem/SmUsdSystem.h"

// SmUSDSystemDialog dialog

IMPLEMENT_DYNAMIC(SmUSDSystemDialog, CBCGPDialog)

SmUSDSystemDialog::SmUSDSystemDialog(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_SYS_AUTO_CONNECT2, pParent)
{

}

SmUSDSystemDialog::~SmUSDSystemDialog()
{
}

void SmUSDSystemDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ALL, _CheckAll);
}


void SmUSDSystemDialog::PostNcDestroy()
{
	//delete this;
	CBCGPDialog::PostNcDestroy();
}

BEGIN_MESSAGE_MAP(SmUSDSystemDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_CONNECT, &SmUSDSystemDialog::OnBnClickedBtnAddConnect)
	ON_BN_CLICKED(IDC_BTN_ADD_SIGNAL, &SmUSDSystemDialog::OnBnClickedBtnAddSignal)
	ON_BN_CLICKED(IDC_BTN_DEL_CONNECT, &SmUSDSystemDialog::OnBnClickedBtnDelConnect)
	ON_BN_CLICKED(IDC_BTN_DEL_SIGNAL, &SmUSDSystemDialog::OnBnClickedBtnDelSignal)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_ORDER_CONFIG, &SmUSDSystemDialog::OnBnClickedBtnOrderConfig)
	ON_BN_CLICKED(IDC_CHECK_ALL, &SmUSDSystemDialog::OnBnClickedCheckAll)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// SmUSDSystemDialog message handlers


BOOL SmUSDSystemDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	active_usd_system_grid_.AttachGrid(this, IDC_STATIC_TOTAL_SIGNAL);
	_UsdSystemDefGrid.AttachGrid(this, IDC_STATIC_SIGNAL_CONNECTION);
	_UsdSystemDefGrid.Active_usd_system_grid(&active_usd_system_grid_);
	active_usd_system_grid_.UsdGrid(&_UsdSystemDefGrid);
	SetTimer(RefTimer, 100, NULL);
	Resize();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmUSDSystemDialog::RefreshOrder()
{
	active_usd_system_grid_.RefreshOrders();
	_UsdSystemDefGrid.RefreshOrders();
}

void SmUSDSystemDialog::Resize()
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
	/*
	pWnd = this->GetDlgItem(IDC_BUTTON_SUM_SIGNAL);


	rcCtrl.left = left - STD_GAP - 100;
	rcCtrl.right = rcCtrl.left + 100;
	rcCtrl.top = rcDlg.top + STD_GAP;
	rcCtrl.bottom = rcCtrl.top + STD_BUTTON_HEIGHT;
	pWnd->MoveWindow(rcCtrl, TRUE);
	left = rcCtrl.left;
	*/
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
	pWnd = this->GetDlgItem(IDC_STATIC_TOTAL_SIGNAL);


	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = rcDlg.right - STD_GAP;
	rcCtrl.top = rcDlg.top + STD_GAP + controlHeight;
	rcCtrl.bottom = horizontal_position - STD_GAP;
	pWnd->MoveWindow(rcCtrl, TRUE);

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
	pWnd = this->GetDlgItem(IDC_STATIC_SIGNAL_CONNECTION);


	rcCtrl.left = rcDlg.left + STD_GAP;
	rcCtrl.right = rcDlg.right - STD_GAP;
	rcCtrl.top = horizontal_position + STD_GAP + controlHeight;
	rcCtrl.bottom = rcDlg.bottom - STD_GAP;
	pWnd->MoveWindow(rcCtrl, TRUE);

	active_usd_system_grid_.GetScrollBarCtrl(SB_VERT)->Invalidate(TRUE);
	_UsdSystemDefGrid.GetScrollBarCtrl(SB_VERT)->Invalidate(TRUE);

	Invalidate(TRUE);
}

void SmUSDSystemDialog::add_usd_system(std::shared_ptr<DarkHorse::SmUsdSystem> usd_system)
{
	if (!usd_system) return;
	if (!usd_system->symbol()) return;
	if (usd_system->order_type() == DarkHorse::OrderType::MainAccount && !usd_system->account()) return;
	if (usd_system->order_type() == DarkHorse::OrderType::SubAccount && !usd_system->account()) return;
	if (usd_system->order_type() == DarkHorse::OrderType::Fund && !usd_system->fund()) return;
	size_t out_system_count = mainApp.out_system_manager()->get_usd_system_vector().size();
	if (out_system_count == 0) return;

	_UsdSystemDefGrid.AddSystem(usd_system);
}

void SmUSDSystemDialog::OnBnClickedBtnAddConnect()
{
	SmAddUsdSystemDlg dlg;
	dlg.auto_connect_dialog(this);
	dlg.DoModal();
}


void SmUSDSystemDialog::OnBnClickedBtnAddSignal()
{
	//VtAddOutSigDefDlg dlg;
	//dlg.SigDefGrid(&_DefineGrid);
	//dlg.DoModal();
}


void SmUSDSystemDialog::OnBnClickedBtnDelConnect()
{
	_UsdSystemDefGrid.RemoveSystem();
	_UsdSystemDefGrid.Refresh();
}


void SmUSDSystemDialog::OnBnClickedBtnDelSignal()
{
	// TODO: Add your control notification handler code here
}


void SmUSDSystemDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == RefTimer) {
		RefreshOrder();
	}
	CBCGPDialog::OnTimer(nIDEvent);
}


void SmUSDSystemDialog::OnClose()
{
	KillTimer(RefTimer);
	CBCGPDialog::OnClose();
}

void SmUSDSystemDialog::OnBnClickedBtnOrderConfig()
{
	SmSystemOrderConfig orderCfgDlg;
	orderCfgDlg._PriceType = DarkHorse::SmOutSystemManager::price_type;
	orderCfgDlg._OrderTick = DarkHorse::SmOutSystemManager::order_tick;
	int result = orderCfgDlg.DoModal();
	if (result == IDOK) {
		DarkHorse::SmOutSystemManager::price_type = orderCfgDlg._PriceType;
		DarkHorse::SmOutSystemManager::order_tick = orderCfgDlg._OrderTick;
	}
}


void SmUSDSystemDialog::OnBnClickedCheckAll()
{
	if (_CheckAll.GetCheck() == BST_CHECKED) {
		_UsdSystemDefGrid.SetCheck(true);
	}
	else {
		_UsdSystemDefGrid.SetCheck(false);
	}
}


void SmUSDSystemDialog::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	this->Resize();
}
