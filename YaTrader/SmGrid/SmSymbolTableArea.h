#pragma once

#include <memory>
#include <vector>
#include <string>
#include "SmGridResource.h"
#define UM_PRODUCT_WHEELED WM_USER + 2
#define UM_SYMBOL_SELECTED WM_USER + 3
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
}

class SmSymbolTableArea : public CBCGPStatic
{
public:
	SmSymbolTableArea();
	~SmSymbolTableArea();
	void SetUp();

	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	void SetProductInfo(const int& row_index, const std::string& ec_name, const std::string& product_name, const std::string& product_code);
	void SetSymbolList(const int& row_index, const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec);
	void ClearValues();
	int GetRowCount();
	void FixedSelectedCell(std::shared_ptr<DarkHorse::SmCell> val) { _FixedSelectedCell = val; }
private:
	void DrawFixedSelectedCell();
	int _X{ 0 };
	int _Y{ 0 };
	void ClearOldSelectedCells();
	void DrawMovingRect();
	std::shared_ptr<DarkHorse::SmCell> _FixedSelectedCell{ nullptr };
	std::vector<std::shared_ptr<DarkHorse::SmCell>> _SelectedCells;
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
