/*
class AccountOrderView
{
};
*/

#pragma once
#include <BCGCBProInc.h>
#include <memory>
#include <vector>
#include <string>
#include <set>
#include <map>

namespace DarkHorse {
	class AccountOrderControl;
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmOrder;
	class SmFund;
	struct Order;

}
class DmAccountOrderWindow;
class DmFundOrderWindow;
class SmOrderCompMainDialog;
class SmFundCompMainDialog;
class AccountOrderView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(AccountOrderView)

		// Construction
public:
	AccountOrderView();

	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return _Fund; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	SmOrderCompMainDialog* CompOrderWnd() const { return _CompOrderWnd; }
	void CompOrderWnd(SmOrderCompMainDialog* val) { _CompOrderWnd = val; }
	SmFundCompMainDialog* CompFundWnd() const { return _CompFundWnd; }
	void CompFundWnd(SmFundCompMainDialog* val) { _CompFundWnd = val; }
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
	void on_update_order();
	void SetFundOrderWnd(DmFundOrderWindow* fund_order_wnd) {
		_FundOrderWnd = fund_order_wnd;
	}
	void SetMainWnd(DmAccountOrderWindow* main_wnd) {
		_OrderWnd = main_wnd;
	}
	virtual ~AccountOrderView();

	virtual void OnHeaderCheckBoxClick(int 	nColumn);

	virtual void OnRowCheckBoxClick(CBCGPGridRow* pRow);


	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }

	void UpdateAcceptedOrder();
	void CancelSelOrders();
	void CancelAll();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool updating_ = false;
	void update_order();
	void ClearCheck();
	SmFundCompMainDialog* _CompFundWnd = nullptr;
	SmOrderCompMainDialog* _CompOrderWnd = nullptr;
	DmFundOrderWindow* _FundOrderWnd = nullptr;
	DmAccountOrderWindow* _OrderWnd = nullptr;
	COLORREF _DefaultBackColor;
	bool enable_order_show_ = false;
	bool _HeaderCheck = false;
	int _OldMaxRow = -1;
	// key : row, value : SmOrder object
	std::map<int, std::shared_ptr<DarkHorse::SmOrder>> _RowToOrderMap;
	std::map<int, std::shared_ptr<DarkHorse::Order>> row_to_order_;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;
	std::set<int> _OldContentRowSet;
	std::shared_ptr<DarkHorse::AccountOrderControl> account_order_control_;
	void ClearOldContents();
	void ClearOldContents(const int& last_index);

	void UpdateAccountAcceptedOrders();
	void UpdateFundAcceptedOrders();
	// 0 : account, 1 : fund
	int _Mode = 0;
	void CancelSelAccountOrders();
	void CancelSelFundOrders();
	void CancelAllAccountOrders();
	void CancelAllFundOrders();
	int id_{ 0 };
	int main_window_id_ = 0;
public:
	void set_order_window_id(int id) { main_window_id_ = id; };
	void on_timer();
	void StartTimer();
	void Update();
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

