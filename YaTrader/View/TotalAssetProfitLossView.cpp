#include "stdafx.h"
#include "TotalAssetProfitLossView.h"
#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../SmGrid/SmCell.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccountManager.h"

#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Fund/SmFund.h"
#include "../Controller/AccountAssetControl.h"
#include "../Controller/AccountProfitLossControl.h"
#include "../Util/VtStringUtil.h"
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(TotalAssetProfitLossView, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

TotalAssetProfitLossView::TotalAssetProfitLossView()
{
	account_profit_loss_control_ = std::make_shared<DarkHorse::AccountProfitLossControl>();
	account_profit_loss_control_->set_event_handler(std::bind(&TotalAssetProfitLossView::on_update_account_profit_loss, this));

	asset_control_ = std::make_shared<DarkHorse::AccountAssetControl>();
	asset_control_->set_event_handler(std::bind(&TotalAssetProfitLossView::on_update_account_profit_loss, this));
}

TotalAssetProfitLossView::~TotalAssetProfitLossView()
{
	if (m_pGM != NULL) delete m_pGM;
}

void TotalAssetProfitLossView::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	//InitHeader();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 5, 4);

	int colWidth[4] = { 80, 80, 80, 80 };

	int width_sum = 0;
	for (int i = 0; i < 4; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	//width_sum -= colWidth[1];
	//_Grid->SetColWidth(1, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();

	auto cell = _Grid->FindCell(0, 0);
	if (cell) { cell->CellType(CT_HEADER); cell->Text("예탁총액"); }

	cell = _Grid->FindCell(1, 0);
	if (cell) { cell->CellType(CT_HEADER); cell->Text("평가손익"); }

	cell = _Grid->FindCell(2, 0);
	if (cell) { cell->CellType(CT_HEADER); cell->Text("실현손익"); }

	cell = _Grid->FindCell(3, 0);
	if (cell) { cell->CellType(CT_HEADER); cell->Text("위탁증거금"); }

	cell = _Grid->FindCell(4, 0);
	if (cell) { cell->CellType(CT_HEADER); cell->Text("유지증거금"); }


	cell = _Grid->FindCell(0, 2);
	if (cell) { cell->CellType(CT_HEADER); cell->Text("평가예탁총액"); }

	cell = _Grid->FindCell(1, 2);
	if (cell) { cell->CellType(CT_HEADER); cell->Text("수수료"); }

	cell = _Grid->FindCell(2, 2);
	if (cell) { cell->CellType(CT_HEADER); cell->Text("순손익"); }

	cell = _Grid->FindCell(3, 2);
	if (cell) { cell->CellType(CT_HEADER); cell->Text("주문가능금액"); }

	cell = _Grid->FindCell(4, 2);
	if (cell) { cell->CellType(CT_HEADER); cell->Text("추가증거금"); }


	//mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&SmTotalAssetArea::OnQuoteEvent, this, _1));
	//mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmTotalAssetArea::OnOrderEvent, this, _1, _2));

	SetTimer(1, 40, NULL);
}

void TotalAssetProfitLossView::OnPaint()
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

void TotalAssetProfitLossView::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	fund_ = val;

	if (!account_profit_loss_control_) return;
	account_profit_loss_control_->set_fund(fund_);
	asset_control_->set_fund(fund_);
	enable_account_profit_loss_show_ = true;
}

void TotalAssetProfitLossView::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	account_ = val;

	if (!account_profit_loss_control_) return;
	account_profit_loss_control_->set_account(account_);
	asset_control_->set_account(account_);
	enable_account_profit_loss_show_ = true;
}

void TotalAssetProfitLossView::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	if (cell) cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}

void TotalAssetProfitLossView::SetAssetInfo()
{
	_Mode == 0 ? SetAccountAssetInfo() : SetFundAssetInfo();
}

void TotalAssetProfitLossView::OnQuoteEvent(const std::string& symbol_code)
{
	_EnableQuoteShow = true;
}

void TotalAssetProfitLossView::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	_EnableOrderShow = true;
}

void TotalAssetProfitLossView::on_update_account_profit_loss()
{
	enable_account_profit_loss_show_ = true;
}

void TotalAssetProfitLossView::update_account_profit_loss()
{
	if (!asset_control_ || !account_profit_loss_control_ || !account_) return;

	const VmAsset& asset = asset_control_->get_asset();
	const VmAccountProfitLoss& account_profit_loss = account_profit_loss_control_->get_account_profit_loss();
	auto cell = _Grid->FindCell(0, 1);
	const int decimal = account_->Type() == "1" ? 2 : 0;
	std::string value;
	value = VtStringUtil::get_format_value(asset.entrust_total, decimal, true);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(1, 1);
	value = VtStringUtil::get_format_value(account_profit_loss.open_profit_loss, decimal, true);
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
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(2, 1);
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

	cell = _Grid->FindCell(3, 1);
	value = VtStringUtil::get_format_value(asset.entrust_deposit, decimal, true);
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(4, 1);
	value = VtStringUtil::get_format_value(asset.maintenance_margin, decimal, true);
	if (cell) cell->Text(value);


	cell = _Grid->FindCell(0, 3);
	value = VtStringUtil::get_format_value(asset.open_trust_toal, decimal, true);
	if (cell) cell->Text(value);


	cell = _Grid->FindCell(1, 3);
	value = VtStringUtil::get_format_value(account_profit_loss.trade_fee, decimal, true);
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(2, 3);
	double pure_profit = account_profit_loss.open_profit_loss + account_profit_loss.trade_profit_loss - abs(account_profit_loss.trade_fee);
	value = VtStringUtil::get_format_value(pure_profit, decimal, true);
	if (cell) cell->Text(value);
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

	cell = _Grid->FindCell(3, 3);
	value = VtStringUtil::get_format_value(asset.order_margin, decimal, true);
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(4, 3);
	value = VtStringUtil::get_format_value(asset.additional_margin, decimal, true);
	if (cell) cell->Text(value);
}

void TotalAssetProfitLossView::SetAccountAssetInfo()
{
	if (!account_) return;

	auto cell = _Grid->FindCell(0, 1);
	std::string value;
	value = std::format("{0:.2f}", account_->Asset.Balance);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(1, 1);
	value = std::format("{0:.2f}", account_->Asset.OpenProfitLoss);
	if (account_->Asset.OpenProfitLoss > 0) {
		cell->CellType(SmCellType::CT_SP_PROFIT);
	}
	else if (account_->Asset.OpenProfitLoss < 0) {
		cell->CellType(SmCellType::CT_SP_LOSS);
	}
	else {
		cell->Text("0");
		cell->CellType(SmCellType::CT_DEFAULT);
	}

	if (cell) cell->Text(value);
	cell = _Grid->FindCell(2, 1);
	value = std::format("{0:.2f}", account_->Asset.TradeProfitLoss);
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(3, 1);
	value = std::format("{0:.2f}", 0.0);
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(4, 1);
	value = std::format("{0:.2f}", 0.0);
	if (cell) cell->Text(value);


	cell = _Grid->FindCell(0, 3);
	value = std::format("{0:.2f}", account_->Asset.TradeProfitLoss);
	if (cell) cell->Text(value);


	cell = _Grid->FindCell(1, 3);
	value = std::format("{0:.2f}", account_->Asset.Fee);
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(2, 3);
	double pure_profit = account_->Asset.OpenProfitLoss + account_->Asset.TradeProfitLoss - abs(account_->Asset.Fee);
	value = std::format("{0:.2f}", pure_profit);

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

	if (cell) cell->Text(value);

	cell = _Grid->FindCell(3, 3);
	value = std::format("{0:.2f}", account_->Asset.OrderMargin);
	if (cell) cell->Text(value);

	cell = _Grid->FindCell(4, 3);
	value = std::format("{0:.2f}", account_->Asset.AdditionalMargin);
	if (cell) cell->Text(value);
}

void TotalAssetProfitLossView::SetFundAssetInfo()
{
	if (!fund_) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = fund_->GetAccountVector();

	double balance = 0.0, order_margin = 0.0, open_pl = 0.0, settled_pl = 0.0, fee = 0.0, pure_pl = 0.0;
	for (auto it = account_vec.begin(); it != account_vec.end(); it++) {
		open_pl += (*it)->Asset.OpenProfitLoss;
		settled_pl += (*it)->Asset.TradeProfitLoss;
		fee += (*it)->Asset.Fee;
		pure_pl = open_pl + settled_pl - abs(fee);
		balance += (*it)->Asset.Balance;
		order_margin += (*it)->Asset.OrderMargin;
	}

	auto cell = _Grid->FindCell(0, 1);
	std::string value;
	value = std::format("{0:.2f}", balance);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(1, 1);
	value = std::format("{0:.2f}", open_pl);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(2, 1);
	value = std::format("{0:.2f}", settled_pl);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(3, 1);
	value = std::format("{0:.2f}", fee);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(4, 1);
	value = std::format("{0:.2f}", order_margin);
	if (cell) cell->Text(value);
}

void TotalAssetProfitLossView::CreateResource()
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

void TotalAssetProfitLossView::InitHeader()
{
	_HeaderTitles.push_back("예탁금잔액");
	_HeaderTitles.push_back("평가손익");
	_HeaderTitles.push_back("청산손익");
	_HeaderTitles.push_back("수수료");
	_HeaderTitles.push_back("주문가능금액");
	_HeaderTitles.push_back("통화코드");
}



void TotalAssetProfitLossView::OnTimer(UINT_PTR nIDEvent)
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
