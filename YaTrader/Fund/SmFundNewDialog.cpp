// SmFundNewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "../DarkHorse.h"
#include "SmFundNewDialog.h"
#include "afxdialogex.h"
#include "../Global/SmTotalManager.h"
#include "SmFund.h"
#include "SmFundManager.h"
#include <format>

// SmFundNewDialog dialog

IMPLEMENT_DYNAMIC(SmFundNewDialog, CBCGPScrollDialog)

SmFundNewDialog::SmFundNewDialog(CWnd* pParent /*=nullptr*/)
	: CBCGPScrollDialog(IDD_FUND_NEW, pParent)
{
	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();
}

SmFundNewDialog::~SmFundNewDialog()
{
}

void SmFundNewDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPScrollDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FUND_NAME, _EditFundName);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT_TYPE, _ComboAccountType);
}


BEGIN_MESSAGE_MAP(SmFundNewDialog, CBCGPScrollDialog)
	
	ON_BN_CLICKED(IDOK, &SmFundNewDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &SmFundNewDialog::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT_TYPE, &SmFundNewDialog::OnCbnSelchangeComboAccountType)
END_MESSAGE_MAP()


// SmFundNewDialog message handlers


BOOL SmFundNewDialog::OnInitDialog()
{
	CBCGPScrollDialog::OnInitDialog();

	if (_EditFundName.GetSafeHwnd()) {
		CString strFundName;
		size_t fund_count = mainApp.FundMgr()->GetFundCount();
		strFundName.Format("Fund%d", fund_count + 1);
		_EditFundName.SetWindowTextA(strFundName);
	}
	_ComboAccountType.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SmFundNewDialog::OnBnClickedOk()
{
	CString fund_name;
	_EditFundName.GetWindowText(fund_name);

	auto fund = mainApp.FundMgr()->FindFund(static_cast<const char*>(fund_name));
	if (fund) {
		AfxMessageBox("같은 이름이 펀드가 있습니다. 다른 이름을 사용해 주세요!");
		return;
	}

	FundName = fund_name;

	CBCGPScrollDialog::OnOK();
}

void SmFundNewDialog::OnBnClickedCancel()
{
	CBCGPScrollDialog::OnCancel();
}


void SmFundNewDialog::OnCbnSelchangeComboAccountType()
{
	if (_ComboAccountType.GetCurSel() < 0) return;
	int index = _ComboAccountType.GetCurSel();
	if (index == 0)
		account_type_ = "9";
	else
		account_type_ = "1";
}
