
#pragma once
#include <BCGCBProInc.h>
#include <memory>
#include <vector>
#include <string>
#include <set>
#include <map>
#include "../SmMenuDefine.h"

namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class QuoteControl;
}
class SmSymbolTableDialog;
class AbAccountOrderWindow;
class AbFundOrderWindow;
class SmOrderCompMainDialog;
class SmFundCompMainDialog;
class FavoriteSymbolView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(FavoriteSymbolView)

		// Construction
public:
	FavoriteSymbolView();

	SmOrderCompMainDialog* CompOrderWnd() const { return _CompOrderWnd; }
	void CompOrderWnd(SmOrderCompMainDialog* val) { _CompOrderWnd = val; }
	// Attributes
protected:
	BOOL m_bExtendedPadding;

	// Operations
public:
	void SetMainWnd(AbAccountOrderWindow* main_wnd) {
		_OrderWnd = main_wnd;
	}
	void SetFundOrderWnd(AbFundOrderWindow* fund_order_wnd) {
		_FundOrderWnd = fund_order_wnd;
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

	virtual ~FavoriteSymbolView();

	virtual void OnHeaderCheckBoxClick(int 	nColumn);

	virtual void OnRowCheckBoxClick(CBCGPGridRow* pRow);


	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val) { _Account = val; }
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	SmFundCompMainDialog* CompFundWnd() const { return _CompFundWnd; }
	void CompFundWnd(SmFundCompMainDialog* val) { _CompFundWnd = val; }

	void UpdateAcceptedOrder();

	void OnQuoteEvent(const std::string& symbol_code);
	// Generated message map functions
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void on_update_quote();
	void update_quote();
	bool _EnableQuoteShow = false;
	SmFundCompMainDialog* _CompFundWnd = nullptr;
	SmOrderCompMainDialog* _CompOrderWnd = nullptr;
	AbFundOrderWindow* _FundOrderWnd = nullptr;
	AbAccountOrderWindow* _OrderWnd = nullptr;
	bool _HeaderCheck = false;
	int _OldMaxRow = -1;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::set<int> _OldContentRowSet;
	void ClearOldCotents();
	void ClearOldContents(const int& last_index);
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	// key : row index, value : smsymbol object
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToSymbolMap;
	// key : symbol id, value : row
	std::map<int, int> symbol_to_row_;
	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	int order_window_id_ = 0;
public:
	void set_order_window_id(int id) { order_window_id_ = id; };
	void Clear();
	void SetFavorite();
	void Update();
	void UpdateQuote();
	void AddSymbol(const int& symbol_id);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnUmSymbolSelected(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMenuRemove();
	afx_msg void OnMenuAdd();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

