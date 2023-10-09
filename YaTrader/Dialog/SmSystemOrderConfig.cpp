// VtSystemOrderConfig.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmSystemOrderConfig.h"
#include "../resource.h" 


// VtSystemOrderConfig dialog

IMPLEMENT_DYNAMIC(SmSystemOrderConfig, CBCGPDialog)

SmSystemOrderConfig::SmSystemOrderConfig(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_SYS_ORDER_SET, pParent)
{

}

SmSystemOrderConfig::~SmSystemOrderConfig()
{
}

void SmSystemOrderConfig::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PRICE, _ComboPrice);
	DDX_Control(pDX, IDC_COMBO_PRICE_TYPE, _ComboPriceType);
	DDX_Control(pDX, IDC_EDIT_TICK, _EditTick);
	DDX_Control(pDX, IDC_SPIN_TICK, _SpinTick);
}


BEGIN_MESSAGE_MAP(SmSystemOrderConfig, CBCGPDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_PRICE_TYPE, &SmSystemOrderConfig::OnCbnSelchangeComboPriceType)
	ON_CBN_SELCHANGE(IDC_COMBO_PRICE, &SmSystemOrderConfig::OnCbnSelchangeComboPrice)
	ON_BN_CLICKED(IDC_BTN_OK, &SmSystemOrderConfig::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &SmSystemOrderConfig::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// VtSystemOrderConfig message handlers


BOOL SmSystemOrderConfig::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	_ComboPriceType.AddString(_T("지정가"));
	_ComboPriceType.AddString(_T("시장가"));
	_PriceType == DarkHorse::SmPriceType::Price ? _ComboPriceType.SetCurSel(0) : _ComboPriceType.SetCurSel(1);

	_ComboPrice.AddString(_T("현재가 + - 5"));
	_ComboPrice.AddString(_T("현재가 + - 10"));
	_ComboPrice.AddString(_T("사용자지정"));
	_EditTick.EnableWindow(FALSE);
	_SpinTick.EnableWindow(FALSE);
	if (_OrderTick == 5) {
		_ComboPrice.SetCurSel(0);
	}
	else if (_OrderTick == 10) {
		_ComboPrice.SetCurSel(1);
	}
	else {
		_ComboPrice.SetCurSel(2);
		_EditTick.EnableWindow(TRUE);
		_SpinTick.EnableWindow(TRUE);
	}

	_SpinTick.SetRange(0, 100);
	CString strTick;
	strTick.Format(_T("%d"), _OrderTick);
	_EditTick.SetWindowText(strTick);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmSystemOrderConfig::OnCbnSelchangeComboPriceType()
{
	int selIndex = _ComboPriceType.GetCurSel();
	if (selIndex < 0) return;
	selIndex == 0 ? _PriceType = DarkHorse::SmPriceType::Price : _PriceType = DarkHorse::SmPriceType::Market;
	if (selIndex != 0) {
		_ComboPrice.EnableWindow(FALSE);
		_EditTick.EnableWindow(FALSE);
		_SpinTick.EnableWindow(FALSE);
	}
	else {
		_ComboPrice.EnableWindow(TRUE);
		_EditTick.EnableWindow(TRUE);
		_SpinTick.EnableWindow(TRUE);
	}
}


void SmSystemOrderConfig::OnCbnSelchangeComboPrice()
{
	int selIndex = _ComboPrice.GetCurSel();
	if (selIndex < 0) return;
	_EditTick.EnableWindow(FALSE);
	_SpinTick.EnableWindow(FALSE);
	if (selIndex == 0) {
		_OrderTick = 5;
		CString strTick;
		strTick.Format(_T("%d"), _OrderTick);
		_EditTick.SetWindowText(strTick);
	}
	else if (selIndex == 1) {
		_OrderTick = 10;
		CString strTick;
		strTick.Format(_T("%d"), _OrderTick);
		_EditTick.SetWindowText(strTick);
	}
	else {
		_EditTick.EnableWindow(TRUE);
		_SpinTick.EnableWindow(TRUE);
		CString strTick;
		_EditTick.GetWindowText(strTick);
		_OrderTick = _ttoi(strTick);
	}
}


void SmSystemOrderConfig::OnBnClickedBtnOk()
{
	CString strTick;
	_EditTick.GetWindowText(strTick);
	_OrderTick = _ttoi(strTick);

	CBCGPDialog::OnOK();
}


void SmSystemOrderConfig::OnBnClickedBtnCancel()
{
	CBCGPDialog::OnCancel();
}
