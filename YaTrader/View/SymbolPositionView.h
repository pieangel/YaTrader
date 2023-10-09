/*
#pragma once
class SymbolPositionView
{
};
*/

#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../Position/PositionConst.h"
#include "../SmGrid/SmGridResource.h"
#include "../Symbol/SymbolConst.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmFund;
	class QuoteControl;
	class SymbolPositionControl;
}

class SymbolPositionView : public CBCGPStatic
{
public:
	SymbolPositionView();
	~SymbolPositionView();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	void Clear();
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return symbol_; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val);
	void update_quote();
	void  OnEndEditCell(int nRow, int nCol, CString str);
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return account_; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	void fund(std::shared_ptr<DarkHorse::SmFund> val);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void Refresh() {
		enable_position_show_ = true;
		enable_quote_show_ = true;
		Invalidate();
	}
	void on_update_quote();
	void on_update_position();
	DarkHorse::PositionType position_type() const { return position_type_; }
	void position_type(DarkHorse::PositionType val) { position_type_ = val; }
	DarkHorse::SymbolType symbol_type() const { return symbol_type_; }
	void symbol_type(DarkHorse::SymbolType val) { symbol_type_ = val; }
private:
	DarkHorse::SymbolType symbol_type_{ DarkHorse::SymbolType::None };
	void set_position();
	DarkHorse::PositionType position_type_{ DarkHorse::PositionType::None };
	void update_position();
	std::shared_ptr<DarkHorse::SymbolPositionControl> position_control_;
	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	bool enable_position_show_ = false;
	bool enable_quote_show_ = false;
	bool _Editing{ false };
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid{ nullptr };

	CBCGPGraphicsManager* m_pGM{ nullptr };

	std::shared_ptr<DarkHorse::SmSymbol> symbol_{ nullptr };
	std::shared_ptr<DarkHorse::SmAccount> account_{ nullptr };
	std::shared_ptr<DarkHorse::SmFund> fund_{ nullptr };
public:
	// OnLButtonDblClk
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void PreSubclassWindow();
};



