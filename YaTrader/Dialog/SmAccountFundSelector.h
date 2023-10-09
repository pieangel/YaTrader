#pragma once
#include "AccountFundView.h"

// VtAccountFundSelector dialog
class VtUsdStrategyConfigDlg;
class VtSignalConnectionGrid;
class SmHftConfig;
class SmAccountFundSelector : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmAccountFundSelector)

public:
	SmAccountFundSelector(CWnd* pParent, int mode);   // standard constructor
	virtual ~SmAccountFundSelector();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACNT_FUND_SELECTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	AccountFundView account_fund_view_;
	int mode_{ 0 };
public:
	void set_mode(int mode) {
		account_fund_view_.mode(mode);
	}
	void set_source_id(const int source_id) {
		account_fund_view_.set_source_id(source_id);
	}
	CBCGPButton radio_acnt_;
	CBCGPButton radio_fund_;
	CString m_SelectedCode;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioAcnt();
	afx_msg void OnBnClickedRadioFund();
	afx_msg void OnBnClickedBtnClose();
};
