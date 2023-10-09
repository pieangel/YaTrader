// SmFundCompRightDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmFundCompRightDialog.h"
#include "afxdialogex.h"
#include "SmFundCompMainDialog.h"

// OrderRightDialog dialog

IMPLEMENT_DYNAMIC(SmFundCompRightDialog, CBCGPDialog)

SmFundCompRightDialog::SmFundCompRightDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_RIGHT_COMP, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmFundCompRightDialog::~SmFundCompRightDialog()
{
	//KillTimer(1);
}

void SmFundCompRightDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, _AssetArea);
	DDX_Control(pDX, IDC_STATIC_SYMBOL_MASTER, _SymbolArea);
	DDX_Control(pDX, IDC_EDIT_SLIP, _EditSlip);
	DDX_Control(pDX, IDC_RADIO_PRICE, _RadioPrice);
	DDX_Control(pDX, IDC_RADIO_MARKET, _RadioMarket);
	DDX_Control(pDX, IDC_SPIN_SLIP, _SpinSlip);
	// IDC_STATIC_ORDER
	DDX_Control(pDX, IDC_STATIC_ORDER, _OrderButton);
}


BEGIN_MESSAGE_MAP(SmFundCompRightDialog, CBCGPDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_ORDER_ALL, &SmFundCompRightDialog::OnBnClickedBtnOrderAll)
	ON_BN_CLICKED(IDC_RADIO_MARKET, &SmFundCompRightDialog::OnBnClickedRadioMarket)
	ON_BN_CLICKED(IDC_RADIO_PRICE, &SmFundCompRightDialog::OnBnClickedRadioPrice)
	ON_EN_CHANGE(IDC_EDIT_SLIP, &SmFundCompRightDialog::OnEnChangeEditSlip)
END_MESSAGE_MAP()


// OrderRightDialog message handlers


void SmFundCompRightDialog::SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account)
{
	_AssetArea.Account(account);
	_AssetArea.SetAssetInfo();
}

void SmFundCompRightDialog::SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol)
{
	_SymbolArea.Symbol(symbol);
}

void SmFundCompRightDialog::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void SmFundCompRightDialog::SetFund(const std::shared_ptr<DarkHorse::SmFund>& fund)
{
	_AssetArea.Fund(fund);
	_AssetArea.SetAssetInfo();
}

void SmFundCompRightDialog::DoOrder()
{
	if (!_MainWnd) return;

	_MainWnd->OnDoOrder(_PriceType, _Slip);
}

BOOL SmFundCompRightDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	_AssetArea.SetUp();
	_SymbolArea.SetUp();

	_RadioPrice.SetCheck(BST_CHECKED);
	_EditSlip.SetWindowText("2");
	//SetTimer(1, 100, NULL);

	_SpinSlip.SetRange32(0, 100);

	_OrderButton.SetUp();
	_OrderButton.Text("ÀÏ°ý ÁÖ¹®");
	_OrderButton.RightCompFund(this);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmFundCompRightDialog::OnTimer(UINT_PTR nIDEvent)
{
	_AssetArea.SetAssetInfo();
	_SymbolArea.UpdateSymbolInfo();

	CBCGPDialog::OnTimer(nIDEvent);
}



void SmFundCompRightDialog::OnBnClickedBtnOrderAll()
{
	if (!_MainWnd) return;

	_MainWnd->OnDoOrder(_PriceType, _Slip);
}


void SmFundCompRightDialog::OnBnClickedRadioMarket()
{
	// TODO: Add your control notification handler code here
	_PriceType = DarkHorse::SmPriceType::Market;
}


void SmFundCompRightDialog::OnBnClickedRadioPrice()
{
	// TODO: Add your control notification handler code here
	_PriceType = DarkHorse::SmPriceType::Price;
}


void SmFundCompRightDialog::OnEnChangeEditSlip()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBCGPDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	if (!_EditSlip.GetSafeHwnd()) return;

	CString strSlip;
	_EditSlip.GetWindowText(strSlip);
	_Slip = _ttoi(strSlip);
}
