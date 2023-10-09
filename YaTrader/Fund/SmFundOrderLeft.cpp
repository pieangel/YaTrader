// SmFundOrderLeft.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmFundOrderLeft.h"
#include "afxdialogex.h"
#include "../Symbol/SmSymbolTableDialog.h"

#define WND_ID1 0x00000001
#define WND_ID2 0x00000002
#define WND_ID3 0x00000003


// SmFundOrderLeft dialog

IMPLEMENT_DYNAMIC(SmFundOrderLeft, CBCGPDialog)

SmFundOrderLeft::SmFundOrderLeft(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_LEFT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmFundOrderLeft::~SmFundOrderLeft()
{
	KillTimer(1);
}

void SmFundOrderLeft::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_ACCEPTED, _AcceptedArea);
	//DDX_Control(pDX, IDC_STATIC_FILLED, _FilledArea);
	//DDX_Control(pDX, IDC_STATIC_FAVORITE, _FavoriteArea);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, _AccountArea);
	DDX_Control(pDX, IDC_BTN_ADD_FAV, _BtnAddFav);
	DDX_Control(pDX, IDC_STATIC_FAV, _StaticFav);
}


BEGIN_MESSAGE_MAP(SmFundOrderLeft, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_FAV, &SmFundOrderLeft::OnBnClickedBtnAddFav)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &SmFundOrderLeft::OnUmSymbolSelected)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CANCEL_SEL, &SmFundOrderLeft::OnBnClickedBtnCancelSel)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, &SmFundOrderLeft::OnBnClickedBtnCancelAll)
	ON_BN_CLICKED(IDC_BTN_LIQ_SEL, &SmFundOrderLeft::OnBnClickedBtnLiqSel)
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &SmFundOrderLeft::OnBnClickedBtnLiqAll)
END_MESSAGE_MAP()


// SmFundOrderLeft message handlers


void SmFundOrderLeft::SetFundOrderWnd(SmFundOrderDialog* fund_order_wnd)
{
	_AcceptedGrid.SetFundOrderWnd(fund_order_wnd);
	_PositionGrid.SetFundOrderWnd(fund_order_wnd);
	_FavoriteGrid.SetFundOrderWnd(fund_order_wnd);
}

BOOL SmFundOrderLeft::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	//_AcceptedArea.SetUp();
	//_FilledArea.SetUp();
	//_FavoriteArea.SetUp();
	_AccountArea.SetUp();
	_AccountArea.Mode(1);




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

	_AcceptedGrid.Mode(1);
	_PositionGrid.Mode(1);


	pWnd = GetDlgItem(IDC_STATIC_FAVORITE);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Create the Windows control and attach it to the Grid object
	_FavoriteGrid.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, WND_ID3);


	SetTimer(1, 100, NULL);

	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();

	//_AcceptedGrid.StartTimer();

	_FavoriteGrid.SetFavorite();

	//_AcceptedGrid.StartTimer();
	//_PositionGrid.StartTimer();
	_AcceptedGrid.UpdateAcceptedOrder();
	_PositionGrid.UpdatePositionInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}



void SmFundOrderLeft::OnBnClickedBtnAddFav()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	_SymbolTableDlg->FundOrderLeft = this;
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

void SmFundOrderLeft::OnTimer(UINT_PTR nIDEvent)
{
	_AcceptedGrid.Update();
	_PositionGrid.Update();
	_FavoriteGrid.Update();
	//_AcceptedGrid.UpdateAcceptedOrder();
	//_PositionGrid.UpdatePositionInfo();
	//_AccountArea.UpdateAssetInfo();
	//_FavoriteGrid.SetFavorite();
}

void SmFundOrderLeft::SetAccount(std::shared_ptr<DarkHorse::SmAccount> account)
{
	_AcceptedGrid.Account(account);
	_PositionGrid.Account(account);
	_AccountArea.Account(account);
	_AccountArea.UpdateAssetInfo();
}

void SmFundOrderLeft::SetFund(std::shared_ptr < DarkHorse::SmFund> fund)
{
	_AcceptedGrid.Fund(fund);
	_PositionGrid.Fund(fund);
	_AccountArea.Fund(fund);
	_AccountArea.UpdateAssetInfo();
}

LRESULT SmFundOrderLeft::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	_FavoriteGrid.AddSymbol(static_cast<int>(wParam));
	return 1;
}

void SmFundOrderLeft::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	_AcceptedGrid.UpdateAcceptedOrder();
	_PositionGrid.UpdatePositionInfo();
}


void SmFundOrderLeft::OnBnClickedBtnCancelSel()
{
	_AcceptedGrid.CancelSelOrders();
}


void SmFundOrderLeft::OnBnClickedBtnCancelAll()
{
	_AcceptedGrid.CancelAll();
}


void SmFundOrderLeft::OnBnClickedBtnLiqSel()
{
	_PositionGrid.LiqSelPositions();
}


void SmFundOrderLeft::OnBnClickedBtnLiqAll()
{
	_PositionGrid.LiqAll();
}
