#pragma once
#include <BCGCBProInc.h>
#include <map>
#include <string>
#include <memory>
#include <vector>

// SmFundEditDialog dialog
#include "SmAccountGrid.h"
#include "SmFundAccountGrid.h"

namespace DarkHorse {
	class SmFund;
	class SmAccount;
}

class SmFundEditDialog : public CBCGPScrollDialog
{
	DECLARE_DYNAMIC(SmFundEditDialog)

public:
	SmFundEditDialog(CWnd* pParent = nullptr);   // standard constructor
	SmFundEditDialog(CWnd* pParent, const int& fund_id);
	virtual ~SmFundEditDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FUND_EDIT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//CBCGPEdit _EditAcntSeungSu;
	CBCGPEdit _EditFundName;
	//CBCGPListCtrl _ListRegAcnt;
	//CBCGPListCtrl _ListUnregAcnt;

	SmAccountGrid _AccountGrid;
	SmFundAccountGrid _FundAccountGrid;
	//CBCGPGroup _StaticAcntDetail;
	//CBCGPStatic _StaticAcntRatio;
	//CBCGPStatic _StaticAcntName;
	afx_msg void OnLvnItemchangedListRegAccount(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedListUnregAccount(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();

	
	//void RemoveFromUnregAccountGroup(std::shared_ptr<DarkHorse::SmAccount> account);
	//void AddToUnregAccountGroup(std::shared_ptr<DarkHorse::SmAccount> account);
	void UnregAccountDoubleClickEvent(const int& row);
	void UnregAccountClickEvent(const int& row);
	void UnregAccountDoubleClickEvent(std::shared_ptr<DarkHorse::SmAccount> account);
	void UnregAccountClickEvent(std::shared_ptr<DarkHorse::SmAccount> account);
	void FundAccountDoubleClickEvent(const int& row);
	void FundAccountClickEvent(const int& row);
	void FundAccountDoubleClickEvent(std::shared_ptr<DarkHorse::SmAccount> account);
	void FundAccountClickEvent(std::shared_ptr<DarkHorse::SmAccount> account);
private:
	CBCGPComboBox _ComboAccountType;
	std::string account_type_{"9"}; // 9: default account type
	void RefreshUnregAccounts();
	void RemoveFromFund(std::shared_ptr<DarkHorse::SmAccount> account);
	void AddToFund(std::shared_ptr<DarkHorse::SmAccount> account);
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _RowToAccountMap;
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _RowToFundAccountMap;
	std::vector<std::shared_ptr<DarkHorse::SmAccount>> _FundAccountsBackup;
	//void InitUnregAccount();
	//void InitFund(std::shared_ptr<DarkHorse::SmFund> fund);
	int _FundId = -1;
	std::shared_ptr<DarkHorse::SmFund> _CurFund = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _CurAccount = nullptr;
	int _MaxAccountRow = 0;
	int _MaxFundAccountRow = 0;
	//void ClearAccountGrid(const int& row);
	//void ClearFundAccountGrid(const int& row);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnIn();
	afx_msg void OnBnClickedBtnInAll();
	afx_msg void OnBnClickedBtnOut();
	afx_msg void OnBnClickedBtnOutAll();
	afx_msg void OnEnChangeEditFundName();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEditAcntSeunsu();
	afx_msg void OnCbnSelchangeComboAccountType();
};
