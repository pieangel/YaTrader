#pragma once
#include "afxdialogex.h"
#include <BCGCBProInc.h>
#include <memory>
#include "MainAccountGrid.h"
#include "SubAccountGrid.h"
// SubAccountEditor dialog
namespace DarkHorse
{
	class SmAccount;
}

class SubAccountEditor : public CBCGPScrollDialog
{
	DECLARE_DYNAMIC(SubAccountEditor)

public:
	SubAccountEditor(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SubAccountEditor();
	virtual BOOL OnInitDialog();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUB_ACCOUNT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	std::shared_ptr<DarkHorse::SmAccount> selected_account_ = nullptr;
	MainAccountGrid main_account_grid_;
	SubAccountGrid sub_account_grid_;	


	CBCGPEdit _EditSubAcntName;
	CBCGPEdit _EditSubAcntCode;
	CBCGPComboBox _ComboAccountType;
	CBCGPStatic  _StaticSelAcntName;
	std::string account_type_{"9"}; // 9: default account type
	void set_sub_account_grid();
	void set_sel_account_name();
	
public:
	CBCGPButton _BtnModify;
	std::string account_type() const { return account_type_; }
	void account_type(std::string val) { account_type_ = val; }
	void set_account(std::shared_ptr<DarkHorse::SmAccount> account);
	void set_sub_account(std::shared_ptr<DarkHorse::SmAccount> sub_account);
	void set_default_account();
	afx_msg void OnCbnSelchangeComboAccountType();
	afx_msg void OnBnClickedBtnCreate();
	afx_msg void OnBnClickedBtnModify();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnDelete();
};
