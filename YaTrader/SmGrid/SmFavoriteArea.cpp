#include "stdafx.h"
#include "SmFavoriteArea.h"


#include "SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "SmCell.h"
#include "../Symbol/SmSymbolTableDialog.h"
#include "../resource.h"
#include "../Symbol/SmSymbolManager.h"

#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include <format>

#include <functional>

#include "../Order/AbAccountOrderWindow.h"
#include "../Fund/SmFundOrderDialog.h"

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SmFavoriteArea, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()

	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEHOVER()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_MENU_REMOVE, &SmFavoriteArea::OnMenuRemove)
	ON_COMMAND(ID_MENU_ADD, &SmFavoriteArea::OnMenuAdd)
	ON_MESSAGE(UM_SYMBOL_SELECTED, &SmFavoriteArea::OnUmSymbolSelected)
	ON_WM_TIMER()
END_MESSAGE_MAP()

SmFavoriteArea::SmFavoriteArea()
{

}

SmFavoriteArea::~SmFavoriteArea()
{
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmFavoriteArea::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 20, 2);

	//LPCTSTR title[2] = { "종목", "현재가" };
	int colWidth[2] = { 81, 80 };

	int width_sum = 0;
	for (int i = 0; i < 2; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	width_sum -= colWidth[1];
	_Grid->SetColWidth(1, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);
	_Grid->GridLineWidth(2);
	
	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("종목");
		_HeaderTitles.push_back("현재가");

		_Grid->SetColHeaderTitles(_HeaderTitles);
	}

	mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&SmFavoriteArea::OnQuoteEvent, this, _1));

	SetTimer(1, 40, NULL);
}

void SmFavoriteArea::OnPaint()
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
	_Grid->DrawCells(m_pGM, rect, true, false);
	//_Grid->DrawHorizontalHeader(m_pGM, _HeaderTitles, 0);
	_Grid->DrawBorder(m_pGM, rect);

	m_pGM->EndDraw();
}

void SmFavoriteArea::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	if (cell) cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}

size_t SmFavoriteArea::GetRowCount()
{
	if (!_Grid) return 0;
	else return _Grid->RowCount();
}

void SmFavoriteArea::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void SmFavoriteArea::CreateResource()
{
	_Resource.OrderStroke.SetStartCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_ROUND);
	_Resource.OrderStroke.SetEndCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_TRIANGLE);
	CBCGPTextFormat fmt3(_T("굴림"), globalUtils.ScaleByDPI(30.0f));

	fmt3.SetFontSize(12);
	fmt3.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt3.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt3.SetWordWrap();
	fmt3.SetClipText();

	_Resource.TextFormat = fmt3;
}

void SmFavoriteArea::InitHeader()
{
	_HeaderTitles.push_back("종목");
	_HeaderTitles.push_back("현재가");
}



void SmFavoriteArea::Clear()
{
	if (!_Grid) return;

	for (int row = 1; row < _Grid->RowCount(); row++) {
		for (int col = 0; col < _Grid->ColCount(); col++) {
			auto cell = _Grid->FindCell(row, col);
			if (cell) cell->Text("");
		}
	}
}

void SmFavoriteArea::SetFavorite()
{
	Clear();
	// 반드시 실시간 등록을 해줄것
	const std::map<int, std::shared_ptr<SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	int row = 1;
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		_RowToSymbolMap[row] = it->second;
		auto cell = _Grid->FindCell(row, 0);
		if (cell) cell->Text(it->second->SymbolCode());
		cell = _Grid->FindCell(row, 1);
		std::string value;
		std::string value_precision;
		value_precision.append("{0:.");
		value_precision.append(std::to_string(it->second->decimal()));
		value_precision.append("f}");
		value = std::format("{0:.2f}", static_cast<double>(it->second->Qoute.close));
		if (cell) cell->Text(value);
		row++;
	}
	Invalidate();
}

void SmFavoriteArea::Update()
{
	if (_EnableQuoteShow) {
		UpdateQuote();
		_EnableQuoteShow = false;
	}
}

void SmFavoriteArea::UpdateQuote()
{
	const std::map<int, std::shared_ptr<SmSymbol>>& favorite_map = mainApp.SymMgr()->GetFavoriteMap();
	int row = 0;
	for (auto it = favorite_map.begin(); it != favorite_map.end(); ++it) {
		_RowToSymbolMap[row] = it->second;
		

		std::string value;
		std::string value_precision;
		value_precision.append("{0:.");
		value_precision.append(std::to_string(it->second->decimal()));
		value_precision.append("f}");
		value = std::format("{0:.2f}", static_cast<double>(it->second->Qoute.close));
		
		auto cell = _Grid->FindCell(row, 1);
		cell->Text(value);
		row++;
	}
	Invalidate();
}

void SmFavoriteArea::AddSymbol(const int& symbol_id)
{
	auto symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	if (!symbol) return;

	mainApp.SymMgr()->AddFavorite(symbol_id);

	SetFavorite();
}

void SmFavoriteArea::OnLButtonDown(UINT nFlags, CPoint point)
{
	auto cell = _Grid->FindCellByPos(point.x, point.y);
	if (!cell) return;

	auto found = _RowToSymbolMap.find(cell->Row());
	if (found == _RowToSymbolMap.end()) return;

	if (_OrderWnd) _OrderWnd->OnSymbolClicked(found->second->SymbolCode());
	if (_FundOrderWnd) _FundOrderWnd->OnSymbolClicked(found->second->SymbolCode());

	CBCGPStatic::OnLButtonDown(nFlags, point);
}


void SmFavoriteArea::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CBCGPStatic::OnLButtonDblClk(nFlags, point);
}


BOOL SmFavoriteArea::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	return CBCGPStatic::OnMouseWheel(nFlags, zDelta, pt);
}


void SmFavoriteArea::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CBCGPStatic::OnMouseMove(nFlags, point);
}


void SmFavoriteArea::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default

	CBCGPStatic::OnMouseLeave();
}


void SmFavoriteArea::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CBCGPStatic::OnMouseHover(nFlags, point);
}


void SmFavoriteArea::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu;

	// 팝업 메뉴를 생성한다.
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_MENU_REMOVE, _T("Remove"));
	menu.AppendMenu(MF_STRING, ID_MENU_ADD, _T("Add"));

	CPoint pt = point;
	ClientToScreen(&pt);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	menu.DestroyMenu();

	CBCGPStatic::OnRButtonDown(nFlags, point);
}

void SmFavoriteArea::OnMenuRemove()
{
	AfxMessageBox("Remove");
}

void SmFavoriteArea::OnMenuAdd()
{
	_SymbolTableDlg = std::make_shared<SmSymbolTableDialog>(this);
	_SymbolTableDlg->Create(IDD_SYMBOL_TABLE, this);
	_SymbolTableDlg->ShowWindow(SW_SHOW);
}

LRESULT SmFavoriteArea::OnUmSymbolSelected(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = static_cast<int>(wParam);
	auto symbol = mainApp.SymMgr()->FindSymbolById(symbol_id);
	if (!symbol) return 0; 

	mainApp.SymMgr()->AddFavorite(symbol_id);

	SetFavorite();

	return 1;
}


void SmFavoriteArea::OnTimer(UINT_PTR nIDEvent)
{
	Update();

	CBCGPStatic::OnTimer(nIDEvent);
}
