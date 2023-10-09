#pragma once
#include <map>
#include <memory>
#include <list>
#include <string>
// SmAccountPwdDlg dialog
namespace DarkHorse {
	class SmAccount;
}
class SmAccountPwdDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmAccountPwdDlg)

public:
	SmAccountPwdDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmAccountPwdDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACCOUNT_PWD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _RowToAccountMap;
	CBCGPGridCtrl m_wndGrid;
	void SavePassword();
	std::list<std::pair<std::string, std::string>> _ReqQ;
	void handle_account_password_error();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnClose();
	afx_msg LRESULT OnUmPasswordConfirmed(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};
