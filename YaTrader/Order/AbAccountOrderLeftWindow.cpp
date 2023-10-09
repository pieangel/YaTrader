// OrderLeftDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "AbAccountOrderLeftWindow.h"
#include "afxdialogex.h"
#include "../Symbol/SmSymbolTableDialog.h"
#include "AbAccountOrderWindow.h"

#define WND_ID1 0x00000001
#define WND_ID2 0x00000002
#define WND_ID3 0x00000003


// OrderLeftDialog dialog

IMPLEMENT_DYNAMIC(AbAccountOrderLeftWindow, CBCGPDialog)

AbAccountOrderLeftWindow::AbAccountOrderLeftWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_LEFT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

AbAccountOrderLeftWindow::~AbAccountOrderLeftWindow()
{
	//KillTimer(1);
}

void AbAccountOrderLeftWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_ACCEPTED, _AcceptedArea);
	//DDX_Control(pDX, IDC_STATIC_FILLED, _FilledArea);
	//DDX_Control(pDX, IDC_STATIC_FAVORITE, _FavoriteArea);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, account_profit_loss_view_);
	//DDX_Control(pDX, IDC_SCROLLBAR_VER_ACPT, _VScrollBarAcpt);
	//DDX_Control(pDX, IDC_SCROLLBAR_VER_POSI, _VScrollBarPosi);
	//DDX_Control(pDX, IDC_SCROLLBAR_VER_FAV, _VScrollBarFav);
	DDX_Control(pDX, IDC_BTN_ADD_FAV, _BtnAddFav);
	DDX_Control(pDX, IDC_STATIC_FAV, _StaticFav);
}


BEGIN_MESSAGE_MAP(AbAccountOrderLeftWindow, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_FAV, &AbAccountOrderLeftWindow::OnBnClickedBtnAddFav)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &AbAccountOrderLeftWindow::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_CANCEL_SEL, &AbAccountOrderLeftWindow::OnBnClickedBtnCancelSel)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, &AbAccountOrderLeftWindow::OnBnClickedBtnCancelAll)
	ON_BN_CLICKED(IDC_BTN_LIQ_SEL, &AbAccountOrderLeftWindow::OnBnClickedBtnLiqSel)
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &AbAccountOrderLeftWindow::OnBnClickedBtnLiqAll)
END_MESSAGE_MAP()


// OrderLeftDialog message handlers


void AbAccountOrderLeftWindow::SetMainWnd(AbAccountOrderWindow* main_wnd)
{
	if (!main_wnd) return;
	order_window_id_ = main_wnd->get_id();

	favorite_symbol_view_.SetMainWnd(main_wnd);
}

BOOL AbAccountOrderLeftWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	account_order_view_.set_order_window_id(order_window_id_);
	account_position_view_.set_order_window_id(order_window_id_);
	favorite_symbol_view_.set_order_window_id(order_window_id_);
	account_profit_loss_view_.SetUp();
	



	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_ACCEPTED);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	account_order_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID1);

	pWnd = GetDlgItem(IDC_STATIC_FILLED);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	account_position_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID2);
	//_PositionGrid.ShowWindow(SW_HIDE);


	pWnd = GetDlgItem(IDC_STATIC_FAVORITE);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	favorite_symbol_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID3);


	SetTimer(1, 100, NULL);

	/*
	_VScrollBarAcpt.ShowWindow(SW_SHOW);
	SCROLLINFO  scrinfo;
	scrinfo.cbSize = sizeof(scrinfo);
	scrinfo.fMask = SIF_ALL;
	scrinfo.nMin = 0;          // 최소값
	scrinfo.nMax = 200;      // 최대값
	scrinfo.nPage = _AcceptedArea.GetRowCount();      // 전체 스크롤바에서 스크롤박스가 차지하는 범위 
	scrinfo.nTrackPos = 0;  // 트랙바가 움직일때의 위치값
	scrinfo.nPos = 0;        // 위치
	_VScrollBarAcpt.SetScrollInfo(&scrinfo);


	_VScrollBarPosi.ShowWindow(SW_SHOW);
	
	scrinfo.cbSize = sizeof(scrinfo);
	scrinfo.fMask = SIF_ALL;
	scrinfo.nMin = 0;          // 최소값
	scrinfo.nMax = 200;      // 최대값
	scrinfo.nPage = _FilledArea.GetRowCount();      // 전체 스크롤바에서 스크롤박스가 차지하는 범위 
	scrinfo.nTrackPos = 0;  // 트랙바가 움직일때의 위치값
	scrinfo.nPos = 0;        // 위치
	_VScrollBarPosi.SetScrollInfo(&scrinfo);


	_VScrollBarFav.ShowWindow(SW_SHOW);

	scrinfo.cbSize = sizeof(scrinfo);
	scrinfo.fMask = SIF_ALL;
	scrinfo.nMin = 0;          // 최소값
	scrinfo.nMax = 200;      // 최대값
	scrinfo.nPage = _FavoriteArea.GetRowCount();      // 전체 스크롤바에서 스크롤박스가 차지하는 범위 
	scrinfo.nTrackPos = 0;  // 트랙바가 움직일때의 위치값
	scrinfo.nPos = 0;        // 위치
	_VScrollBarFav.SetScrollInfo(&scrinfo);
	*/


	//account_order_view_.StartTimer();
	//account_position_view_.start_timer();

	//_AcceptedGrid.StartTimer();

	favorite_symbol_view_.SetFavorite();

	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();

	//_FavoriteArea.SetFavorite();
	//_AcceptedArea.UpdateAcceptedOrder();
	//_FilledArea.UpdatePositionInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}



void AbAccountOrderLeftWindow::OnBnClickedBtnAddFav()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	_SymbolTableDlg->OrderLeftWnd = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

void AbAccountOrderLeftWindow::OnTimer(UINT_PTR nIDEvent)
{
	account_order_view_.on_timer();
	account_position_view_.on_timer();
	favorite_symbol_view_.Update();
}

void AbAccountOrderLeftWindow::SetAccount(std::shared_ptr<DarkHorse::SmAccount> account)
{
	account_order_view_.Account(account);
	account_position_view_.Account(account);
	account_profit_loss_view_.Account(account);
	account_profit_loss_view_.update_account_profit_loss();
}

void AbAccountOrderLeftWindow::SetFund(std::shared_ptr<DarkHorse::SmFund> fund)
{
	account_order_view_.Fund(fund);
	account_position_view_.Fund(fund);
}

LRESULT AbAccountOrderLeftWindow::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	//_FavoriteGrid.AddSymbol(static_cast<int>(wParam));
	return 1;
}

void AbAccountOrderLeftWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();
}


void AbAccountOrderLeftWindow::OnBnClickedBtnCancelSel()
{
	account_order_view_.CancelSelOrders();
}


void AbAccountOrderLeftWindow::OnBnClickedBtnCancelAll()
{
	account_order_view_.CancelAll();
}


void AbAccountOrderLeftWindow::OnBnClickedBtnLiqSel()
{
	account_position_view_.LiqSelPositions();
}


void AbAccountOrderLeftWindow::OnBnClickedBtnLiqAll()
{
	account_position_view_.LiqAll();
}

void AbAccountOrderLeftWindow::OnDestroy()
{
	CBCGPDialog::OnDestroy();
	KillTimer(1);
}
