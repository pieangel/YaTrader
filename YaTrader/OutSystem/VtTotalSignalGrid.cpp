#include "stdafx.h"
#include "VtTotalSignalGrid.h"
#include <array>
#include <numeric>
#include "VtSignalConnectionGrid.h"
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

VtTotalSignalGrid::VtTotalSignalGrid()
{
	quote_control_ = std::make_shared<DarkHorse::QuoteControl>();
	quote_control_->set_event_handler(std::bind(&VtTotalSignalGrid::on_update_quote, this));
}


VtTotalSignalGrid::~VtTotalSignalGrid()
{
}

void VtTotalSignalGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);
	_RowCount = 100;
	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	//Push Button cell type
	m_nButtonIndex = AddCellType(&m_button);
	CUGCell cell;
	for (int yIndex = 0; yIndex < _RowCount; yIndex++)
	{
		for (int xIndex = 0; xIndex < _ColCount; xIndex++)
		{
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);

			
		}
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(GetSystemMetrics(SM_CXVSCROLL));
	SetHS_Height(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
	InitGrid();
}

void VtTotalSignalGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

int VtTotalSignalGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	if (ID == m_nButtonIndex) {
		return OnButton(ID, col, row, msg, param);
	}

	return TRUE;
}

void VtTotalSignalGrid::OnMouseMove(int col, long row, POINT* point, UINT nFlags, BOOL processed /*= 0*/)
{
	if (_OldSelRow == row)
		return;

	if (_OldSelRow != _ClickedRow && _OldSelRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _OldSelRow);
		}
	}

	if (row != _ClickedRow) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, row, _SelColor);
			QuickRedrawCell(i, row);
		}
	}
	else {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, row, _ClickedColor);
			QuickRedrawCell(i, row);
		}
	}

	_OldSelRow = row;
}

void VtTotalSignalGrid::OnMouseLeaveFromMainGrid()
{
	if (_OldSelRow == _ClickedRow)
		return;

	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
		QuickRedrawCell(i, _OldSelRow);
	}

	_OldSelRow = -2;
}

void VtTotalSignalGrid::OnLClicked(int col, long row, int updn, RECT* rect, POINT* point, int processed)
{
	if (_ClickedRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _ClickedRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _ClickedRow);
		}
	}
	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, row, _ClickedColor);
		QuickRedrawCell(i, row);
	}
	_ClickedRow = row;
}

void VtTotalSignalGrid::OnRClicked(int col, long row, int updn, RECT* rect, POINT* point, int processed)
{
	if (_ClickedRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _ClickedRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _ClickedRow);
		}
	}
	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, row, _ClickedColor);
		QuickRedrawCell(i, row);
	}
	_ClickedRow = row;
}

void VtTotalSignalGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[10] = { "계좌번호", "종목", "포지션", "평균단가", "현재가", "평가손익", "매도주문", "매수주문", "신호차트", "청산" };
	int colWidth[10] = { 100, 100, 40, 100, 100, 100, 100, 100, 124 - GetSystemMetrics(SM_CXVSCROLL), 80 };


	for (int i = 0; i < _ColCount; i++)
	{
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

void VtTotalSignalGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtTotalSignalGrid::Refresh()
{
	ClearCells();
	InitGrid();
}

void VtTotalSignalGrid::RefreshOrders()
{
	auto out_system_vector = mainApp.out_system_manager()->get_active_out_system_map();
	int i = 0;
	for (auto it = out_system_vector.begin(); it != out_system_vector.end(); ++it) {
		auto out_system = it->second;
		if (out_system->order_type() == DarkHorse::OrderType::MainAccount || out_system->order_type() == DarkHorse::OrderType::SubAccount) {
			if (out_system->account()) QuickSetText(0, i, out_system->account()->No().c_str());
		}
		else {
			if (out_system->fund()) QuickSetText(0, i, out_system->fund()->Name().c_str());
		}

		if (out_system->symbol()) QuickSetText(1, i, out_system->symbol()->SymbolCode().c_str());

		// 포지션 표시
		const VmPosition& posi = out_system->position_control()->get_position();
		posi.open_quantity == 0 ? QuickSetText(2, i, _T("없음")) : posi.open_quantity > 0 ? QuickSetText(2, i, _T("매수")) : QuickSetText(2, i, _T("매도"));
		std::string thVal;
		std::string temp;
		if (out_system->symbol()) {
			thVal = DarkHorse::VtStringUtil::format_with_thousand_separator(posi.average_price, out_system->symbol()->decimal());
		}
		else {
			thVal = "0";
		}
		QuickSetText(3, i, thVal.c_str());


		// 현재가 표시
		CUGCell cell;
		GetCell(4, i, &cell);
		auto quote_p = mainApp.QuoteMgr()->find_quote(out_system->symbol()->SymbolCode());
		if (quote_p) {
			thVal = DarkHorse::VtStringUtil::format_with_thousand_separator(quote_p->close / std::pow(10.0, out_system->symbol()->decimal()), out_system->symbol()->decimal());
		}
		else {
			thVal = "0";
		}
		cell.SetText(thVal.c_str());
		cell.LongValue(quote_p->close);
		SetCell(4, i, &cell);

		thVal = DarkHorse::VtStringUtil::format_with_thousand_separator(posi.open_profit_loss, 0);
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

		// 잔고 표시
		if (posi.open_quantity != 0) {
			if (posi.open_quantity > 0) {
				QuickSetNumber(7, i, std::abs(posi.open_quantity));
				QuickSetNumber(6, i, 0);
			}
			else if (posi.open_quantity < 0) {
				QuickSetNumber(6, i, std::abs(posi.open_quantity));
				QuickSetNumber(7, i, 0);
			}
		}
		else {
			QuickSetNumber(6, i, 0);
			QuickSetNumber(7, i, 0);
		}
		// 시그널 이름 표시
		QuickSetText(8, i, out_system->name().c_str());

		GetCell(9, i, &cell);
		cell.SetCellType(m_nButtonIndex);
		cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetFont(&_titleFont);
		cell.SetTextColor(RGB(0, 0, 0));
		cell.SetBackColor(_ClickedColor);
		cell.Tag((void*)out_system.get());
		cell.SetText("청산");
		SetCell(9, i, &cell);

		for (int j = 0; j < _ColCount; ++j) {
			QuickRedrawCell(j, i);
		}
		// 인덱스를 증가 시킨다.
		i++;
	}

	_RowNumber = out_system_vector.size();
}

void VtTotalSignalGrid::on_update_quote()
{
	CString log;
	log.Format("on_update_quote\n");
	TRACE(log);
}

int VtTotalSignalGrid::OnButton(long ID, int col, long row, long msg, long param)
{
	if (msg != 1)
		return -1;

	CString log;
	log.Format("OnButton col = %d, row = %d, msg = %d, param = %d \n", col, row, msg, param);
	TRACE(log);

	CUGCell cell;
	GetCell(col, row, &cell);
	auto found = out_system_map_.find(row);
	if (found == out_system_map_.end()) return 0;
	auto out_system = found->second;
	if (out_system) {
		// 시스템을 청산시킨다.
		out_system->liq_all();
		// 시스템을 없애 버린다.
		mainApp.out_system_manager()->remove_out_system(out_system);
		// 연결창에서 체크를 풀고 시스템을 비활성화 시킨다.
		if (_ConnectGrid) {
			_ConnectGrid->ClearCheck(out_system);
		}
		// 모든 셀을 지운다.
		ClearCells();
		// 그리드를 다시 그린다.
		RefreshOrders();
	}
	return 1;
}

void VtTotalSignalGrid::InitGrid()
{
	RefreshOrders();
}

void VtTotalSignalGrid::ClearCells()
{
	CUGCell cell;
	for (int i = 0; i < _RowNumber + 1; i++)
	{
		for (int j = 0; j < _ColCount; j++)
		{
			QuickSetText(j, i, _T(""));
			QuickSetBackColor(j, i, RGB(255, 255, 255));
			QuickRedrawCell(j, i);
			GetCell(j, i, &cell);
			cell.Tag(nullptr);
			cell.SetCellType(UGCT_NORMAL);
			cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
			SetCell(j, i, &cell);
		}
	}
}
