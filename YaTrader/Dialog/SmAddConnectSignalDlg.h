#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <BCGCBProInc.h>
#include <map>
#include <memory>
//#include "VtOutSignalDefManager.h"

// VtAddConnectSignalDlg dialog
namespace DarkHorse {
	class SmAccount;
	class SmFund;
	class SmSymbol;
	class SmOutSignalDef;
}
class SmAutoSignalManagerDialog;
class HdSymbolSelecter;
class VtAutoSignalManagerDialog;
class AbFavoriteSymbolSelector;
class SmAddConnectSignalDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmAddConnectSignalDlg)

public:
	SmAddConnectSignalDlg(SmAutoSignalManagerDialog* source_dialog);
	SmAddConnectSignalDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SmAddConnectSignalDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_SIG_CONNECT };
#endif
	SmAutoSignalManagerDialog* source_dialog() const { return source_dialog_; }
	void source_dialog(SmAutoSignalManagerDialog* val) { source_dialog_ = val; }
	VtAutoSignalManagerDialog* auto_connect_dialog() const { return auto_connect_dialog_; }
	void auto_connect_dialog(VtAutoSignalManagerDialog* val) { auto_connect_dialog_ = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox _ComboAcnt;
	CComboBox _ComboSignal;
	CComboBox _ComboSymbol;
	CComboBox _ComboType;
	CEdit _EditSeungsu;
	CSpinButtonCtrl _SpinSeungsu;
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnCbnSelchangeComboAcnt();
	afx_msg void OnCbnSelchangeComboSymbol();
	afx_msg void OnBnClickedBtnFindSymbol();
	afx_msg void OnCbnSelchangeComboSignal();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnCancel();
	virtual BOOL OnInitDialog();
	int _Mode = 0;
	void InitCombo();
	void InitOutSigDefCombo();

private:
	VtAutoSignalManagerDialog* auto_connect_dialog_ = nullptr;
	std::shared_ptr<HdSymbolSelecter> dm_symbol_selector_;
	std::shared_ptr<AbFavoriteSymbolSelector> ab_symbol_selector_;
	void set_symbol_from_out(const int window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol);
	int id_ = 0;
	std::shared_ptr<DarkHorse::SmAccount> account_;
	std::shared_ptr<DarkHorse::SmFund> fund_;
	std::shared_ptr<DarkHorse::SmSymbol> symbol_;
	std::shared_ptr<DarkHorse::SmOutSignalDef> out_sig_def_;
	SmAutoSignalManagerDialog* source_dialog_ = nullptr;
	// key: combo index, value: account
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> combo_to_account_map_;
	// key: combo index, value: fund
	std::map<int, std::shared_ptr<DarkHorse::SmFund>> combo_to_fund_map_;
	// key: combo index, value: symbol
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> combo_to_symbol_map_;
	// key: combo index, value: out signal def
	std::map<int, std::shared_ptr<DarkHorse::SmOutSignalDef>> combo_to_out_sig_def_map_;
public:
	afx_msg void OnBnClickedBtnAdd();
};
