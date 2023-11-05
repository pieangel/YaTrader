#include "stdafx.h"
#include "VtSignalConnectionGrid.h"
#include "VtTotalSignalGrid.h"
#include "../Log/MyLogger.h"
#include "../OutSystem/SmOutSystem.h"
#include "../OutSystem/SmOutSystemManager.h"
#include "../OutSystem/SmOutSignalDef.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccount.h"
#include "../Fund/SmFund.h"
#include "../Dialog/HdSymbolSelecter.h"
#include "../Util/IdGenerator.h"
#include "../Event/EventHub.h"
#include "../Order/SmOrderConst.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../ViewModel/VmPosition.h"
#include "../OutSystem/SmOutSystem.h"
#include "../Controller/SymbolPositionControl.h"
#include "../Util/VtStringUtil.h"
#include "../Quote/SmQuote.h"
#include "../Controller/QuoteControl.h"
#include "../Quote/SmQuoteManager.h"
#include "../Global/SmConst.h"
#include "../Util/IdGenerator.h"
#include "../Dialog/SmAccountFundSelector.h"

using namespace DarkHorse;
VtSignalConnectionGrid::VtSignalConnectionGrid()
	:id_(IdGenerator::get_id())
{
	mainApp.event_hub()->subscribe_symbol_event_handler(id_, std::bind(&VtSignalConnectionGrid::set_symbol_from_out, this, std::placeholders::_1, std::placeholders::_2));
	mainApp.event_hub()->subscribe_account_event_handler(id_, std::bind(&VtSignalConnectionGrid::set_account_from_out, this, std::placeholders::_1, std::placeholders::_2));
	mainApp.event_hub()->subscribe_fund_event_handler(id_, std::bind(&VtSignalConnectionGrid::set_fund_from_out, this, std::placeholders::_1, std::placeholders::_2));

}


VtSignalConnectionGrid::~VtSignalConnectionGrid()
{
}

void VtSignalConnectionGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	//Ellipsis Button cell type
	m_nEllipsisIndex = AddCellType(&m_ellipsis);
	//Spin Button cell type
	m_nSpinIndex = AddCellType(&m_spin);
	//Push Button cell type
	m_nButtonIndex = AddCellType(&m_button);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	CUGCell cell;
	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(GetSystemMetrics(SM_CXVSCROLL));
	SetHS_Height(0);
	SetColTitle();
	EnableScrollBar(SB_VERT);
	SetVScrollMode(UG_SCROLLNORMAL);
	InitGrid();
}

void VtSignalConnectionGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	if (col == 0 && row == 0)
		RedrawAll();
}

void VtSignalConnectionGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	//if (_ClickedRow == row)
	//	return;

	if (_ClickedRow >= 0) {
		for (int i = 0; i < _ColCount - 1; ++i) {
			QuickSetBackColor(i, _ClickedRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _ClickedRow);
		}
	}
	for (int i = 0; i < _ColCount - 1; ++i) {
		QuickSetBackColor(i, row, _ClickedColor);
		QuickRedrawCell(i, row);
	}
	_ClickedRow = row;
	if (updn == 1) {
		OnLogButton(col, row);
	}
}

void VtSignalConnectionGrid::OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	//if (_ClickedRow == row)
	//	return;

	if (_ClickedRow >= 0) {
		for (int i = 0; i < _ColCount - 1; ++i) {
			QuickSetBackColor(i, _ClickedRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _ClickedRow);
		}
	}
	for (int i = 0; i < _ColCount - 1; ++i) {
		QuickSetBackColor(i, row, _ClickedColor);
		QuickRedrawCell(i, row);
	}
	_ClickedRow = row;
}

void VtSignalConnectionGrid::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed /*= 0*/)
{
	if (_OldSelRow == row)
		return;

	if (_OldSelRow != _ClickedRow && _OldSelRow >= 0) {
		for (int i = 0; i < _ColCount - 1; ++i) {
			QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _OldSelRow);
		}
	}

	if (row != _ClickedRow) {
		for (int i = 0; i < _ColCount - 1; ++i) {
			QuickSetBackColor(i, row, _SelColor);
			QuickRedrawCell(i, row);
		}
	}
	else {
		for (int i = 0; i < _ColCount - 1; ++i) {
			QuickSetBackColor(i, row, _ClickedColor);
			QuickRedrawCell(i, row);
		}
	}

	_OldSelRow = row;
}

int VtSignalConnectionGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param) {
	CString log;
	log.Format("OnCellTypeNotify col = %d, row = %d, msg = %d, param = %d \n", col, row, msg, param);
	TRACE(log);

	if (ID == UGCT_DROPLIST) {
		return OnDropList(ID, col, row, msg, param);
	}
	if (ID == UGCT_CHECKBOX) {
		return OnCheckbox(ID, col, row, msg, param);
	}
	if (ID == m_nEllipsisIndex) {
		return OnEllipsisButton(ID, col, row, msg, param);
	}
	if (ID == m_nSpinIndex) {
		return OnSpinButton(ID, col, row, msg, param);
	}

	if (ID == m_nButtonIndex) {
		return OnButton(ID, col, row, msg, param);
	}
	
	return TRUE;
}


void VtSignalConnectionGrid::OnMouseLeaveFromMainGrid()
{
	if (_OldSelRow == _ClickedRow)
		return;

	for (int i = 0; i < _ColCount - 1; ++i) {
		QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
		QuickRedrawCell(i, _OldSelRow);
	}

	_OldSelRow = -2;
}

void VtSignalConnectionGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[9] = { "실행", "계좌번호", "종목", "신호차트", "승수", "평가손익", "청산손익", "총손익", "로그"};
	int colWidth[9] = { 25, 98, 90, 80, 58, 100, 100, 100, 85 };


	for (int i = 0; i < _ColCount; i++) {
		SetColWidth(i, colWidth[i]);
		GetCell(i, -1, &cell);
		cell.SetText(title[i]);
		cell.SetBackColor(GridTitleBackColor);
		cell.SetTextColor(GridTitleTextColor);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetFont(&_titleFont);
		SetCell(i, -1, &cell);
		QuickRedrawCell(i, -1);
	}
}

void VtSignalConnectionGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtSignalConnectionGrid::InitGrid()
{
	try {
	_SystemMap.clear();
	_SystemToRowMap.clear();
	int yIndex = 0;
	CUGCell cell;
	auto out_system_vector = mainApp.out_system_manager()->get_out_system_vector();
	for (auto it = out_system_vector.begin(); it != out_system_vector.end(); ++it) {
		auto out_system = *it;
		const VmPosition& posi = out_system->position_control()->get_position();
		CString thVal;
		std::string temp;
		for (int xIndex = 0; xIndex < _ColCount; ++xIndex) {
			if (xIndex == 0) {
				GetCell(xIndex, yIndex, &cell);
				out_system->enable() ? cell.SetNumber(1.0) : cell.SetNumber(0.0);
				cell.SetLabelText(_T(""));
				cell.SetCellType(UGCT_CHECKBOX);
				cell.SetCellTypeEx(UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 1) {
				GetCell(xIndex, yIndex, &cell);
				if (out_system->order_type() == OrderType::MainAccount || out_system->order_type() == OrderType::SubAccount) {
					if (out_system->account()) cell.SetText(out_system->account()->No().c_str());
				}
				else {
					if (out_system->fund()) cell.SetText(out_system->fund()->Name().c_str());
				}
				cell.SetCellType(m_nEllipsisIndex);
				cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
				cell.SetParam(ELLIPSISBUTTON_CLICK_ACNT);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 2) {
				GetCell(xIndex, yIndex, &cell);
				if (out_system->symbol()) cell.SetText(out_system->symbol()->SymbolCode().c_str());
				cell.SetCellType(m_nEllipsisIndex);
				cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
				cell.SetParam(ELLIPSISBUTTON_CLICK_PRDT);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 3) {
				GetCell(xIndex, yIndex, &cell);
				cell.SetText(out_system->name().c_str());
				cell.SetCellType(UGCT_DROPLIST);
				cell.SetCellTypeEx(UGCT_DROPLISTHIDEBUTTON);
				cell.SetReadOnly(FALSE);
				cell.SetLabelText(out_system->desc().c_str());
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 4) {
				GetCell(xIndex, yIndex, &cell);
				cell.SetNumber(out_system->seung_su());
				cell.SetCellType(m_nSpinIndex);
				cell.SetParam(SPIN_TYPE_SEUNGSU);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 5) {
				std::string thVal = DarkHorse::VtStringUtil::format_with_thousand_separator(posi.open_profit_loss, out_system->symbol()->decimal());
				// 평가손익 표시
				if (posi.open_profit_loss > 0) {
					QuickSetTextColor(5, yIndex, RGB(255, 0, 0));
					QuickSetText(5, yIndex, thVal.c_str());
				}
				else if (posi.open_profit_loss < 0) {
					QuickSetTextColor(5, yIndex, RGB(0, 0, 255));
					QuickSetText(5, yIndex, thVal.c_str());
				}
				else {
					QuickSetTextColor(5, yIndex, RGB(0, 0, 0));
					QuickSetNumber(5, yIndex, 0);
				}
			}
			else if (xIndex == 6) {
				std::string thVal = DarkHorse::VtStringUtil::format_with_thousand_separator(posi.trade_profit_loss, out_system->symbol()->decimal());
				// 실현손익 표시
				if (posi.trade_profit_loss > 0) {
					QuickSetTextColor(6, yIndex, RGB(255, 0, 0));
					QuickSetText(6, yIndex, thVal.c_str());
				}
				else if (posi.trade_profit_loss < 0) {
					QuickSetTextColor(6, yIndex, RGB(0, 0, 255));
					QuickSetText(6, yIndex, thVal.c_str());
				}
				else {
					QuickSetTextColor(6, yIndex, RGB(0, 0, 0));
					QuickSetNumber(6, yIndex, 0);
				}
			}
			else if (xIndex == 7) {
				double total_profit_loss = posi.open_profit_loss + posi.trade_profit_loss;
				std::string thVal = DarkHorse::VtStringUtil::format_with_thousand_separator(total_profit_loss, out_system->symbol()->decimal());

				// 총손익 표시
				if (total_profit_loss > 0) {
					QuickSetTextColor(7, yIndex, RGB(255, 0, 0));
					QuickSetText(7, yIndex, thVal.c_str());
				}
				else if (total_profit_loss < 0) {
					QuickSetTextColor(7, yIndex, RGB(0, 0, 255));
					QuickSetText(7, yIndex, thVal.c_str());
				}
				else {
					QuickSetTextColor(7, yIndex, RGB(0, 0, 0));
					QuickSetNumber(7, yIndex, 0);
				}
			}
			else if (xIndex == 8) {
				GetCell(xIndex, yIndex, &cell);
				cell.SetNumber(out_system->seung_su());
				cell.SetCellType(m_nButtonIndex);
				cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
				cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
				cell.SetFont(&_titleFont);
				cell.SetTextColor(RGB(0, 0, 0));
				//cell.SetBackColor(RGB(218, 226, 245));
				cell.SetBackColor(_ClickedColor);
				cell.SetText("로그보기");
				SetCell(xIndex, yIndex, &cell);
			}
			QuickRedrawCell(xIndex, yIndex);			
		}
		_SystemMap[yIndex] = out_system;
		_SystemToRowMap[out_system->id()] = yIndex;
		yIndex++;
	}
	_OccupiedRowCount = yIndex;

	}
	catch (std::exception& e) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::ClearCells()
{
	CUGCell cell;
	for (int i = 0; i < _RowCount; i++) {
		for (int j = 0; j < _ColCount; j++) {
			QuickSetText(j, i, _T(""));
			QuickSetBackColor(j, i, RGB(255, 255, 255));
			QuickRedrawCell(j, i);
			GetCell(j, i, &cell);
			cell.Tag(nullptr);
			cell.SetCellType(UGCT_NORMAL);
			SetCell(j, i, &cell);
		}
	}
}

void VtSignalConnectionGrid::SetTargetAcntOrFund(std::tuple<int, std::shared_ptr<SmAccount>, std::shared_ptr<SmFund>>& selItem)
{
	/*
	try {
	CUGCell cell;
	GetCell(1, _ButtonRow, &cell);
	int _Type = std::get<0>(selItem);
	SharedSystem sys = _SystemMap[_ButtonRow];
	cell.LongValue(_Type);
	if (_Type == 0 || _Type == 1) {
		VtAccount* _Account = std::get<1>(selItem);
		cell.SetText(_Account->AccountNo.c_str());
		cell.Tag(_Account);
		if (sys) {
			sys->Account(_Account);
		}
	}
	else {
		VtFund* _Fund = std::get<2>(selItem);
		cell.SetText(_Fund->Name.c_str());
		cell.Tag(_Fund);
		if (sys) {
			sys->Fund(_Fund);
		}
	}
	SetCell(1, _ButtonRow, &cell);
	QuickRedrawCell(1, _ButtonRow);

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
	*/
}

void VtSignalConnectionGrid::SetSymbol(std::shared_ptr<SmSymbol> sym)
{
	try {
	if (!sym)
		return;
	auto sys = _SystemMap[_ButtonRow];
	if (sys) {
		CUGCell cell;
		GetCell(2, _ButtonRow, &cell);
		cell.SetText(sym->SymbolCode().c_str());
		SetCell(2, _ButtonRow, &cell);
		sys->symbol(sym);
		QuickRedrawCell(2, _ButtonRow);
	}

	}
	catch (std::exception& e) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::AddSystem(std::shared_ptr<SmOutSystem> sys)
{
	try {
	auto out_system_vector = mainApp.out_system_manager()->get_out_system_vector();
	CUGCell cell;
	size_t yIndex = out_system_vector.size() == 0 ? 0 : out_system_vector.size() - 1;
	for (size_t xIndex = 0; xIndex < (size_t)_ColCount; ++xIndex) {
		if (xIndex == 0) {
			GetCell(xIndex, yIndex, &cell);
			sys->enable() ? cell.SetNumber(1.0) : cell.SetNumber(0.0);
			cell.SetLabelText(_T(""));
			cell.SetCellType(UGCT_CHECKBOX);
			cell.SetCellTypeEx(UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 1) {
			GetCell(xIndex, yIndex, &cell);
			if (sys->order_type() == OrderType::MainAccount || sys->order_type() == OrderType::SubAccount) {
				if (sys->account()) cell.SetText(sys->account()->No().c_str());
			}
			else {
				if (sys->fund()) cell.SetText(sys->fund()->Name().c_str());
			}
			cell.SetCellType(m_nEllipsisIndex);
			cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
			cell.SetParam(ELLIPSISBUTTON_CLICK_ACNT);
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 2) {
			GetCell(xIndex, yIndex, &cell);
			if (sys->symbol()) cell.SetText(sys->symbol()->SymbolCode().c_str());
			cell.SetCellType(m_nEllipsisIndex);
			cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
			cell.SetParam(ELLIPSISBUTTON_CLICK_PRDT);
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 3) {
			GetCell(xIndex, yIndex, &cell);
			cell.SetText(sys->name().c_str());
			cell.SetCellType(UGCT_DROPLIST);
			cell.SetCellTypeEx(UGCT_DROPLISTHIDEBUTTON);
			cell.SetReadOnly(FALSE);
			cell.SetLabelText(sys->desc().c_str());
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 4) {
			GetCell(xIndex, yIndex, &cell);
			cell.SetNumber(sys->seung_su());
			cell.SetCellType(m_nSpinIndex);
			cell.SetParam(SPIN_TYPE_SEUNGSU);
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 8) {
			GetCell(xIndex, yIndex, &cell);
			cell.SetNumber(sys->seung_su());
			cell.SetCellType(m_nButtonIndex);
			cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
			cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
			cell.SetFont(&_titleFont);
			cell.SetTextColor(RGB(0, 0, 0));
			cell.SetBackColor(RGB(218, 226, 245));
			cell.SetText("로그보기");
			SetCell(xIndex, yIndex, &cell);
		}
		QuickRedrawCell(xIndex, yIndex);
	}
	_SystemMap[yIndex] = sys;
	//mainApp.out_system_manager()->add_out_system(sys);
	_OccupiedRowCount = yIndex;

	}
	catch (std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), " %s 알수없는 오류", __FUNCTION__);
	}
}

void VtSignalConnectionGrid::RemoveSystem()
{
	try {
	int row = _ClickedRow;
	auto it = _SystemMap.find(row);
	if (it != _SystemMap.end()) {
		auto  sys = it->second;
		// 시스템을 정지 시킨다.
		sys->enable(false);
		auto outSysOrderMgr = mainApp.out_system_manager();
		// 주문관리자에서 삭제한다.
		outSysOrderMgr->remove_active_out_system(sys);
		// 주문상태 목록을 리프레쉬 한다.
		if (_TotalGrid) _TotalGrid->Refresh();

		// 시스템 목록에서 삭제한다.
		outSysOrderMgr->remove_out_system(sys);

		// 모든 셀 정보를 초기화 시킨다.
		CUGCell cell;
		for (int i = 0; i < _ColCount; ++i) {
			GetCell(i, row, &cell);
			cell.SetCellType(UGCT_NORMAL);
			cell.SetText(_T(""));
			SetCell(i, row, &cell);
			QuickRedrawCell(i, row);
		}
	}

	}
	catch (std::exception& e) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::Refresh()
{
	ClearCells();
	InitGrid();
}

void VtSignalConnectionGrid::SetCheck(bool flag)
{
	try {
	_TotalGrid->ClearCells();
	int row = 0;
	CUGCell cell;
	auto outSysOrderMgr = mainApp.out_system_manager();
	for (auto it = _SystemMap.begin(); it != _SystemMap.end(); ++it) {
		auto sys = _SystemMap[row];
		GetCell(0, row, &cell);
		if (flag) {
			cell.SetNumber(1);
			sys->enable(true);
			outSysOrderMgr->add_active_out_system(sys);
		}
		else {
			cell.SetNumber(0);
			sys->enable(false);
			outSysOrderMgr->remove_active_out_system(sys);
		}
		SetCell(0, row, &cell);
		QuickRedrawCell(0, row);
		row++;
	}
	if (_TotalGrid) _TotalGrid->Refresh();

	}
	catch (std::exception& e) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::RefreshOrders()
{
	try {
	if (_OccupiedRowCount == 0)
		return;

	int i = 0;
	for (auto it = _SystemMap.begin(); it != _SystemMap.end(); ++it) {
		auto sys = it->second;
	
		
		const VmPosition& posi = sys->position_control()->get_position();

		std::string thVal = DarkHorse::VtStringUtil::format_with_thousand_separator(posi.open_profit_loss, 0);
		// 평가손익 표시
		if (posi.open_profit_loss > 0) {
			QuickSetTextColor(5, i, RGB(255, 0, 0));
			QuickSetText(5, i, thVal.c_str());
		}
		else if (posi.open_profit_loss < 0) {
			QuickSetTextColor(5, i, RGB(0, 0, 255));
			QuickSetText(5, i, thVal.c_str());
		}
		else {
			QuickSetTextColor(5, i, RGB(0, 0, 0));
			QuickSetNumber(5, i, 0);
		}

		thVal = DarkHorse::VtStringUtil::format_with_thousand_separator(posi.trade_profit_loss, 0);
		// 실현손익 표시
		if (posi.trade_profit_loss > 0) {
			QuickSetTextColor(6, i, RGB(255, 0, 0));
			QuickSetText(6, i, thVal.c_str());
		}
		else if (posi.trade_profit_loss < 0) {
			QuickSetTextColor(6, i, RGB(0, 0, 255));
			QuickSetText(6, i, thVal.c_str());
		}
		else {
			QuickSetTextColor(6, i, RGB(0, 0, 0));
			QuickSetNumber(6, i, 0);
		}

		double total_profit_loss = posi.open_profit_loss + posi.trade_profit_loss;

		thVal = DarkHorse::VtStringUtil::format_with_thousand_separator(total_profit_loss, 0);

		// 총손익 표시
		if (total_profit_loss > 0) {
			QuickSetTextColor(7, i, RGB(255, 0, 0));
			QuickSetText(7, i, thVal.c_str());
		}
		else if (total_profit_loss < 0) {
			QuickSetTextColor(7, i, RGB(0, 0, 255));
			QuickSetText(7, i, thVal.c_str());
		}
		else {
			QuickSetTextColor(7, i, RGB(0, 0, 0));
			QuickSetNumber(7, i, 0);
		}
		
		
		for (int j = 5; j < 8; ++j) {
			QuickRedrawCell(j, i);
		}
		// 인덱스를 증가 시킨다.
		i++;
	}

	}
	catch (std::exception& e) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::ClearCheck(std::shared_ptr<SmOutSystem> sys)
{
	if (!sys)
		return;
	auto it = _SystemToRowMap.find(sys->id());
	if (it != _SystemToRowMap.end()) {
		int row = it->second;
		CUGCell cell;
		GetCell(0, row, &cell);
		cell.SetNumber(0);
		sys->enable(false);
		SetCell(0, row, &cell);
		QuickRedrawCell(0, row);
	}
}

void VtSignalConnectionGrid::set_symbol_from_out(const int window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (window_id != id_ || !symbol || _ButtonCol == -2 || _ButtonRow == -2) return;

	try {
		auto sys = _SystemMap[_ButtonRow];
		if (!sys)
			return;

		auto outSysOrderMgr = mainApp.out_system_manager();
		if (sys->enable())
			outSysOrderMgr->remove_active_out_system(sys);

		if (sys) {
			CUGCell cell;
			GetCell(_ButtonCol, _ButtonRow, &cell);
			cell.SetText(symbol->SymbolCode().c_str());
			//cell.Tag(sym);
			SetCell(_ButtonCol, _ButtonRow, &cell);
			sys->symbol(symbol);
			QuickRedrawCell(_ButtonCol, _ButtonRow);
			if (sys->enable())
				outSysOrderMgr->add_active_out_system(sys);
		}

	}
	catch (std::exception& e) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::set_account_from_out(const int window_id, std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (window_id != id_ || !account || _ButtonCol == -2 || _ButtonRow == -2) return;

	try {
		auto sys = _SystemMap[_ButtonRow];
		if (!sys)
			return;

		auto outSysOrderMgr = mainApp.out_system_manager();
		if (sys->enable())
			outSysOrderMgr->remove_active_out_system(sys);

		if (sys) {
			CUGCell cell;
			GetCell(_ButtonCol, _ButtonRow, &cell);
			cell.SetText(account->No().c_str());
			//cell.Tag(sym);
			SetCell(_ButtonCol, _ButtonRow, &cell);
			sys->account(account);
			if (account->is_subaccount()) {
				sys->order_type(OrderType::SubAccount);
			}
			else {
				sys->order_type(OrderType::MainAccount);
			}
			QuickRedrawCell(_ButtonCol, _ButtonRow);
			if (sys->enable())
				outSysOrderMgr->add_active_out_system(sys);
		}

	}
	catch (std::exception& e) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

void VtSignalConnectionGrid::set_fund_from_out(const int window_id, std::shared_ptr<DarkHorse::SmFund> fund)
{
	if (window_id != id_ || !fund || _ButtonCol == -2 || _ButtonRow == -2) return;

	try {
		auto sys = _SystemMap[_ButtonRow];
		if (!sys)
			return;

		auto outSysOrderMgr = mainApp.out_system_manager();
		if (sys->enable())
			outSysOrderMgr->remove_active_out_system(sys);

		if (sys) {
			CUGCell cell;
			GetCell(_ButtonCol, _ButtonRow, &cell);
			cell.SetText(fund->Name().c_str());
			//cell.Tag(sym);
			SetCell(_ButtonCol, _ButtonRow, &cell);
			sys->fund(fund);
			sys->order_type(OrderType::Fund);
			QuickRedrawCell(_ButtonCol, _ButtonRow);
			if (sys->enable())
				outSysOrderMgr->add_active_out_system(sys);
		}

	}
	catch (std::exception& e) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

int VtSignalConnectionGrid::OnDropList(long ID, int col, long row, long msg, long param)
{
	try {
	if (msg == UGCT_DROPLISTSTART) {
		CStringList* pList = (CStringList*)param;
		pList->RemoveAll();
		auto signal_def_vector = mainApp.out_system_manager()->get_out_system_signal_map();
		int selIndex = -1;
		for (size_t i = 0; i < signal_def_vector.size(); i++) {
			pList->AddTail(signal_def_vector[i]->name.c_str());
		}
	}
	if (msg == UGCT_DROPLISTSELECT) {
		CUGCell cell;
		GetCell(col, row, &cell);
		CString oldSigName;
		oldSigName = cell.GetText();
		auto sys = _SystemMap[row];
		if (!sys)
			return TRUE;

		 auto outSysOrderMgr = mainApp.out_system_manager();
		// 먼저 기존 시그널을 없애 준다.
		if (sys->enable())
			outSysOrderMgr->remove_active_out_system(sys);

		// 새로운 시그널을 등록해 준다.
		CString * pString = (CString*)param;
		std::string sigName(*pString);
		sys->name(sigName);
		GetCell(col, row, &cell);
		cell.SetText(sigName.c_str());
		QuickRedrawCell(col, row);
		if (sys->enable())
			outSysOrderMgr->add_active_out_system(sys);
	}

	}
	catch (std::exception& e) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
	}
	
	return TRUE;
}
// 체크박스에 체크하면 시스템 주문 관리자에 등록해 준다.
int VtSignalConnectionGrid::OnCheckbox(long ID, int col, long row, long msg, long param)
{
	try {
		auto  outSysOrderMgr = mainApp.out_system_manager();
		auto sys = _SystemMap[row];
		if (sys) {
			CUGCell cell;
			GetCell(col, row, &cell);
			double num = cell.GetNumber();
			if (num == 1.0) {
				sys->enable(true);
				outSysOrderMgr->add_active_out_system(sys);
			}
			else {
				sys->enable(false);
				outSysOrderMgr->remove_active_out_system(sys);
			}
			if (_TotalGrid) _TotalGrid->Refresh();
		}

	}
	catch (std::exception& e) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOGINFO(CMyLogger::getInstance(), _T(" %s 알수없는 오류"), __FUNCTION__);
	}

	return TRUE;
}

int VtSignalConnectionGrid::OnEllipsisButton(long ID, int col, long row, long msg, long param)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	int nCellTypeIndex = cell.GetCellType();
	int nParam = cell.GetParam();

	if (msg == UGCT_ELLIPSISBUTTONCLICK) {
		if (nParam == ELLIPSISBUTTON_CLICK_ACNT) {
			_ButtonRow = row;
			_ButtonCol = col;
			int mode = 0;
			auto it = _SystemMap.find(row);
			if (it != _SystemMap.end()) {
				auto sys = it->second;
				if (sys->order_type() == OrderType::MainAccount) {
					mode = 0;
				}
				else if (sys->order_type() == OrderType::SubAccount) {
					mode = 0;
				}
				else {
					mode = 1;
				}
			}
			SmAccountFundSelector dlg(nullptr, mode);
			dlg.set_source_id(id_);
			dlg.DoModal();
		}
		else if (nParam == ELLIPSISBUTTON_CLICK_PRDT) {
			_ButtonRow = row;
			_ButtonCol = col;
			HdSymbolSelecter dlg;
			dlg.set_source_window_id(id_);
			dlg.DoModal();
		}
		else {
			MessageBox("The button was clicked", "Cell Type Notification");
		}
	}

	return TRUE;
}

int VtSignalConnectionGrid::OnSpinButton(long ID, int col, long row, long msg, long param)
{
	long num;
	CUGCell cell;
	GetCell(col, row, &cell);
	cell.GetNumber(&num);

	if (msg == UGCT_SPINBUTTONUP) {
		num++;
	}
	if (msg == UGCT_SPINBUTTONDOWN) {
		num--;
		num == 0 ? num = 1 : num = num;
	}
	CString str;
	str.Format("%ld", num);
	cell.SetText(str);
	SetCell(col, row, &cell);

	auto sys = _SystemMap[row];
	sys->seung_su((int)num);

	return TRUE;
}

int VtSignalConnectionGrid::OnLogButton(int col, long row)
{
	if (col != 8)
		return -1;

	
	

	return 1;
}

int VtSignalConnectionGrid::OnButton(long ID, int col, long row, long msg, long param)
{
	if (msg != 1)
		return -1;
	// Specify the path to your text file
	const char* filePath = CMyLogger::cur_log_file.c_str();

	// Use ShellExecute to open the file with Notepad.exe
	HINSTANCE result = ShellExecute(NULL, "open", "notepad.exe", filePath, NULL, SW_SHOWNORMAL);

	// Check the result
	if ((int)result <= 32) {
		// An error occurred
		// You can handle the error here
		// The value returned is the HINSTANCE cast to an integer
		// For more details, you can check the ShellExecute documentation
		return -1;
	}

	return 1;
}
