#pragma once
#include <BCGCBProInc.h>
#include <string>
// SmFundNewDialog dialog

class SmFundNewDialog : public CBCGPScrollDialog
{
	DECLARE_DYNAMIC(SmFundNewDialog)

public:
	SmFundNewDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmFundNewDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FUND_NEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString FundName;
	std::string account_type_;
	CBCGPEdit _EditFundName;
	CBCGPComboBox _ComboAccountType;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeComboAccountType();
};
