#pragma once
#include <memory>
#include <vector>
#include <string>
#include <set>
#include "SmGridResource.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmFund;
}

class SmJangoPositionArea : public CBCGPStatic
{
public:
	SmJangoPositionArea();
	~SmJangoPositionArea();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
public:
	void Clear();
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) {
		_Symbol = val;
		UpdateSymbolInfo();
		UpdateAccountPositionInfo();
	}
	void UpdateSymbolInfo();
	void UpdateAccountPositionInfo();
	void UpdateFundPositionInfo();
	void  OnEndEditCell(int nRow, int nCol, CString str);
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val) {
		_Account = val;

	}
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void Refresh() {
		_EnableOrderShow = true;
		_EnableQuoteShow = true;
		Invalidate();
	}
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return _Fund; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val) { _Fund = val; }
private:
	// 0 : account, 1 : fund
	int _Mode = 0;
	void ClearOldCotents();
	std::set<int> _OldContentRowSet;
	bool _EnableOrderShow = false;
	bool _EnableQuoteShow = false;
	bool _Editing{ false };
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid{ nullptr };

	CBCGPGraphicsManager* m_pGM{ nullptr };

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol{ nullptr };
	std::shared_ptr<DarkHorse::SmAccount> _Account{ nullptr };
	std::shared_ptr<DarkHorse::SmFund> _Fund{ nullptr };
	
public:
	// OnLButtonDblClk
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void PreSubclassWindow();
};



