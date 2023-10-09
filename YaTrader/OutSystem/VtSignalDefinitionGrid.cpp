#include "stdafx.h"
#include "VtSignalDefinitionGrid.h"
#include "../Global/SmConst.h"
#include "SmOutSystemManager.h"
#include "../Global/SmTotalManager.h"
#include "../OutSystem/SmOutSignalDef.h"
VtSignalDefinitionGrid::VtSignalDefinitionGrid()
{
}


VtSignalDefinitionGrid::~VtSignalDefinitionGrid()
{
}

void VtSignalDefinitionGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

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
	SetVScrollMode(UG_SCROLLNORMAL);
	InitGrid();
}


void VtSignalDefinitionGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	if (col == 0 && row == 0)
		RedrawAll();
}

void VtSignalDefinitionGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	//if (_ClickedRow == row)
	//	return;

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

void VtSignalDefinitionGrid::OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	//if (_ClickedRow == row)
	//	return;

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

void VtSignalDefinitionGrid::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed /*= 0*/)
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

void VtSignalDefinitionGrid::OnMouseLeaveFromMainGrid()
{
	if (_OldSelRow == _ClickedRow)
		return;

	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
		QuickRedrawCell(i, _OldSelRow);
	}

	_OldSelRow = -2;
}

void VtSignalDefinitionGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[4] = { "½ÅÈ£Â÷Æ®", "¼³¸í" };
	int colWidth[4] = { 100, 71 - GetSystemMetrics(SM_CXVSCROLL) };


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

void VtSignalDefinitionGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtSignalDefinitionGrid::InitGrid()
{
	auto outSigDefMgr = mainApp.out_system_manager();
	int i = 0;
	const std::vector<std::shared_ptr<DarkHorse::SmOutSignalDef>>& sigDefVec = outSigDefMgr->get_out_system_signal_map();
	for (auto it = sigDefVec.begin(); it != sigDefVec.end(); ++it) {
		const std::shared_ptr<DarkHorse::SmOutSignalDef>& sig = *it;
		QuickSetText(0, i, sig->name.c_str());
		QuickSetText(1, i, sig->desc.c_str());
		QuickRedrawCell(0, i);
		QuickRedrawCell(1, i);
		i++;
	}
}

void VtSignalDefinitionGrid::ClearCells()
{
	CUGCell cell;
	for (int i = 0; i < _RowCount; i++) {
		for (int j = 0; j < _ColCount; j++) {
			QuickSetText(j, i, _T(""));
			QuickSetBackColor(j, i, RGB(255, 255, 255));
			QuickRedrawCell(j, i);
			GetCell(j, i, &cell);
			cell.Tag(nullptr);
			SetCell(j, i, &cell);
		}
	}
}
/*
void VtSignalDefinitionGrid::AddOutSigDef(SharedOutSigDef sig)
{
	VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
	OutSigDefVec& sigDefVec = outSigDefMgr->GetSignalDefVec();
	int yIndex = sigDefVec.size() - 1;
	QuickSetText(0, yIndex, sig->SignalName.c_str());
	QuickSetText(1, yIndex, sig->Desc.c_str());
	QuickRedrawCell(0, yIndex);
	QuickRedrawCell(1, yIndex);
}
*/
