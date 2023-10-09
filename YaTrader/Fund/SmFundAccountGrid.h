

#pragma once
#include <BCGCBProInc.h>
#include <memory>
#include <vector>
#include <string>
#include <set>
#include <map>

namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmOrder;
	class SmFund;
}
class SmFundDialog;
class SmFundEditDialog;
class SmAccountGrid;
class SmFundGrid;
class SmFundAccountGrid : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(SmFundAccountGrid)

	// Construction
public:
	SmFundAccountGrid();

	
	void Fund(std::shared_ptr<DarkHorse::SmFund> val) { _Fund = val; }
	// Attributes
protected:
	BOOL m_bExtendedPadding;

	// Operations
public:
	void ToggleExtendedPadding();
	BOOL IsExtendedPadding() const
	{
		return m_bExtendedPadding;
	}

	// Overrides
	virtual int GetTextMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetTextMargin();
	}

	virtual int GetTextVMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetTextVMargin();
	}

	virtual int GetImageMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetImageMargin();
	}

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicGridCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	std::string Account_type() const { return account_type_; }
	void Account_type(std::string val) { account_type_ = val; }
	SmFundGrid* FundGrid = nullptr;
	SmAccountGrid* AccountGrid = nullptr;

	virtual ~SmFundAccountGrid();

	virtual void OnHeaderCheckBoxClick(int 	nColumn);

	virtual void OnRowCheckBoxClick(CBCGPGridRow* pRow);


	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val) { _Account = val; }
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }

	void UpdateAcceptedOrder();
	void CancelSelOrders();
	void CancelAll();
	void SetFundDialog(SmFundDialog* fund_dialog) {
		_FundDialog = fund_dialog;
	}
	void SetFundEditDialog(SmFundEditDialog* fund_edit_dialog) {
		_FundEditDialog = fund_edit_dialog;
	}
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	std::string account_type_{ "9" }; // 9: default account type

	SmFundEditDialog* _FundEditDialog = nullptr;
	SmFundDialog* _FundDialog = nullptr;
	bool _EnableOrderShow = false;
	bool _HeaderCheck = false;
	int _OldMaxRow = -1;
	// key : row, value : SmOrder object
	std::map<int, std::shared_ptr<DarkHorse::SmOrder>> _RowToOrderMap;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;
	std::set<int> _OldContentRowSet;
	void ClearOldContents();
	void ClearOldContents(const int& last_index);

	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _RowToAccountMap;
	int _MaxAccountRow{ 0 };
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _RowToFundAccountMap;
	int _MaxFundAccountRow{ 0 };
	int _SelectedIndex = -1;
public:
	void MoveSelectedAccounts();
	void MoveUp();
	void MoveDown();
	void MoveFirstAccount();
	void MoveAllAccounts();
	void ClearAll();
	int MaxFundAccountRow() const { return _MaxFundAccountRow; }
	void MaxFundAccountRow(int val) { _MaxFundAccountRow = val; }
	void ClearFundAccountGrid(const int& row);
	void InitFund(std::shared_ptr<DarkHorse::SmFund> fund);
	void InitFund();
	
	void StartTimer();
	void Update();
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

