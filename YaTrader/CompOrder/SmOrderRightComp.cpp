// SmOrderRightComp.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmOrderRightComp.h"
#include "afxdialogex.h"
#include "SmOrderCompMainDialog.h"


// OrderRightDialog dialog

IMPLEMENT_DYNAMIC(SmOrderRightComp, CBCGPDialog)

SmOrderRightComp::SmOrderRightComp(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_ORDER_RIGHT_COMP, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmOrderRightComp::~SmOrderRightComp()
{
	//KillTimer(1);
}

void SmOrderRightComp::DoDataExchange(CDataExchange* pDX)
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


BEGIN_MESSAGE_MAP(SmOrderRightComp, CBCGPDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_ORDER_ALL, &SmOrderRightComp::OnBnClickedBtnOrderAll)
	ON_BN_CLICKED(IDC_RADIO_MARKET, &SmOrderRightComp::OnBnClickedRadioMarket)
	ON_BN_CLICKED(IDC_RADIO_PRICE, &SmOrderRightComp::OnBnClickedRadioPrice)
	ON_EN_CHANGE(IDC_EDIT_SLIP, &SmOrderRightComp::OnEnChangeEditSlip)
END_MESSAGE_MAP()


// OrderRightDialog message handlers


void SmOrderRightComp::SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account)
{
	_AssetArea.Account(account);
	_AssetArea.SetAssetInfo();
}

void SmOrderRightComp::SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol)
{
	_SymbolArea.Symbol(symbol);
}

void SmOrderRightComp::OnOrderChanged(const int& account_id, const int& symbol_id)
{

}

void SmOrderRightComp::DoOrder()
{
	if (!_MainWnd) return;

	_MainWnd->OnDoOrder(_PriceType, _Slip);
}

BOOL SmOrderRightComp::OnInitDialog()
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
	_OrderButton.RightWnd(this);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmOrderRightComp::OnTimer(UINT_PTR nIDEvent)
{
	_AssetArea.SetAssetInfo();
	_SymbolArea.UpdateSymbolInfo();

	CBCGPDialog::OnTimer(nIDEvent);
}



void SmOrderRightComp::OnBnClickedBtnOrderAll()
{
	if (!_MainWnd) return;

	_MainWnd->OnDoOrder(_PriceType, _Slip);
}


void SmOrderRightComp::OnBnClickedRadioMarket()
{
	// TODO: Add your control notification handler code here
	_PriceType = DarkHorse::SmPriceType::Market;
}


void SmOrderRightComp::OnBnClickedRadioPrice()
{
	// TODO: Add your control notification handler code here
	_PriceType = DarkHorse::SmPriceType::Price;
}


void SmOrderRightComp::OnEnChangeEditSlip()
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
