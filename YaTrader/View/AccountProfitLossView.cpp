//#include "stdafx.h"
//#include "AccountProfitLossView.h"

#include "stdafx.h"
#include "AccountProfitLossView.h"

#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../SmGrid/SmCell.h"
#include "../Account/SmAccount.h"

#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Controller/AccountProfitLossControl.h"
#include <format>

#include <functional>
#include "../Fund/SmFund.h"
#include "../Util/VtStringUtil.h"

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(AccountProfitLossView, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

AccountProfitLossView::AccountProfitLossView()
{
	account_profit_loss_control_ = std::make_shared<DarkHorse::AccountProfitLossControl>();
	account_profit_loss_control_->set_event_handler(std::bind(&AccountProfitLossView::on_update_account_profit_loss, this));
}

AccountProfitLossView::~AccountProfitLossView()
{
	if (m_pGM != NULL) delete m_pGM;
}

void AccountProfitLossView::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 4, 2);

	int colWidth[2] = { 60, 97 };

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

	_Grid->CreateGrids();
	{
		_HeaderTitles.push_back("평가손익");
		_HeaderTitles.push_back("청산손익");
		_HeaderTitles.push_back("수수료");
		_HeaderTitles.push_back("순손익");

		_Grid->SetRowHeaderTitles(_HeaderTitles);
	}

	SetTimer(1, 40, NULL);
}

void AccountProfitLossView::OnPaint()
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
	_Grid->draw_cells(m_pGM, rect, false, true);
	//_Grid->DrawVerticalHeader(m_pGM, _HeaderTitles, 0);
	_Grid->DrawBorder(m_pGM, rect);

	m_pGM->EndDraw();
}

void AccountProfitLossView::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	fund_ = val;
	if (!account_profit_loss_control_) return;
	account_profit_loss_control_->set_fund(fund_);
	enable_account_profit_loss_show_ = true;
}

void AccountProfitLossView::on_update_account_profit_loss()
{
	enable_account_profit_loss_show_ = true;
}

void AccountProfitLossView::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	account_ = val;

	if (!account_profit_loss_control_) return;
	account_profit_loss_control_->set_account(account_);
	enable_account_profit_loss_show_ = true;
}

void AccountProfitLossView::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	if (cell) cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}



void AccountProfitLossView::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void AccountProfitLossView::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	enable_account_profit_loss_show_ = true;
}

std::string AccountProfitLossView::get_format_price(const double& value)
{
	std::string value_string;
	CString format_price;
	if (account_->Type() == "1")
		format_price.Format("%.2f", value);
	else
		format_price.Format("%.0f", value);
	return std::string(CT2CA(format_price));
}

void AccountProfitLossView::update_account_profit_loss()
{
	if (!account_profit_loss_control_ ) return;
	if (updating_) return;
	updating_ = true;
	std::string format_type("0");
	if (account_) format_type = account_->Type();
	if (fund_) format_type = fund_->fund_type();

	const VmAccountProfitLoss& account_profit_loss = account_profit_loss_control_->get_account_profit_loss();
	const int decimal = format_type == "1" ? 2 : 0;
	auto cell = _Grid->FindCell(0, 1);
	std::string value;
	value = VtStringUtil::get_format_value(account_profit_loss.open_profit_loss, decimal, true);
	if (cell) {
		cell->Text(value);
		if (account_profit_loss.open_profit_loss > 0) {
			cell->CellType(SmCellType::CT_SP_PROFIT);
		}
		else if (account_profit_loss.open_profit_loss < 0) {
			cell->CellType(SmCellType::CT_SP_LOSS);
		}
		else {
			cell->Text("0");
			cell->CellType(SmCellType::CT_DEFAULT);
		}
	}

	cell = _Grid->FindCell(1, 1);
	value = VtStringUtil::get_format_value(account_profit_loss.trade_profit_loss, decimal, true);
	if (cell) {
		cell->Text(value);

		if (account_profit_loss.trade_profit_loss > 0) {
			cell->CellType(SmCellType::CT_SP_PROFIT);
		}
		else if (account_profit_loss.trade_profit_loss < 0) {
			cell->CellType(SmCellType::CT_SP_LOSS);
		}
		else {
			cell->Text("0");
			cell->CellType(SmCellType::CT_DEFAULT);
		}
	}
	cell = _Grid->FindCell(2, 1);
	value = VtStringUtil::get_format_value(account_profit_loss.trade_fee, decimal, true);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(3, 1);
	const double pure_profit = account_profit_loss.open_profit_loss + account_profit_loss.trade_profit_loss - abs(account_profit_loss.trade_fee);
	value = VtStringUtil::get_format_value(pure_profit, decimal, true);
	if (cell) {
		cell->Text(value);

		if (pure_profit > 0) {
			cell->CellType(SmCellType::CT_SP_PROFIT);
		}
		else if (pure_profit < 0) {
			cell->CellType(SmCellType::CT_SP_LOSS);
		}
		else {
			cell->Text("0");
			cell->CellType(SmCellType::CT_DEFAULT);
		}
	}

	updating_ = false;
	enable_account_profit_loss_show_ = true;
}

void AccountProfitLossView::UpdateFundAssetInfo()
{
	if (!fund_) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = fund_->GetAccountVector();

	double open_pl = 0.0, settled_pl = 0.0, fee = 0.0, pure_pl = 0.0;
	for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
		open_pl += (*it)->Asset.OpenProfitLoss;
		settled_pl += (*it)->Asset.TradeProfitLoss;
		fee += (*it)->Asset.Fee;
		pure_pl = open_pl + settled_pl - abs(fee);
	}

	auto cell = _Grid->FindCell(0, 1);
	std::string value;
	value = std::format("{0:.2f}", open_pl);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(1, 1);
	value = std::format("{0:.2f}", settled_pl);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(2, 1);
	value = std::format("{0:.2f}", fee);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(3, 1);
	value = std::format("{0:.2f}", pure_pl);
	if (cell) cell->Text(value);
}

void AccountProfitLossView::CreateResource()
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

void AccountProfitLossView::InitHeader()
{
	_HeaderTitles.push_back("평가손익");
	_HeaderTitles.push_back("실현손익");
	_HeaderTitles.push_back("수수료");
	_HeaderTitles.push_back("총손익");
}



void AccountProfitLossView::OnTimer(UINT_PTR nIDEvent)
{
	bool needDraw = false;

	if (enable_account_profit_loss_show_) {
		update_account_profit_loss();
		enable_account_profit_loss_show_ = false;
		needDraw = true;
	}
	if (needDraw) Invalidate();

	CBCGPStatic::OnTimer(nIDEvent);
}

void AccountProfitLossView::OnLButtonDown(UINT nFlags, CPoint point)
{

}
