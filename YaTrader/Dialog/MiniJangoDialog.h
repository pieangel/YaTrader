#pragma once
#include <BCGCBProInc.h>
#include "../View/AccountProfitLossView.h"
#include "../View/AccountPositionView.h"
#include <map>
#include <string.h>
// MiniJangoDialog dialog

class MiniJangoDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(MiniJangoDialog)

public:
	MiniJangoDialog(CWnd* pParent = nullptr);   // standard constructor
	MiniJangoDialog(CWnd* pParent, const std::string& type);   // standard constructor
	MiniJangoDialog(CWnd* pParent, const std::string& type, const int mode, const std::string& target);
	virtual ~MiniJangoDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JANGO };
#endif

	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	std::string account_no() const { return account_no_; }
	void account_no(std::string val) { account_no_ = val; }
	std::string fund_name() const { return fund_name_; }
	void fund_name(std::string val) { fund_name_ = val; }
	std::string Type() const { return type_; }
	void Type(std::string val) { type_ = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CBCGPComboBox _ComboAccount;
	CBCGPStatic _StaticCombo;
	AccountProfitLossView account_profit_loss_view_;
	AccountPositionView account_position_view_;
	virtual BOOL OnInitDialog();

private:
	void SetAccount();
	void SetFund();
	std::string account_no_;
	std::string fund_name_;
	// key : combo index, value : account object.
	std::map<int, std::string> _ComboAccountMap;
	std::map<int, std::string> _ComboFundMap;
	int _CurrentAccountIndex{ 0 };
	// 0 : account, 1 : fund
	int _Mode = 0;
	// "1" : 해외, "9" : 국내 
	std::string type_;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeComboAccount();
	virtual void PostNcDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
