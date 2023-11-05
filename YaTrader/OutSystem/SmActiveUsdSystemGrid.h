#pragma once
#include "../UGrid/VtGrid.h"
#include "../UGrid/CellTypes/UGCTSeperateText.h"
//#include "Global/VtDefine.h"
#include "../UGrid/CellTypes/UGCTbutn.h"
#include <map>
#include <vector>
#include <memory>
namespace DarkHorse {
	class QuoteControl;
	//class SmOutSystem;
	class SmUsdSystem;
}
class VtSignalConnectionGrid;
class UsdSystemDefGrid;
class SmActiveUsdSystemGrid : public VtGrid
{
public:
	SmActiveUsdSystemGrid();
	virtual ~SmActiveUsdSystemGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT* rect, POINT* point, BOOL processed);
	//cell type notifications
	int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	virtual void OnMouseMove(int col, long row, POINT* point, UINT nFlags, BOOL processed = 0);
	virtual void OnMouseLeaveFromMainGrid();
	virtual void OnLClicked(int col, long row, int updn, RECT* rect, POINT* point, int processed);
	virtual void OnRClicked(int col, long row, int updn, RECT* rect, POINT* point, int processed);
	//Push Button cell type
	CUGButtonType		m_button;
	int					m_nButtonIndex;

	void SetColTitle();
	int _ColCount = 10;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	void Refresh();
	void RefreshOrders();
	void ClearCells();
	UsdSystemDefGrid* UsdGrid() const { return _UsdGrid; }
	void UsdGrid(UsdSystemDefGrid* val) { _UsdGrid = val; }
private:
	void on_update_quote();
	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	int OnButton(long ID, int col, long row, long msg, long param);
	void InitGrid();
	int _RowNumber = 0;

	int _SelRow = -2;
	int _OldSelRow = -2;
	int _ClickedRow = -2;
	int _ButtonRow = -2;
	COLORREF _SelColor = RGB(255, 227, 132);
	COLORREF _ClickedColor = RGB(216, 234, 253);
	// key : row index, value : out system object.
	std::map<int, std::shared_ptr<DarkHorse::SmUsdSystem>> usd_system_map_;
	UsdSystemDefGrid* _UsdGrid = nullptr;
};


