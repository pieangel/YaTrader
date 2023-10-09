#pragma once
#include <afxdialogex.h>
#include <BCGPDialog.h>
#include <BCGPEdit.h>

// SmLoginDlg dialog

class SmLoginDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmLoginDlg)

public:
	SmLoginDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmLoginDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnCan();
	CBCGPEdit _EditCert;
	CBCGPEdit _EditId;
	CBCGPEdit _EditPwd;
	CBCGPButton _CheckSave;
	CBCGPButton _CheckSimul;
	virtual BOOL OnInitDialog();
	CComboBox log_in_server_;
	afx_msg void OnCbnSelchangeComboLoginServer();
};
