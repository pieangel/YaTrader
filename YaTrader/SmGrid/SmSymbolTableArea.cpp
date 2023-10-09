#include "stdafx.h"
#include "SmSymbolTableArea.h"


#include "SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Global/SmTotalManager.h"
#include "SmCell.h"
using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SmSymbolTableArea, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

SmSymbolTableArea::SmSymbolTableArea()
{

}

SmSymbolTableArea::~SmSymbolTableArea()
{
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmSymbolTableArea::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	const int col_count = 40;
	
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 40, col_count);

	int colWidth[3] = { 80, 163, 60 };

	int width_sum = 0;
	for (int i = 0; i < 3; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}

	for (int i = 3; i < col_count; i++) {
		_Grid->SetColWidth(i, 80);
		width_sum += 80;
	}
	

	_Grid->AddMergeCell(0, 3, 1, 36);

	_Grid->SetRowHeight(0, 30);
	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("거래소");
		_HeaderTitles.push_back("종목이름");
		_HeaderTitles.push_back("코드");
		_HeaderTitles.push_back("월물");

		_Grid->SetColHeaderTitles(_HeaderTitles);

		_Grid->SetTextHAlign(0, 3, 0);
		_Grid->SetLeftMargin(0, 3, 20);

		for(int i = 0; i < _Grid->RowCount(); i++)
			for (int j = 0; j < 3; j++) {
				_Grid->SetCellType(i, j, DarkHorse::SmCellType::CT_HEADER);
			}
	}
}

void SmSymbolTableArea::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rect;
	GetClientRect(rect);

	if (m_pGM == NULL)
	{
		return;
	}

	m_pGM->BindDC(pDC, rect);

	if (!m_pGM->BeginDraw())
	{
		return;
	}



	m_pGM->FillRectangle(rect, _Resource.GridNormalBrush);
	rect.right -= 1;
	rect.bottom -= 1;

	_Grid->DrawGrid(m_pGM, rect);
	_Grid->DrawCells(m_pGM, rect, true, true);
	_Grid->DrawBorder(m_pGM, rect);
	DrawFixedSelectedCell();
	DrawMovingRect();
	m_pGM->EndDraw();
}

void SmSymbolTableArea::OnMouseMove(UINT nFlags, CPoint point)
{
	_X = point.x;
	_Y = point.y;

	Invalidate();
}

void SmSymbolTableArea::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	auto cell_pos = _Grid->FindRowCol(point.x, point.y);
	auto cell = _Grid->FindCell(cell_pos.first, cell_pos.second);
	const std::string text = cell->Text();

	int symbol_id = -1;
	if (!text.empty()) {
		std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(text);

		if (symbol) symbol_id = symbol->Id();
	}
	GetParent()->SendMessage(UM_SYMBOL_SELECTED, symbol_id, 0);
}

void SmSymbolTableArea::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	if (cell) cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}

void SmSymbolTableArea::SetProductInfo(const int& row_index, const std::string& ec_name, const std::string& product_name, const std::string& product_code)
{
	std::shared_ptr<SmCell> cell = _Grid->FindCell(row_index, 0);
	if (cell) cell->Text(ec_name);
	cell = _Grid->FindCell(row_index, 1);
	if (cell) cell->Text(product_name);
	cell = _Grid->FindCell(row_index, 2);
	if (cell) cell->Text(product_code);
}

void SmSymbolTableArea::SetSymbolList(const int& row_index, const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec)
{
	if (symbol_vec.empty()) return;
	std::shared_ptr<SmCell> cell = nullptr;
	int col_index = 3;
	for (size_t i = 0; i < symbol_vec.size(); i++) {
		auto symbol = symbol_vec[i];
		cell = _Grid->FindCell(row_index, col_index++);
		if (cell) cell->Text(symbol->SymbolCode());
	}
}

void SmSymbolTableArea::DrawFixedSelectedCell()
{
	if (_FixedSelectedCell) {
		_Grid->DrawCell(_FixedSelectedCell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
	}
}

void SmSymbolTableArea::ClearOldSelectedCells()
{
	for (size_t i = 0; i < _SelectedCells.size(); i++) {
		auto cell = _SelectedCells[i];
		_Grid->DrawCell(cell, m_pGM, _Resource.GridNormalBrush, _Resource.GridNormalBrush, _Resource.TextBrush, _Resource.TextFormat, true, false, false);
	}
	_SelectedCells.clear();
}

void SmSymbolTableArea::DrawMovingRect()
{
	ClearOldSelectedCells();
	auto cell = _Grid->FindCellByPos(_X, _Y);
	if (!cell) return;
	if (cell->Row() > 0 && cell->Col() >= 3) {
		_SelectedCells.push_back(cell);
		_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
	}
}

void SmSymbolTableArea::CreateResource()
{
	_Resource.OrderStroke.SetStartCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_ROUND);
	_Resource.OrderStroke.SetEndCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_TRIANGLE);

	CBCGPTextFormat fmt2(_T("굴림"), globalUtils.ScaleByDPI(30.0f));

	fmt2.SetFontSize(12);
	fmt2.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt2.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt2.SetClipText();
	fmt2.SetWordWrap();
	_Resource.TextFormat = fmt2;

	CBCGPTextFormat fmt3(_T("굴림"), globalUtils.ScaleByDPI(30.0f));

	fmt3.SetFontSize(12);
	fmt3.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_LEADING);
	fmt3.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt3.SetClipText();
	fmt3.SetWordWrap();
	
	_Resource.LeftTextFormat = fmt3;


	CBCGPTextFormat fmt4(_T("굴림"), globalUtils.ScaleByDPI(30.0f));

	fmt4.SetFontSize(12);
	fmt4.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_TRAILING);
	fmt4.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt4.SetClipText();
	fmt4.SetWordWrap();

	_Resource.RightTextFormat = fmt4;
}

void SmSymbolTableArea::InitHeader()
{

}

void SmSymbolTableArea::ClearValues()
{
	for(int i = 1; i < _Grid->RowCount(); i++)
		for (int j = 0; j < _Grid->ColCount(); j++) {
			std::shared_ptr<SmCell> cell = _Grid->FindCell(i, j);
			if (cell) cell->Text("");
		}
}

int SmSymbolTableArea::GetRowCount()
{
	return _Grid->RowCount();
}



BOOL SmSymbolTableArea::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int distance = zDelta / 60;
	if (abs(zDelta) > 120)
		distance = zDelta / 120;
	else
		distance = zDelta / 40;

	GetParent()->SendMessage(UM_PRODUCT_WHEELED, distance, 0);

	return CBCGPStatic::OnMouseWheel(nFlags, zDelta, pt);
}


void SmSymbolTableArea::OnLButtonDown(UINT nFlags, CPoint point)
{
	auto cell = _Grid->FindCellByPos(_X, _Y);
	if (cell && cell->Col() >= 3) { _FixedSelectedCell = cell; }

	const std::string text = cell->Text();
	
	int symbol_id = -1;
	if (!text.empty()) {
		std::shared_ptr<DarkHorse::SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(text);
		
		if (symbol) symbol_id = symbol->Id();
	}
	GetParent()->SendMessage(UM_SYMBOL_SELECTED, symbol_id, 0);

	CBCGPStatic::OnLButtonDown(nFlags, point);
}
