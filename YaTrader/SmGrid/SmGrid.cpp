#include "stdafx.h"
#include "SmGrid.h"
#include "SmGridConsts.h"
#include "SmCell.h"
#include "../Order/SmOrderGridConst.h"
#include "../Grid/InPlaceEdit.h"
#include "SmRow.h"

int DarkHorse::SmGrid::IsInMergedCells(const int& row, const int& col)
{
	if (_MergedCellMap.empty()) return -1;

	for (auto it = _MergedCellMap.begin(); it !=  _MergedCellMap.end(); it++) {
		std::pair<int, int> cell_pos = it->first;
		std::pair<int, int> cell_span = it->second;
		if (cell_pos.first == row && cell_pos.second == col)
			return 0;
		if ((cell_pos.first <= row && row < cell_pos.first + cell_span.first) && 
			(cell_pos.second <= col && col < cell_pos.second + cell_span.second))
			return 1;
	}

	return -1;
}

void DarkHorse::SmGrid::DrawHeader(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());

	pGM->FillRectangle(rect, _Res.HeaderBackBrush);

	CBCGPRect text_rect(cell->X() + cell->LeftMargin(), cell->Y(), cell->X() + cell->Width() - cell->LeftMargin(), cell->Y() + cell->Height());

	if (cell->TextHAlign() == 0) pGM->DrawText(cell->Text().c_str(), text_rect, _Res.LeftTextFormat, _Res.TextBrush);
	else if (cell->TextHAlign() == 2) pGM->DrawText(cell->Text().c_str(), text_rect, _Res.RightTextFormat, _Res.TextBrush);
	else pGM->DrawText(cell->Text().c_str(), text_rect, _Res.TextFormat, _Res.TextBrush);

	pGM->DrawLine(rect.left, rect.top, rect.right, rect.top, _Res.HeaderButtonBrush);
	pGM->DrawLine(rect.left, rect.top, rect.left, rect.bottom, _Res.HeaderButtonBrush);
	pGM->DrawLine(rect.right, rect.top, rect.right, rect.bottom, _Res.HeaderButtonBrush);
	pGM->DrawLine(rect.left, rect.bottom, rect.right, rect.bottom, _Res.HeaderButtonBrush);
}

void DarkHorse::SmGrid::DrawCheckHeader(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());

	pGM->FillRectangle(rect, _Res.HeaderBackBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.TextBrush);

	pGM->DrawLine(rect.left + rect.Width() / 4, rect.top + rect.Height() / 2.0, rect.left + rect.Width() / 2.0, rect.bottom - rect.Height() / 4.0, _Res.HeaderButtonBrush);
	pGM->DrawLine(rect.left + rect.Width() / 2.0, rect.bottom - rect.Height() / 4.0, rect.right - rect.Height() / 5, rect.top + rect.Height() / 3, _Res.HeaderButtonBrush);

	pGM->DrawLine(rect.left, rect.top, rect.right, rect.top, _Res.HeaderButtonBrush);
	pGM->DrawLine(rect.left, rect.top, rect.left, rect.bottom, _Res.HeaderButtonBrush);
	pGM->DrawLine(rect.right, rect.top, rect.right, rect.bottom, _Res.HeaderButtonBrush);
	pGM->DrawLine(rect.left, rect.bottom, rect.right, rect.bottom, _Res.HeaderButtonBrush);
}



void DarkHorse::SmGrid::SetAllRowHeight(const int& height)
{
	cell_height_ = height;
	for (auto it = _RowHeightMap.begin(); it != _RowHeightMap.end(); ++it) {
		_RowHeightMap[it->first] = cell_height_;
	}
}

void DarkHorse::SmGrid::OnEndEdit()
{
	m_pEditWnd = nullptr;
}

bool DarkHorse::SmGrid::Edit(CWnd* wnd, int nRow, int nCol, CRect rect, UINT nID, UINT nChar)
{
	DWORD dwStyle = ES_CENTER;

	m_pEditWnd = new CInPlaceEdit(wnd, rect, dwStyle, nID, nRow, nCol, "", nChar);
	return true;
}

void DarkHorse::SmGrid::RecalCells()
{
// 	int y = _RowGridHeight;
// 	for (int row = 0; row < _RowCount; row++) {
// 		int x = _ColGridWidth;
// 		std::shared_ptr<SmRow> row_obj = std::make_shared<SmRow>(row);
// 		for (int col = 0; col < _ColCount; col++) {
// 			// 병합된 셀이면 건너 뛴다.
// 			const int mr = IsInMergedCells(row, col);
// 			if (mr == 1) {
// 				x += (_ColWidthMap[col] + _ColGridWidth);
// 				continue;
// 			}
// 			const auto key = std::make_pair(row, col);
// 			const auto found = _CellMap.find(key);
// 			if (found != _CellMap.end()) continue;
// 
// 			std::shared_ptr<SmCell> cell = found->second;
// 			cell->Row(row);
// 			cell->Col(col);
// 			cell->RowSpan(1);
// 			cell->ColSpan(1);
// 			cell->X(x);
// 			cell->Y(y);
// 			cell->Width(_ColWidthMap[col]);
// 			cell->Height(_RowHeightMap[row]);
// 
// 			if (cell->Row() == _HeaderRow) {
// 				cell->CellType(SmCellType::CT_HEADER);
// 			}
// 			// 병합한 셀일 때
// 			if (mr == 0) {
// 				auto cell_span = _MergedCellMap[std::make_pair(row, col)];
// 				cell->RowSpan(cell_span.first);
// 				cell->ColSpan(cell_span.second);
// 				int cell_height = 0;
// 				for (int i = row; i < row + cell->RowSpan(); ++i) {
// 					cell_height += _RowHeightMap[i];
// 					cell_height += _RowGridHeight;
// 				}
// 				cell_height -= _RowGridHeight;
// 				cell->Height(cell_height);
// 
// 				int cell_width = 0;
// 				for (int i = col; i < col + cell->ColSpan(); ++i) {
// 					cell_width += _ColWidthMap[i];
// 					cell_width += _ColGridWidth;
// 				}
// 				cell_width -= _ColGridWidth;
// 				cell->Width(cell_width);
// 			}
// 			//_CellMap[std::make_pair(row, col)] = cell;
// 			x += (_ColWidthMap[col] + _ColGridWidth);
// 		}
// 		y += (_RowHeightMap[row] + _RowGridHeight);
// 	}
}



void DarkHorse::SmGrid::RegisterOrderButtons(std::map<std::shared_ptr<DarkHorse::SmCell>, BUTTON_ID>& order_button_map)
{
	order_button_map.clear();
	SetCellType(1, DarkHorse::OrderHeader::SELL_CNT, SmCellType::CT_BUTTON_SELL);
	order_button_map[FindCell(1, DarkHorse::OrderHeader::SELL_CNT)] = BUTTON_ID::SELL;
	SetCellType(1, DarkHorse::OrderHeader::BUY_QTY, SmCellType::CT_BUTTON_BUY);
	order_button_map[FindCell(1, DarkHorse::OrderHeader::BUY_QTY)] = BUTTON_ID::BUY;
	SetCellText(1, DarkHorse::OrderHeader::SELL_CNT, "시장가매도");
	SetCellText(1, DarkHorse::OrderHeader::BUY_QTY, "시장가매수");

	SetCellType(1, DarkHorse::OrderHeader::QUOTE, SmCellType::CT_BUTTON_NORMAL);
	//_ButtonSet.insert(std::make_pair(1, DarkHorse::OrderGridHeader::QUOTE));
	order_button_map[FindCell(1, DarkHorse::OrderHeader::QUOTE)] = BUTTON_ID::ARRANGE;
	SetCellText(1, DarkHorse::OrderHeader::QUOTE, "호가정렬");

	SetCellType(_RowCount - 1, DarkHorse::OrderHeader::SELL_STOP, SmCellType::CT_BUTTON_SELL);
	SetCellType(_RowCount - 1, DarkHorse::OrderHeader::SELL_ORDER, SmCellType::CT_BUTTON_SELL);
	SetCellType(_RowCount - 1, DarkHorse::OrderHeader::QUOTE, SmCellType::CT_BUTTON_NORMAL);
	SetCellType(_RowCount - 1, DarkHorse::OrderHeader::BUY_ORDER, SmCellType::CT_BUTTON_BUY);
	SetCellType(_RowCount - 1, DarkHorse::OrderHeader::BUY_STOP, SmCellType::CT_BUTTON_BUY);

	_ButtonSet.insert(std::make_pair(_RowCount - 1, DarkHorse::OrderHeader::SELL_STOP));
	_ButtonSet.insert(std::make_pair(_RowCount - 1, DarkHorse::OrderHeader::SELL_ORDER));
	_ButtonSet.insert(std::make_pair(_RowCount - 1, DarkHorse::OrderHeader::QUOTE));
	_ButtonSet.insert(std::make_pair(_RowCount - 1, DarkHorse::OrderHeader::BUY_ORDER));
	_ButtonSet.insert(std::make_pair(_RowCount - 1, DarkHorse::OrderHeader::BUY_STOP));

	order_button_map[FindCell(_RowCount - 1, DarkHorse::OrderHeader::SELL_STOP)] = BUTTON_ID::CANCEL_SELL_STOP;
	order_button_map[FindCell(_RowCount - 1, DarkHorse::OrderHeader::SELL_ORDER)] = BUTTON_ID::CANCEL_SELL_ORDER;
	order_button_map[FindCell(_RowCount - 1, DarkHorse::OrderHeader::QUOTE)] = BUTTON_ID::CANCEL_ALL;
	order_button_map[FindCell(_RowCount - 1, DarkHorse::OrderHeader::BUY_ORDER)] = BUTTON_ID::CANCEL_BUY_ORDER;
	order_button_map[FindCell(_RowCount - 1, DarkHorse::OrderHeader::BUY_STOP)] = BUTTON_ID::CANCEL_BUY_STOP;

	SetCellText(_RowCount - 1, DarkHorse::OrderHeader::SELL_STOP, "취소");
	SetCellText(_RowCount - 1, DarkHorse::OrderHeader::SELL_ORDER, "취소");
	SetCellText(_RowCount - 1, DarkHorse::OrderHeader::QUOTE, "전체취소");
	SetCellText(_RowCount - 1, DarkHorse::OrderHeader::BUY_STOP, "취소");
	SetCellText(_RowCount - 1, DarkHorse::OrderHeader::BUY_ORDER, "취소");
}

void DarkHorse::SmGrid::DrawNormalCell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;

	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.TextBrush);

	if (cell->ShowMark()) {
		
		const int mark_width = 5;
		CBCGPRect right_pos_rect(cell->X() + cell->Width() - mark_width, cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
		switch (cell->MarkType())
		{
		case 0: pGM->FillRectangle(right_pos_rect, _Res.QMNormalBrush); break;
		case 1: pGM->FillRectangle(right_pos_rect, _Res.QMBuyBrush); break;
		case 2: pGM->FillRectangle(right_pos_rect, _Res.QMSellBrush); break;
		case 3: pGM->FillRectangle(right_pos_rect, _Res.QMHighLowBrush); break;
		default: break;
		}
		
	}
}

void DarkHorse::SmGrid::DrawButtonBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->DrawBeveledRectangle(rect, _Res.BuyButtonColor, 1, FALSE);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.TextBrush);
}

void DarkHorse::SmGrid::DrawButtonSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->DrawBeveledRectangle(rect, _Res.SellButtonColor, 1, FALSE);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.TextBrush);
}

void DarkHorse::SmGrid::DrawButtonNormal(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->DrawBeveledRectangle(rect, _Res.HeaderBackColor, 1, FALSE);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.TextBrush);
}

void DarkHorse::SmGrid::DrawQuoteClose(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;

	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.SelectedBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.CloseTextBrush);

	CBCGPRect quote_rect(cell->X(), cell->Y(), cell->X() + cell->Height(), cell->Y() + cell->Height());
	pGM->DrawText("C", quote_rect, _Res.QuoteTextFormat, _Res.CloseTextColor);

	
}

void DarkHorse::SmGrid::DrawQuotePreClose(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.CloseTextBrush);

	CBCGPRect quote_rect(cell->X(), cell->Y(), cell->X() + cell->Height(), cell->Y() + cell->Height());
	pGM->DrawText("C", quote_rect, _Res.QuoteTextFormat, _Res.QuotePreCloseBrush);
}

void DarkHorse::SmGrid::DrawQuoteOpen(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.CloseTextBrush);

	CBCGPRect quote_rect(cell->X(), cell->Y(), cell->X() + cell->Height(), cell->Y() + cell->Height());
	pGM->DrawText("O", quote_rect, _Res.QuoteTextFormat, _Res.CloseTextBrush);
}

void DarkHorse::SmGrid::DrawQuoteHigh(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.BuyHogaTextBrush);

	pGM->DrawRectangle(rect, _Res.BuyHogaTextBrush);

	CBCGPRect quote_rect(cell->X(), cell->Y(), cell->X() + cell->Height(), cell->Y() + cell->Height());
	pGM->DrawText("H", quote_rect, _Res.QuoteTextFormat, _Res.BuyHogaTextBrush);
}

void DarkHorse::SmGrid::DrawQuoteLow(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.SellHogaTextBrush);

	pGM->DrawRectangle(rect, _Res.SellHogaTextBrush);

	CBCGPRect quote_rect(cell->X(), cell->Y(), cell->X() + cell->Height(), cell->Y() + cell->Height());
	pGM->DrawText("L", quote_rect, _Res.QuoteTextFormat, _Res.SellHogaTextBrush);


}

void DarkHorse::SmGrid::DrawPositionBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;

	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.TextBrush);

	CBCGPRect right_pos_rect(cell->X() + cell->Width() - cell->Height() * 2 + 10, cell->Y(), cell->X() + cell->Width() - cell->Height() + 10, cell->Y() + cell->Height());
	right_pos_rect.DeflateRect(5, 5);

	//CBCGPBrush brFill3(CBCGPColor::Red, CBCGPColor::White, CBCGPBrush::BCGP_GRADIENT_RADIAL_CENTER);
	pGM->FillEllipse(right_pos_rect, _Res.BuyPositionBrush);

	pGM->DrawEllipse(right_pos_rect, _Res.TextBrush);
}

void DarkHorse::SmGrid::DrawPositionSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;

	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.TextBrush);


	CBCGPRect left_pos_rect(cell->X() + cell->Height() - 10, cell->Y(), cell->X() + cell->Height() * 2 - 10, cell->Y() + cell->Height());
	left_pos_rect.DeflateRect(5, 5);

	//CBCGPBrush brFill2(CBCGPColor::Blue, CBCGPColor::White, CBCGPBrush::BCGP_GRADIENT_RADIAL_CENTER);
	pGM->FillEllipse(left_pos_rect, _Res.SellPositionBrush);

	
	pGM->DrawEllipse(left_pos_rect, _Res.TextBrush);
}

void DarkHorse::SmGrid::DrawHogaSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.SellHogaTextBrush);
}

void DarkHorse::SmGrid::DrawHogaBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.BuyHogaTextBrush);
}

void DarkHorse::SmGrid::DrawOrderBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.TextBrush);
}

void DarkHorse::SmGrid::DrawOrderSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.TextBrush);
}

void DarkHorse::SmGrid::DrawCheck(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;
	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	//rect.DeflateRect(2, 2);
	//pGM->DrawCheckBox(rect, BST_UNCHECKED, _Res.QuoteTextColor);
	pGM->DrawLine(rect.left + rect.Width() / 4, rect.top + rect.Height() / 2.0, rect.left + rect.Width() / 2.0, rect.bottom - rect.Height() / 4.0, _Res.HeaderButtonBrush);
	pGM->DrawLine(rect.left + rect.Width() / 2.0, rect.bottom - rect.Height() / 4.0, rect.right - rect.Height() / 5, rect.top + rect.Height() / 3, _Res.HeaderButtonBrush);
}

void DarkHorse::SmGrid::DrawTickBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;

	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.BuyHogaTextBrush);
}

void DarkHorse::SmGrid::DrawTickSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;

	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.GridNormalBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.SellHogaTextBrush);
}

void DarkHorse::SmGrid::DrawRemainSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;

	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.SellPositionBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.QuoteTextColor);
}

void DarkHorse::SmGrid::DrawRemainBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell)
{
	if (!cell) return;

	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	pGM->FillRectangle(rect, _Res.BuyPositionBrush);
	pGM->DrawText(cell->Text().c_str(), rect, _Res.TextFormat, _Res.QuoteTextColor);
}

void DarkHorse::SmGrid::SetColCellType(const int& col, const SmCellType& cell_type)
{
	for (auto it = _RowMap.begin(); it != _RowMap.end(); ++it) {
		auto row_obj = it->second;
		auto cell = row_obj->FindCell(col);
		if (cell) cell->CellType(cell_type);
	}
}

void DarkHorse::SmGrid::SetRowHeaderTitles(const std::vector<std::string>& row_titles)
{
	//_RowHeaderTitles.assign(row_titles.begin(), row_titles.end());
	for (size_t i = 0; i < row_titles.size(); i++) {
		if (i == 0 && _HeaderMode == SmHeaderMode::HeaderBoth && _HeaderPriority == SmHeaderPriority::ColFirst) continue;
		auto cell = FindCell(i, 0);
		if (cell) {
			cell->Text(row_titles[i].c_str());
			cell->CellType(SmCellType::CT_HEADER);
		}
	}
}

void DarkHorse::SmGrid::SetColHeaderTitles(const std::vector<std::string>& col_titles)
{
	//_ColHeaderTitles.assign(col_titles.begin(), col_titles.end());
	for (size_t i = 0; i < col_titles.size(); i++) {
		if (i == 0 && _HeaderMode == SmHeaderMode::HeaderBoth && _HeaderPriority == SmHeaderPriority::RowFirst) continue;
		auto cell = FindCell(0, i);
		if (cell) {
			cell->Text(col_titles[i].c_str());
			cell->CellType(SmCellType::CT_HEADER);
		}
	}
}

void DarkHorse::SmGrid::SetColHeaderTitles(const std::vector<std::string>& col_titles, const std::vector< SmCellType>& cell_types)
{
	for (size_t i = 0; i < col_titles.size(); i++) {
		if (i == 0 && _HeaderMode == SmHeaderMode::HeaderBoth && _HeaderPriority == SmHeaderPriority::RowFirst) continue;
		auto cell = FindCell(0, i);
		if (cell) {
			cell->Text(col_titles[i].c_str());
			cell->CellType(cell_types[i]);
		}
	}
}

// void DarkHorse::SmGrid::ReleaseOrderButtons()
// {
// 	for (auto it = _ButtonSet.begin(); it != _ButtonSet.end(); ++it) {
// 		std::shared_ptr<SmCell> pCell = FindCell(it->first, it->second);
// 		if (pCell) {
// 			pCell->Text(""); pCell->CellType(SmCellType::CT_NORMAL);
// 		}
// 	}
// }


void DarkHorse::SmGrid::ReleaseOrderButtons(const std::map<std::shared_ptr<DarkHorse::SmCell>, BUTTON_ID>& order_button_map)
{
	for (auto it = order_button_map.begin(); it != order_button_map.end(); it++) {
		it->first->Text(""); it->first->CellType(SmCellType::CT_NORMAL);
	}
}

std::pair<int, int> DarkHorse::SmGrid::FindRowCol(const int& x, const int& y)
{
	int row = -1, col = -1;
	int acc_measure = 0;
	for (auto it_row = _RowHeightMap.begin(); it_row != _RowHeightMap.end(); it_row++) {
		acc_measure += (it_row->second + _RowGridLineHeight); 
		if (y < 0) break;
		else if (y <= acc_measure) {	row = it_row->first; break; }
	}
	acc_measure = 0;
	for (auto it_col = _ColWidthMap.begin(); it_col != _ColWidthMap.end(); it_col++) {
		acc_measure += it_col->second;
		if (x < 0) break;
		else if (x <= acc_measure) { col = it_col->first; break; }
	}
	return std::make_pair(row, col);
}

void DarkHorse::SmGrid::SetOrderHeaderTitles()
{
	_RowHeaderTitles.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_STOP].title);
	_RowHeaderTitles.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_ORDER].title);
	_RowHeaderTitles.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_CNT].title);
	_RowHeaderTitles.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_QTY].title);
	_RowHeaderTitles.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::QUOTE].title);
	_RowHeaderTitles.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_QTY].title);
	_RowHeaderTitles.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_CNT].title);
	_RowHeaderTitles.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_ORDER].title);
	_RowHeaderTitles.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_STOP].title);
}

int DarkHorse::SmGrid::RecalRowCount(const int& height, bool change_close_row)
{
	_GridHeight = _RowGridLineHeight;
	size_t row_count = 0, each_row_height = 0;
	for (;; row_count++) {
		if (_RowHeightMap.size() == row_count) break;
		each_row_height = _RowHeightMap[row_count] + _RowGridLineHeight;
		_GridHeight += each_row_height;
		if (_GridHeight >= height) { if (_GridHeight == height) row_count++;  break; }
	}
	
	_RowCount = row_count;

	if (_GridHeight > height) _GridHeight -= each_row_height;

	const int extra_height = height - _GridHeight;

	if (change_close_row) index_row_ = static_cast<int>(row_count / 2.0);

	return extra_height;
}

void DarkHorse::SmGrid::MakeColWidthMap()
{
	for (int i = 0; i < _ColCount; i++) {
		auto found = _ColWidthMap.find(i);
		if (found == _ColWidthMap.end()) {
			_ColWidthMap[i] = DefaultColWidth;
		}
	}
}

void DarkHorse::SmGrid::MakeRowHeightMap()
{
	for (int i = 0; i < _RowCount; i++) {
		auto found = _RowHeightMap.find(i);
		if (found == _RowHeightMap.end()) {
			_RowHeightMap[i] = DefaultRowHeight;
		}
	}
}

void DarkHorse::SmGrid::SetMergeCells()
{
	_MergedCellMap[std::make_pair(1, 2)] = std::make_pair(1, 2);
	_MergedCellMap[std::make_pair(1, 5)] = std::make_pair(1, 2);
}

void DarkHorse::SmGrid::AddMergeCell(const int& row, const int& col, const int& row_span, const int& col_span)
{
	_MergedCellMap[std::make_pair(row, col)] = std::make_pair(row_span, col_span);
}

void DarkHorse::SmGrid::Clear()
{
	for(size_t row = 1; row < (size_t)_RowCount; row++)
		for (size_t col = 0; col < (size_t)_ColCount; col++) {
			auto cell = FindCell(row, col);
			if (cell) {
				cell->CellType(CT_NORMAL);
				cell->Text("");
			}
		}
}

void DarkHorse::SmGrid::DrawCell(std::shared_ptr<SmCell> cell, CBCGPGraphicsManager* pGM, const CBCGPBrush& back_brush, const CBCGPBrush& border_brush, const CBCGPBrush& text_brush, const CBCGPTextFormat& text_format, const bool& fill, const bool& draw_border, const bool& draw_text)
{
	if (!cell || !pGM) return;

	CBCGPRect rect(cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height());
	if (fill) pGM->FillRectangle(rect, back_brush);
	if (draw_border) pGM->DrawRectangle(rect, border_brush);
	if (draw_text)  pGM->DrawText(cell->Text().c_str(), rect, text_format, text_brush);
}

std::shared_ptr<DarkHorse::SmCell> DarkHorse::SmGrid::FindCellByPos(const int& x, const int& y)
{
	auto row_col = FindRowCol(x, y);
	return FindCell(row_col.first, row_col.second);
}

std::shared_ptr<DarkHorse::SmCell> DarkHorse::SmGrid::FindCell(const int& row, const int& col)
{
	auto found_row = _RowMap.find(row);
	if (found_row == _RowMap.end()) return nullptr;
	return found_row->second->FindCell(col);
}

void DarkHorse::SmGrid::SetCellMark(const int& row, const int& col, const bool& show_mark)
{
	std::shared_ptr<DarkHorse::SmCell> cell = FindCell(row, col);
	if (!cell) return;

	cell->ShowMark(show_mark);
}

void DarkHorse::SmGrid::SetCellType(const int& row, const int& col, const SmCellType& cell_type)
{
	std::shared_ptr<DarkHorse::SmCell> cell = FindCell(row, col);
	if (!cell) return;

	cell->CellType(cell_type);
}

void DarkHorse::SmGrid::SetCellText(const int& row, const int& col, const std::string& text)
{
	std::shared_ptr<DarkHorse::SmCell> cell = FindCell(row, col);
	if (!cell) return;

	cell->Text(text);
}

void DarkHorse::SmGrid::SetTextHAlign(const int& row, const int& col, const int h_align)
{
	std::shared_ptr<DarkHorse::SmCell> cell = FindCell(row, col);
	if (!cell) return;

	cell->TextHAlign(h_align);
}

void DarkHorse::SmGrid::SetLeftMargin(const int& row, const int& col, const int& margin)
{
	std::shared_ptr<DarkHorse::SmCell> cell = FindCell(row, col);
	if (!cell) return;

	cell->LeftMargin(margin);
}

void DarkHorse::SmGrid::SetRightMargin(const int& row, const int& col, const int& margin)
{
	std::shared_ptr<DarkHorse::SmCell> cell = FindCell(row, col);
	if (!cell) return;

	cell->TextHAlign(margin);
}

void DarkHorse::SmGrid::CreateGrids()
{
	//_RowHeightMap[2] = 40;
	//MakeColWidthMap();
	// 여기서는 행과 열 정보를 이용하여 셀 목록을 만든다.
	// 위에서 부터 아래로 만들어 간다.
	int y = _RowGridLineHeight;
	for (int row = 0; row < _RowCount; row++) {
		int x = _ColGridWidth;
		std::shared_ptr<SmRow> row_obj = std::make_shared<SmRow>(row);
		_RowMap[row] = row_obj;
		for (int col = 0; col < _ColCount; col++) {
			// 병합된 셀이면 건너 뛴다.
			const int mr = IsInMergedCells(row, col);
			if (mr == 1) {
				x += (_ColWidthMap[col] + _ColGridWidth);
				continue;
			}
			auto found = FindCell(row, col);
			if (found) continue;

			std::shared_ptr<SmCell> cell = std::make_shared<SmCell>();
			cell->Row(row);
			cell->Col(col);
			cell->RowSpan(1);
			cell->ColSpan(1);
			cell->X(x);
			cell->Y(y);
			cell->Width(_ColWidthMap[col]);
			cell->Height(_RowHeightMap[row]);

			
			// 병합한 셀일 때
			if (mr == 0) {
				auto cell_span = _MergedCellMap[std::make_pair(row, col)];
				cell->RowSpan(cell_span.first);
				cell->ColSpan(cell_span.second);
				int cell_height = 0;
				for (int i = row; i < row + cell->RowSpan(); ++i) {
					cell_height += _RowHeightMap[i];
					cell_height += _RowGridLineHeight;
				}
				cell_height -= _RowGridLineHeight;
				cell->Height(cell_height);

				int cell_width = 0;
				for (int i = col; i < col + cell->ColSpan(); ++i) {
					cell_width += _ColWidthMap[i];
					cell_width += _ColGridWidth;
				}
				cell_width -= _ColGridWidth;
				cell->Width(cell_width);
			}
			//_CellMap[std::make_pair(row, col)] = cell;
			row_obj->AddCell(cell);
			x += (_ColWidthMap[col] + _ColGridWidth);
		}
		y += (_RowHeightMap[row] + _RowGridLineHeight);
	}
}


void DarkHorse::SmGrid::DrawGrid(CBCGPGraphicsManager* pGM, CRect& wnd_area)
{
	if (!pGM) return;
	int y = 0;
	int acc_grid_width = 0;
	for (int row = 0; row <= _RowCount; row++) {
		pGM->DrawLine(0, y, wnd_area.Width(), y, _Res.RowGridBrush, _GridLineWidth);
		auto it = _RowHeightMap.find(row);
		if (it == _RowHeightMap.end())
			y += (DefaultRowHeight);
		else
			y += (it->second);
		y += _RowGridLineHeight;
	}

	int x = 0;
	for (int col = 0; col <= _ColCount; col++) {
		pGM->DrawLine(x, 0, x, _GridHeight, _Res.ColGridBrush, _GridLineWidth);
		auto it = _ColWidthMap.find(col);
		if (it == _ColWidthMap.end())
			x += (DefaultColWidth);
		else
			x += (it->second);
		x += _ColGridWidth;
	}
}

void DarkHorse::SmGrid::DrawBorder(CBCGPGraphicsManager* pGM, CRect& wnd_area, const bool& selected)
{
	pGM->DrawRectangle(wnd_area, selected ? _Res.SelectedBrush : _Res.GridBorderBrush, 2.0);
}

void DarkHorse::SmGrid::DrawCells(CBCGPGraphicsManager* pGM, CRect& wnd_area, const bool& use_hor_header /*= false*/, const bool& use_ver_header /*= false*/)
{
	for (auto row = _RowMap.begin(); row != _RowMap.end(); row++) {
		const auto& row_obj = row->second;
		if (row_obj->RowIndex() >= _RowCount) continue;

		const std::map<int, std::shared_ptr<SmCell>>& row_cell_list = row_obj->GetCellList();
		for (auto col = row_cell_list.begin(); col != row_cell_list.end(); col++) {
			const auto& cell = col->second;
			switch (cell->CellType())
			{
			case SmCellType::CT_HEADER: DrawHeader(pGM, cell); break;
			case SmCellType::CT_CHECK_HEADER: DrawCheckHeader(pGM, cell); break;
			case SmCellType::CT_NORMAL: DrawNormalCell(pGM, cell); break;
			case SmCellType::CT_BUTTON_NORMAL: DrawButtonNormal(pGM, cell); break;
			case SmCellType::CT_BUTTON_BUY: DrawButtonBuy(pGM, cell); break;
			case SmCellType::CT_BUTTON_SELL: DrawButtonSell(pGM, cell); break;
			case SmCellType::CT_QUOTE_CLOSE: DrawQuoteClose(pGM, cell); break;
			case SmCellType::CT_QUOTE_PRECLOSE: DrawQuotePreClose(pGM, cell); break;
			case SmCellType::CT_QUOTE_OPEN: DrawQuoteOpen(pGM, cell); break;
			case SmCellType::CT_QUOTE_HIGH: DrawQuoteHigh(pGM, cell); break;
			case SmCellType::CT_QUOTE_LOW: DrawQuoteLow(pGM, cell); break;
			case SmCellType::CT_POSITION_BUY: DrawPositionBuy(pGM, cell); break;
			case SmCellType::CT_POSITION_SELL: DrawPositionSell(pGM, cell); break;
			case SmCellType::CT_ORDER_BUY: DrawOrderBuy(pGM, cell); break;
			case SmCellType::CT_ORDER_SELL: DrawOrderSell(pGM, cell); break;
			case SmCellType::CT_HOGA_SELL: DrawHogaSell(pGM, cell); break;
			case SmCellType::CT_HOGA_BUY: DrawHogaBuy(pGM, cell); break;
			case SmCellType::CT_CHECK: DrawCheck(pGM, cell); break;
			case SmCellType::CT_TICK_BUY: DrawTickBuy(pGM, cell); break;
			case SmCellType::CT_TICK_SELL: DrawTickSell(pGM, cell); break;
			case SmCellType::CT_REMAIN_BUY: DrawRemainBuy(pGM, cell); break;
			case SmCellType::CT_REMAIN_SELL: DrawRemainSell(pGM, cell); break;
			default: break;
			}
		}
	}
}

void DarkHorse::SmGrid::draw_cells(CBCGPGraphicsManager* pGM, CRect& wnd_area, const bool& use_hor_header /*= false*/, const bool& use_ver_header /*= false*/)
{
	for (auto row = _RowMap.begin(); row != _RowMap.end(); row++) {
		const auto& row_obj = row->second;
		if (row_obj->RowIndex() >= _RowCount) continue;

		const std::map<int, std::shared_ptr<SmCell>>& row_cell_list = row_obj->GetCellList();
		for (auto col = row_cell_list.begin(); col != row_cell_list.end(); col++) {
			const auto& cell = col->second;
			cell->draw(pGM, _Res);
		}
	}
}

void DarkHorse::SmGrid::DrawSelectedCell(CBCGPGraphicsManager* pGM, const CBCGPBrush& fill_brush, const CBCGPBrush& select_brush, bool selected)
{
	// 먼저 이전 셀을 지운다.
}

void DarkHorse::SmGrid::SetRowHeight(const int& row, const int& height) 
{
	_RowHeightMap[row] = height;
}

void DarkHorse::SmGrid::SetColWidth(const int& col, const int& width)
{
	_ColWidthMap[col] = width;
}
