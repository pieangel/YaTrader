/*
#pragma once
class AccountPositionView
{
};
*/
#pragma once



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
	struct SmPosition;
	class SmFund;
	struct Position;
	class AccountPositionControl;
}

class DmAccountOrderWindow;
class DmFundOrderWindow;
class SmOrderCompMainDialog;
class SmFundCompMainDialog;
using position_p = std::shared_ptr<DarkHorse::Position>;

class AccountPositionView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(AccountPositionView)

		// Construction
public:
	void start_timer();
	AccountPositionView();

	std::shared_ptr<DarkHorse::SmFund> Fund() const { return fund_; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	SmOrderCompMainDialog* CompOrderWnd() const { return _CompOrderWnd; }
	void CompOrderWnd(SmOrderCompMainDialog* val) { _CompOrderWnd = val; }
	// Attributes
protected:
	BOOL m_bExtendedPadding;

	// Operations
public:
	void SetFundOrderWnd(DmFundOrderWindow* fund_order_wnd) {
		_FundOrderWnd = fund_order_wnd;
	}

	void SetMainWnd(DmAccountOrderWindow* main_wnd) {
		_OrderWnd = main_wnd;
	}
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

	virtual ~AccountPositionView();

	virtual void OnHeaderCheckBoxClick(int 	nColumn);

	virtual void OnRowCheckBoxClick(CBCGPGridRow* pRow);


	std::shared_ptr<DarkHorse::SmAccount> Account() const { return account_; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }

	void UpdatePositionInfo();
	void LiqSelPositions();
	void LiqAll();
	void on_update_single_position(const int position_id);
	void on_update_whole_position(const int result);
	// Generated message map functions
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void on_timer();
	void ClearCheck();
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void OnQuoteEvent(const std::string& symbol_code);
	SmFundCompMainDialog* CompFundWnd() const { return _CompFundWnd; }
	void CompFundWnd(SmFundCompMainDialog* val) { _CompFundWnd = val; }
	void set_column_widths(std::vector<int> column_width_vector);
	void set_column_widths(const std::string& type);
	void set_column_names(const std::string& type);
private:
	bool updating_ = false;
	void LiqSelPositionsForAccount();
	void LiqSelPositionsForFund();
	void LiqAllForAccount();
	void LiqAllForFund();
	SmFundCompMainDialog* _CompFundWnd = nullptr;
	DmFundOrderWindow* _FundOrderWnd = nullptr;
	DmAccountOrderWindow* _OrderWnd = nullptr;
	SmOrderCompMainDialog* _CompOrderWnd = nullptr;
	// 0 : account , 1 : fund
	int _Mode = 0;
	void update_account_position();
	void update_dm_account_position(CBCGPGridRow* row, position_p position, const std::string& format_type);
	void update_ab_account_position(CBCGPGridRow* row, position_p position, const std::string& format_type);
	bool _EnableQuoteShow = false;
	bool enable_position_show_ = false;
	COLORREF _DefaultBackColor;
	bool _HeaderCheck = false;
	int _OldMaxRow = -1;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> account_ = nullptr;
	std::shared_ptr<DarkHorse::SmFund> fund_ = nullptr;

	std::set<int> _OldContentRowSet;
	void ClearOldContents();
	void ClearOldContents(const int& last_index);
	std::shared_ptr<DarkHorse::AccountPositionControl> account_position_control_;
	// key : row, value : position object.
	std::map<int, std::shared_ptr<DarkHorse::Position>> row_to_position_;
	// key : position object id, value : row.
	std::map<int, int> position_to_row_;
	std::vector<int> ab_column_widths_vector_;
	std::vector<int> dm_column_widths_vector_;
	int id_{ 0 };
	int main_window_id_ = 0;
public:
	void set_order_window_id(int id) { main_window_id_ = id; };
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
