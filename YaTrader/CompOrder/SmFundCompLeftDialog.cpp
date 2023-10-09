// SmFundCompLeftDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmFundCompLeftDialog.h"
#include "afxdialogex.h"

#include "../Symbol/SmSymbolTableDialog.h"

#define WND_ID1 0x00000001
#define WND_ID2 0x00000002
#define WND_ID3 0x00000003


// SmFundCompLeftDialog dialog

IMPLEMENT_DYNAMIC(SmFundCompLeftDialog, CBCGPDialog)

SmFundCompLeftDialog::SmFundCompLeftDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_LEFT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmFundCompLeftDialog::~SmFundCompLeftDialog()
{
	//KillTimer(1);
}

void SmFundCompLeftDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_ACCEPTED, _AcceptedArea);
	//DDX_Control(pDX, IDC_STATIC_FILLED, _FilledArea);
	//DDX_Control(pDX, IDC_STATIC_FAVORITE, _FavoriteArea);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, _AccountArea);
	//DDX_Control(pDX, IDC_SCROLLBAR_VER_ACPT, _VScrollBarAcpt);
	//DDX_Control(pDX, IDC_SCROLLBAR_VER_POSI, _VScrollBarPosi);
	//DDX_Control(pDX, IDC_SCROLLBAR_VER_FAV, _VScrollBarFav);
	DDX_Control(pDX, IDC_BTN_ADD_FAV, _BtnAddFav);
	DDX_Control(pDX, IDC_STATIC_FAV, _StaticFav);
}


BEGIN_MESSAGE_MAP(SmFundCompLeftDialog, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_FAV, &SmFundCompLeftDialog::OnBnClickedBtnAddFav)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &SmFundCompLeftDialog::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CANCEL_SEL, &SmFundCompLeftDialog::OnBnClickedBtnCancelSel)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, &SmFundCompLeftDialog::OnBnClickedBtnCancelAll)
	ON_BN_CLICKED(IDC_BTN_LIQ_SEL, &SmFundCompLeftDialog::OnBnClickedBtnLiqSel)
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &SmFundCompLeftDialog::OnBnClickedBtnLiqAll)
END_MESSAGE_MAP()


// SmFundCompLeftDialog message handlers


void SmFundCompLeftDialog::SetMainWnd(SmFundCompMainDialog* main_wnd)
{
	_AcceptedGrid.CompFundWnd(main_wnd);
	_PositionGrid.CompFundWnd(main_wnd);
	_FavoriteGrid.CompFundWnd(main_wnd);
}

BOOL SmFundCompLeftDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	//_AcceptedArea.SetUp();
	//_FilledArea.SetUp();
	//_FavoriteArea.SetUp();
	_AccountArea.SetUp();




	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_ACCEPTED);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	_AcceptedGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID1);

	pWnd = GetDlgItem(IDC_STATIC_FILLED);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	_PositionGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID2);
	//_PositionGrid.ShowWindow(SW_HIDE);


	pWnd = GetDlgItem(IDC_STATIC_FAVORITE);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	_FavoriteGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID3);


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


	_AcceptedGrid.UpdateAcceptedOrder();
	_PositionGrid.UpdatePositionInfo();

	//_AcceptedGrid.StartTimer();

	_FavoriteGrid.SetFavorite();

	_AcceptedGrid.Mode(1);
	_PositionGrid.Mode(1);

	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();

	//_FavoriteArea.SetFavorite();
	//_AcceptedArea.UpdateAcceptedOrder();
	//_FilledArea.UpdatePositionInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}



void SmFundCompLeftDialog::OnBnClickedBtnAddFav()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	//_SymbolTableDlg->CompOrderLeft = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

void SmFundCompLeftDialog::OnTimer(UINT_PTR nIDEvent)
{
	_AcceptedGrid.Update();
	_PositionGrid.Update();
	_FavoriteGrid.Update();
	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();
	//_AccountArea.UpdateAssetInfo();
	//_FavoriteGrid.SetFavorite();
}

void SmFundCompLeftDialog::SetAccount(std::shared_ptr<DarkHorse::SmAccount> account)
{
	_AcceptedGrid.Account(account);
	_PositionGrid.Account(account);
	_AccountArea.Account(account);
	_AccountArea.UpdateAssetInfo();
	//_FilledArea.Account(account);
	//_AcceptedArea.Account(account);
}

void SmFundCompLeftDialog::SetFund(std::shared_ptr < DarkHorse::SmFund> fund)
{
	_AcceptedGrid.Fund(fund);
	_PositionGrid.Fund(fund);
	_AccountArea.Fund(fund);
	_AccountArea.UpdateAssetInfo();
}

LRESULT SmFundCompLeftDialog::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	//_FavoriteGrid.AddSymbol(static_cast<int>(wParam));
	return 1;
}

void SmFundCompLeftDialog::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();
}


void SmFundCompLeftDialog::OnBnClickedBtnCancelSel()
{
	_AcceptedGrid.CancelSelOrders();
}


void SmFundCompLeftDialog::OnBnClickedBtnCancelAll()
{
	_AcceptedGrid.CancelAll();
}


void SmFundCompLeftDialog::OnBnClickedBtnLiqSel()
{
	_PositionGrid.LiqSelPositions();
}


void SmFundCompLeftDialog::OnBnClickedBtnLiqAll()
{
	_PositionGrid.LiqAll();
}
