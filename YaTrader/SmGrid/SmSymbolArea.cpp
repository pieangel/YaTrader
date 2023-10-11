#include "stdafx.h"
#include "SmSymbolArea.h"

#include "SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "SmCell.h"
#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;
using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SmSymbolArea, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

SmSymbolArea::SmSymbolArea()
{

}

SmSymbolArea::~SmSymbolArea()
{
	KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);

	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmSymbolArea::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 15, 2);

	int colWidth[2] = { 80, 103 };

	int width_sum = 0;
	for (int i = 0; i < 2; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	width_sum -= colWidth[1];
	_Grid->SetColWidth(1, rect.Width() - width_sum);

	_Grid->AddMergeCell(0, 0, 1, 2);

	_Grid->SetRowHeight(0, 40);
	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("�ɺ� �̸�");
		_HeaderTitles.push_back("�ð�");
		_HeaderTitles.push_back("����");
		_HeaderTitles.push_back("����");
		_HeaderTitles.push_back("����");
		_HeaderTitles.push_back("���ϴ��");
		_HeaderTitles.push_back("�Ѱŷ���");
		_HeaderTitles.push_back("ƽ��ġ");
		_HeaderTitles.push_back("ƽũ��");
		_HeaderTitles.push_back("�ŷ���");
		_HeaderTitles.push_back("������");
		_HeaderTitles.push_back("����۽ð�");
		_HeaderTitles.push_back("������ð�");
		_HeaderTitles.push_back("���ű�");
		_HeaderTitles.push_back("��ȭ");

		_Grid->SetRowHeaderTitles(_HeaderTitles);
	}

	mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&SmSymbolArea::OnQuoteEvent, this, _1));
	SetTimer(1, 40, NULL);
}

void SmSymbolArea::OnPaint()
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
	//_Grid->DrawVerticalHeader(m_pGM, _HeaderTitles, 0, 1);
	_Grid->DrawBorder(m_pGM, rect);

	m_pGM->EndDraw();
}

void SmSymbolArea::Symbol(std::shared_ptr<DarkHorse::SmSymbol> val)
{
	_Symbol = val;
	UpdateSymbolInfo();
}

void SmSymbolArea::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	std::shared_ptr<SmCell> cell = _Grid->FindCell(0, 0);
	if (cell) cell->Text(_Symbol->SymbolNameKr());
	cell = _Grid->FindCell(1, 1);
	
	std::string	value_string = std::format("{0}", _Symbol->Qoute.open);
	if (_Symbol->decimal() > 0 && value_string.length() > (size_t)_Symbol->decimal())
		value_string.insert(value_string.length() - _Symbol->decimal(), 1, '.');
	
	if (cell) cell->Text(value_string);

	cell = _Grid->FindCell(2, 1);
	value_string = std::format("{0}", _Symbol->Qoute.high);
	if (_Symbol->decimal() > 0 && value_string.length() > (size_t)_Symbol->decimal())
		value_string.insert(value_string.length() - _Symbol->decimal(), 1, '.');
	if (cell) cell->Text(value_string);

	cell = _Grid->FindCell(3, 1);
	value_string = std::format("{0}", _Symbol->Qoute.low);
	if (_Symbol->decimal() > 0 && value_string.length() > (size_t)_Symbol->decimal())
		value_string.insert(value_string.length() - _Symbol->decimal(), 1, '.');
	if (cell) cell->Text(value_string);

	cell = _Grid->FindCell(4, 1);
	value_string = std::format("{0}", _Symbol->Qoute.close);
	if (_Symbol->decimal() > 0 && value_string.length() > (size_t)_Symbol->decimal())
		value_string.insert(value_string.length() - _Symbol->decimal(), 1, '.');
	if (cell) cell->Text(value_string);

	cell = _Grid->FindCell(5, 1);
	if (cell) cell->Text(_Symbol->PreDayRate());

	cell = _Grid->FindCell(6, 1);
	if (cell) cell->Text(std::to_string(_Symbol->TotalVolume()));

	std::string value_precision;
	value_precision.append("{0:.");
	value_precision.append(std::to_string(_Symbol->decimal()));
	value_precision.append("f}");

	cell = _Grid->FindCell(7, 1);
	std::string value = std::format("{0:.2f}", (_Symbol->TickValue()));
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(8, 1);
	value = std::format("{0:.2f}", (_Symbol->TickSize()));
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(9, 1);
	if (cell) cell->Text(_Symbol->Exchange());

	cell = _Grid->FindCell(10, 1);

	std::string expire_date = _Symbol->ExpireDate();
	if (expire_date.length() > 0) {
		expire_date.insert(expire_date.length() - 2, " / ");
		expire_date.insert(expire_date.length() - 7, " / ");
	}

	if (cell) cell->Text(expire_date);

	cell = _Grid->FindCell(11, 1);

	std::string start_time = _Symbol->StartTime();
	if (start_time.length() > 0) {
		start_time.insert(start_time.length() - 2, " : ");
		start_time.insert(start_time.length() - 7, " : ");
	}

	if (cell) cell->Text(start_time);

	cell = _Grid->FindCell(12, 1);

	std::string end_time = _Symbol->EndTime();
	if (end_time.length() > 0) {
		end_time.insert(end_time.length() - 2, " : ");
		end_time.insert(end_time.length() - 7, " : ");
	}

	if (cell) cell->Text(end_time);

	cell = _Grid->FindCell(13, 1);
	if (cell) cell->Text(_Symbol->Deposit());

	cell = _Grid->FindCell(14, 1);
	if (cell) cell->Text(_Symbol->Currency());


	Invalidate();
}

void SmSymbolArea::OnQuoteEvent(const std::string& symbol_code)
{
	if (!_Symbol) return;
	if (_Symbol->SymbolCode() != symbol_code) return;

	_EnableQuoteShow = true;
}

void SmSymbolArea::CreateResource()
{
	_Resource.OrderStroke.SetStartCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_ROUND);
	_Resource.OrderStroke.SetEndCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_TRIANGLE);
	CBCGPTextFormat fmt3(_T("����"), globalUtils.ScaleByDPI(30.0f));

	fmt3.SetFontSize(12);
	fmt3.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt3.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt3.SetWordWrap();
	fmt3.SetClipText();

	_Resource.TextFormat = fmt3;
}

void SmSymbolArea::InitHeader()
{
	_HeaderTitles.push_back("�ɺ� �̸�");
	_HeaderTitles.push_back("�ð�");
	_HeaderTitles.push_back("����");
	_HeaderTitles.push_back("����");
	_HeaderTitles.push_back("����");
	_HeaderTitles.push_back("���ϴ��");
	_HeaderTitles.push_back("�Ѱŷ���");
	_HeaderTitles.push_back("ƽ��ġ");
	_HeaderTitles.push_back("ƽũ��");
	_HeaderTitles.push_back("�ŷ���");
	_HeaderTitles.push_back("������");
}



void SmSymbolArea::OnTimer(UINT_PTR nIDEvent)
{
	if (!_Symbol) return;
	bool needDraw = false;
	if (_EnableQuoteShow) {
		UpdateSymbolInfo();
		_EnableQuoteShow = false;
		needDraw = true;
	}

	if (needDraw) Invalidate();

	CBCGPStatic::OnTimer(nIDEvent);
}