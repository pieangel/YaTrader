#pragma once
#include <memory>
#include <vector>
#include <string>
#include <map>

#include "SmGridResource.h"
#include "../SmMenuDefine.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
}


class SmSymbolTableDialog;
class AbAccountOrderWindow;
class SmFundOrderDialog;
class SmFavoriteArea : public CBCGPStatic
{
public:
	SmFavoriteArea();
	~SmFavoriteArea();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	size_t GetRowCount();
	void OnQuoteEvent(const std::string& symbol_code);
private:
	bool _EnableQuoteShow = false;
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	// key : row index, value : smsymbol object
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToSymbolMap;
	SmFundOrderDialog* _FundOrderWnd = nullptr;
	AbAccountOrderWindow* _OrderWnd = nullptr;
public:
	void SetMainWnd(AbAccountOrderWindow* main_wnd) {
		_OrderWnd = main_wnd;
	}
	void SetFundOrderWnd(SmFundOrderDialog* fund_order_wnd) {
		_FundOrderWnd = fund_order_wnd;
	}
	void Clear();
	void SetFavorite();
	void Update();
	void UpdateQuote();
	void AddSymbol(const int& symbol_id);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMenuRemove();
	afx_msg void OnMenuAdd();
	afx_msg LRESULT OnUmSymbolSelected(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

