

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
class SmFundAccountGrid;
class SmFundGrid : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(SmFundGrid)

	// Construction
public:
	SmFundGrid();

	// Attributes
protected:
	BOOL m_bExtendedPadding;

	// Operations
public:
	void ClearAll();
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

	virtual ~SmFundGrid();

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
	SmFundAccountGrid* FundAccountGrid = nullptr;

	void AddFund(std::shared_ptr<DarkHorse::SmFund> fund);
	// Generated message map functions
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SmFundDialog* _FundDialog = nullptr;
	bool _EnableOrderShow = false;
	bool _HeaderCheck = false;
	int _OldMaxRow = -1;
	// key : row, value : SmOrder object
	std::map<int, std::shared_ptr<DarkHorse::SmOrder>> _RowToOrderMap;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::set<int> _OldContentRowSet;
	void ClearOldContents();
	void ClearOldContents(const int& last_index);

	// key : row index, value : SmFund object
	std::map<int, std::shared_ptr<DarkHorse::SmFund>> _RowToFundMap;
	int _MaxFundRow = 0;
public:
	void UpdateFundInfo(std::shared_ptr<DarkHorse::SmFund> fund);
	std::shared_ptr<DarkHorse::SmFund> GetFirstFund();
	std::shared_ptr<DarkHorse::SmFund> GetLastFund();
	void ClearFundGrid(const int& row);
	void SetFundList();
	void StartTimer();
	void Update();
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};