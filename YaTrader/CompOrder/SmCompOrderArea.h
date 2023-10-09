
#pragma once
#include <memory>
#include <vector>
#include <string>
#include <set>
#include <map>

#include "../SmGrid/SmGridResource.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	struct SmPosition;
	class SmFund;
}

class AbAccountOrderWindow;
class SmFundOrderDialog;
class SmOrderCompMainDialog;
class SmFundCompMainDialog;
class SmCompOrderArea : public CBCGPStatic
{
public:
	SmCompOrderArea();
	~SmCompOrderArea();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

	
public:
	bool Selected() const { return _Selected; }
	void Selected(bool val) { _Selected = val; }
	void SetMainOrderWnd(SmOrderCompMainDialog* wnd);
	void SetFundCompOrderWnd(SmFundCompMainDialog* wnd);
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return _Fund; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	void SetFundOrderWnd(SmFundOrderDialog* fund_order_wnd) {
		_FundOrderWnd = fund_order_wnd;
	}

	void SetMainWnd(AbAccountOrderWindow* main_wnd) {
		_OrderWnd = main_wnd;
	}
	size_t GetRowCount();
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val);
	void UpdateSymbolInfo();
	void UpdatePositionInfo();
	void LiqSelPositions();
	void LiqAll();
	void Update();
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void OnQuoteEvent(const std::string& symbol_code);
	void OnHogaEvent(const std::string& symbol_code);
	void SetHoga(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void SetQuote(std::shared_ptr<DarkHorse::SmSymbol> symbol);

private:
	bool _Selected = false;
	SmOrderCompMainDialog* _OrderMainWnd = nullptr;
	SmFundCompMainDialog* _FundCompOrderWnd = nullptr;
	void DrawHogaLine(const CRect& rect);
	bool _EnableHogaShow = false;

	std::vector<std::string> _OrderTableHeader;

	void LiqSelPositionsForAccount();
	void LiqSelPositionsForFund();
	void LiqAllForAccount();
	void LiqAllForFund();
	SmFundOrderDialog* _FundOrderWnd = nullptr;
	AbAccountOrderWindow* _OrderWnd = nullptr;
	// 0 : account , 1 : fund
	int _Mode = 0;
	void UpdateAccountPositionInfo();
	void UpdateFundPositionInfo();
	bool _EnableQuoteShow = false;
	bool _EnableOrderShow = false;


	CBCGPScrollBar m_HScroll;
	CBCGPScrollBar m_VScroll;
	void ProcessCheck(const CPoint& point);
	std::set<int> _OldContentRowSet;
	void ClearOldCotents();
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;

	CBCGPGraphicsManager* m_pGM = nullptr;

	COLORREF _DefaultBackColor;
	bool _HeaderCheck = false;
	int _OldMaxRow = -1;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;


	void ClearOldContents();
	void ClearOldContents(const int& last_index);
	// key : row, value : SmPosition Object
	std::map<int, std::shared_ptr<DarkHorse::SmPosition>> _RowToPositionMap;
public:
	void Refresh();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void PreSubclassWindow();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

