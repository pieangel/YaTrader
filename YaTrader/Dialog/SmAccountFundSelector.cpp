// VtAccountFundSelector.cpp : implementation file
//

#include "stdafx.h"
#include "SmAccountFundSelector.h"
#include "afxdialogex.h"
#include "../resource.h"

// VtAccountFundSelector dialog

IMPLEMENT_DYNAMIC(SmAccountFundSelector, CBCGPDialog)

SmAccountFundSelector::SmAccountFundSelector(CWnd* pParent, int mode)
	: CBCGPDialog(IDD_ACNT_FUND_SELECTOR, pParent), mode_(mode)
{
}


SmAccountFundSelector::~SmAccountFundSelector()
{
}

void SmAccountFundSelector::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SmAccountFundSelector, CBCGPDialog)
	ON_BN_CLICKED(IDC_RADIO_ACNT, &SmAccountFundSelector::OnBnClickedRadioAcnt)
	ON_BN_CLICKED(IDC_RADIO_FUND, &SmAccountFundSelector::OnBnClickedRadioFund)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &SmAccountFundSelector::OnBnClickedBtnClose)
END_MESSAGE_MAP()


// VtAccountFundSelector message handlers


BOOL SmAccountFundSelector::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_ACNT_FUND_GRID);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	// Create the Windows control and attach it to the Grid object
	account_fund_view_.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, 0x0023);

	if (mode_ == 0) {
		((CButton*)GetDlgItem(IDC_RADIO_ACNT))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_FUND))->SetCheck(BST_UNCHECKED);
	}
	else if (mode_ == 1) {
		((CButton*)GetDlgItem(IDC_RADIO_ACNT))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_FUND))->SetCheck(BST_CHECKED);
	}
	else {
		((CButton*)GetDlgItem(IDC_RADIO_ACNT))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_FUND))->SetCheck(BST_UNCHECKED);
	}

	account_fund_view_.mode(mode_);
	account_fund_view_.init_grid();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmAccountFundSelector::OnBnClickedRadioAcnt()
{
	mode_ = 0;
	account_fund_view_.mode(mode_);
	account_fund_view_.init_grid();
}


void SmAccountFundSelector::OnBnClickedRadioFund()
{
	mode_ = 1;
	account_fund_view_.mode(mode_);
	account_fund_view_.init_grid();
}


void SmAccountFundSelector::OnBnClickedBtnClose()
{
	SendMessage(WM_CLOSE, 0, 0);
}
