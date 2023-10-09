// OrderRightDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "AbAccountOrderRightWindow.h"
#include "afxdialogex.h"


// OrderRightDialog dialog

IMPLEMENT_DYNAMIC(AbAccountOrderRightWindow, CBCGPDialog)

AbAccountOrderRightWindow::AbAccountOrderRightWindow(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_RIGHT, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

AbAccountOrderRightWindow::~AbAccountOrderRightWindow()
{
	//KillTimer(1);
}

void AbAccountOrderRightWindow::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, asset_view_);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_MASTER, symbol_info_view_);
}


BEGIN_MESSAGE_MAP(AbAccountOrderRightWindow, CBCGPDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// OrderRightDialog message handlers


void AbAccountOrderRightWindow::SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account)
{
	asset_view_.Account(account);
	asset_view_.SetAssetInfo();
}

void AbAccountOrderRightWindow::SetFund(std::shared_ptr<DarkHorse::SmFund> fund)
{
	asset_view_.Fund(fund);
	asset_view_.SetAssetInfo();
}

void AbAccountOrderRightWindow::SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol)
{
	symbol_info_view_.Symbol(symbol);
}

void AbAccountOrderRightWindow::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void AbAccountOrderRightWindow::OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	symbol_info_view_.Symbol(symbol);
}

BOOL AbAccountOrderRightWindow::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	asset_view_.order_window_id(order_window_id_);
	asset_view_.SetUp();
	symbol_info_view_.SetUp();
	

	//SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void AbAccountOrderRightWindow::OnTimer(UINT_PTR nIDEvent)
{
	CBCGPDialog::OnTimer(nIDEvent);
}
