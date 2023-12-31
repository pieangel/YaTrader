#pragma once
#include "../UGrid/VtGrid.h"
#include "../UGrid/CellTypes/UGCTSeperateText.h"
#include <map>
#include <vector>
#include <memory>
#include "../UGrid/CellTypes/ugctelps.h"
#include "../UGrid/CellTypes/ugctspin.h"
#include "../UGrid/CellTypes/UGCTbutn.h"

#define ELLIPSISBUTTON_CLICK_ACNT		150
#define ELLIPSISBUTTON_CLICK_PRDT		151
#define SPIN_TYPE_SEUNGSU               152
#define ELLIPSISBUTTON_CLICK_ALGO_SET		153
namespace DarkHorse {
	class SmAccount;
	class SmFund;
	class SmOutSystem;
	class SmSymbol;
	class SmUsdSystem;
}
class VtOrderLogDlg;
class SmActiveUsdSystemGrid;
class SmUSDSystemDialog;
class UsdSystemDefGrid : public VtGrid
{
public:
	UsdSystemDefGrid();
	virtual ~UsdSystemDefGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed = 0);
	//cell type notifications
	int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	virtual void OnMouseLeaveFromMainGrid();
	void SetColTitle();
	int _ColCount = 14;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	void InitGrid();

	void ClearCells();

	//Ellipsis Button cell type
	CUGEllipsisType		m_ellipsis;
	int					m_nEllipsisIndex;
	//Push Button cell type
	CUGButtonType		m_button;
	int					m_nButtonIndex;

	//Spin Button cell type
	CUGSpinButtonType	m_spin;
	int					m_nSpinIndex;
	void SetTargetAcntOrFund(std::tuple<int, std::shared_ptr<DarkHorse::SmAccount>, std::shared_ptr<DarkHorse::SmFund>>& selItem);
	void SetSymbol(std::shared_ptr<DarkHorse::SmSymbol> sym);
	void AddSystem(std::shared_ptr<DarkHorse::SmUsdSystem> sys);
	void RemoveSystem();
	//SmActiveUsdSystemGrid* TotalGrid() const { return active_usd_system_grid_; }
	//void TotalGrid(SmActiveUsdSystemGrid* val) { active_usd_system_grid_ = val; }
	void Refresh();
	void SetCheck(bool flag);
	void RefreshOrders();
	void ClearCheck(std::shared_ptr<DarkHorse::SmUsdSystem> sys);
	SmActiveUsdSystemGrid* Active_usd_system_grid() const { return active_usd_system_grid_; }
	void Active_usd_system_grid(SmActiveUsdSystemGrid* val) { active_usd_system_grid_ = val; }
	void update_usd_system(std::shared_ptr<DarkHorse::SmUsdSystem> sys);
	SmUSDSystemDialog* Source_dialog() const { return source_dialog_; }
	void Source_dialog(SmUSDSystemDialog* val) { source_dialog_ = val; }
private:
	SmUSDSystemDialog* source_dialog_ = nullptr;
	void refresh_row(const int row, std::shared_ptr<DarkHorse::SmUsdSystem> sys);
	void set_symbol_from_out(const int window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void set_account_from_out(const int window_id, std::shared_ptr<DarkHorse::SmAccount> account);
	void set_fund_from_out(const int window_id, std::shared_ptr<DarkHorse::SmFund> fund);
	int id_{ 0 };
	int _SelRow = -2;
	int _OldSelRow = -2;
	int _ClickedRow = -2;
	int _OccupiedRowCount = 0;
	COLORREF _SelColor = RGB(255, 227, 132);
	COLORREF _ClickedColor = RGB(216, 234, 253);
	//celltype notification handlers
	int OnDropList(long ID, int col, long row, long msg, long param);
	int OnCheckbox(long ID, int col, long row, long msg, long param);
	int OnEllipsisButton(long ID, int col, long row, long msg, long param);
	int OnSpinButton(long ID, int col, long row, long msg, long param);
	int OnLogButton(int col, long row);
	int OnButton(long ID, int col, long row, long msg, long param);
	// 키 : 행인덱스, 값 : 시스템 객체
	std::map<int, std::shared_ptr<DarkHorse::SmUsdSystem>> _SystemMap;
	// 키 : 시스템 아이디, 값 : 행 인덱스
	std::map<int, int> _SystemToRowMap;
	SmActiveUsdSystemGrid* active_usd_system_grid_ = nullptr;
	VtOrderLogDlg* _LogDlg = nullptr;
	int _ButtonCol = -2;
	int _ButtonRow = -2;
};


