
#include "stdafx.h"
#include "DmFutureView.h"

#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmProduct.h"
#include "../Symbol/SmProductYearMonth.h"
#include "../SmGrid/SmCell.h"
#include "../Account/SmAccount.h"

#include "../Global/SmTotalManager.h"
#include "../Event/SmCallbackManager.h"
#include "../Symbol/SmSymbolManager.h"
#include <format>

#include <functional>
#include "../Fund/SmFund.h"
#include "../Event/EventHub.h"

#include "../Controller/QuoteControl.h"
#include "../ViewModel/VmQuote.h"
#include "../Util/SmUtil.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Util/IdGenerator.h"
#include "../Controller/SymbolPositionControl.h"
#include "../Log/MyLogger.h"
#include "../Order/OrderProcess/TotalOrderManager.h"
#include "../Order/OrderProcess/AccountOrderManager.h"
#include "../Order/OrderProcess/SymbolOrderManager.h"
#include "../Order/Order.h"

#include "../Position/TotalPositionManager.h"
#include "../Position/AccountPositionManager.h"
#include "../Position/Position.h"
#include "../Log/MyLogger.h"

using namespace std;
using namespace std::placeholders;

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(DmFutureView, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

DmFutureView::DmFutureView()
	: id_(IdGenerator::get_id())
{
	quote_control_ = std::make_shared<DarkHorse::QuoteControl>();
	quote_control_->symbol_type(SymbolType::Domestic);
	quote_control_->set_event_handler(std::bind(&DmFutureView::on_update_quote, this));

	position_control_ = std::make_shared<DarkHorse::SymbolPositionControl>();
	position_control_->set_vm_fund_event_handler(std::bind(&DmFutureView::on_update_position_vm_future, this, _1));

	CString strLog;
	strLog.Format("DmFutureView futur_view id = [%d], position_control_id = [%d]", id_, position_control_->get_id());
	//LOGINFO(CMyLogger::getInstance(), (const char*)strLog);

	mainApp.event_hub()->subscribe_expected_event_handler
	(
		id_,
		std::bind(&DmFutureView::update_expected, this, std::placeholders::_1)
	);

	mainApp.event_hub()->subscribe_order_event_handler
	(
		id_,
		std::bind(&DmFutureView::update_order, this, std::placeholders::_1, std::placeholders::_2)
	);
}

DmFutureView::~DmFutureView()
{
	mainApp.event_hub()->unsubscribe_order_event_handler(id_);
	mainApp.event_hub()->unsubscribe_expected_event_handler(id_);
	//KillTimer(1);

	if (m_pGM != NULL)
		delete m_pGM;
}

std::string DmFutureView::get_position_text(const DarkHorse::VmFuture& future_info)
{
	if (future_info.position != 0)
		return std::to_string(future_info.position);
	else if (future_info.accepted_count > 0 ||
		future_info.ordered)
		return "0";
	else return "";
}

void DmFutureView::get_future_info(DarkHorse::VmFuture& future_info)
{
	get_init_accepted_order_count(future_info);
	if (order_type_ == OrderType::None) return;
	if (order_type_ == OrderType::MainAccount) {
		VmPosition position;
		mainApp.total_position_manager()->get_position_from_parent_account(future_info.account_no, future_info.symbol_code, position);
		future_info.position = position.open_quantity;
	}
	else if (order_type_ == OrderType::SubAccount) {
		VmPosition position;
		mainApp.total_position_manager()->get_position_from_account(future_info.account_no, future_info.symbol_code, position);
		future_info.position = position.open_quantity;
	}
	else if (order_type_ == OrderType::Fund) {
		VmPosition position;
		mainApp.total_position_manager()->get_position_from_fund(future_info.fund_name, future_info.symbol_code, position);
		future_info.position = position.open_quantity;
	}
}

void DmFutureView::get_init_accepted_order_count(DarkHorse::VmFuture& future_info)
{
	if (order_type_ == OrderType::None) return;
	if (order_type_ == OrderType::MainAccount) {
		if (!_Account) return;
		future_info.account_no = _Account->No();
		future_info.fund_name = "";
		auto init_and_count = mainApp.total_order_manager()->get_init_and_acpt_order_count_from_parent_account(future_info.account_no, future_info.symbol_code);
		future_info.ordered = init_and_count.first;
		future_info.accepted_count = init_and_count.second;
	}
	else if (order_type_ == OrderType::SubAccount) {
		if (!_Account) return;
		future_info.account_no = _Account->No();
		future_info.fund_name = "";
		auto init_and_count = mainApp.total_order_manager()->get_init_and_acpt_order_count_from_account(future_info.account_no, future_info.symbol_code);
		future_info.ordered = init_and_count.first;
		future_info.accepted_count = init_and_count.second;
	}
	else if (order_type_ == OrderType::Fund) {
		if (!_Fund) return;
		future_info.fund_name = _Fund->Name();
		future_info.account_no = "";
		auto init_and_count = mainApp.total_order_manager()->get_init_and_acpt_order_count_from_fund(future_info.fund_name, future_info.symbol_code);
		future_info.ordered = init_and_count.first;
		future_info.accepted_count = init_and_count.second;
	}
}void DmFutureView::set_position(DarkHorse::VmFuture& future_info)
{
	if (!_Account || !future_info.symbol_p) return;

	auto account_position_manager = mainApp.total_position_manager()->get_account_position_manager(_Account->No());

	auto position = account_position_manager->find_position(future_info.symbol_p->SymbolCode());
	if (!position) return;
	future_info.position = position->open_quantity;
}

void DmFutureView::update_position()
{
	if (!position_control_) return;

	const VmPosition& position = position_control_->get_position();

	auto found = symbol_row_index_map_.find(position.symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.position = position.open_quantity;
	show_value(found->second, 2, future_info);
}

void DmFutureView::on_update_position()
{
	if (!position_control_) return;

	const VmPosition& position = position_control_->get_position();

	auto found = symbol_row_index_map_.find(position.symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.position = position.open_quantity;
	show_value(found->second, 2, future_info);

	enable_show_ = true;
}

void DmFutureView::on_update_quote()
{
	enable_show_ = true;
}

void DmFutureView::update_quote()
{
	const VmQuote quote = quote_control_->get_quote();
	CString msg;
	msg.Format("DmFutureView::update_quote ::  close : %d\n", quote.close);
	//TRACE(msg);
	if (view_mode_ != ViewMode::VM_Close) return;
	update_close(quote);
}

void DmFutureView::set_view_mode(ViewMode view_mode)
{
	view_mode_ = view_mode;
	show_values();
	Invalidate();
}

void DmFutureView::on_update_position_vm_future(const VmPosition& position)
{
	if (!position_control_) return;
	if (position_control_ && position_control_->Position_type() != position.position_type) return;
	auto found = symbol_row_index_map_.find(position.symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.position = position.open_quantity;
	show_value(found->second, 2, future_info);
}

void DmFutureView::update_order(order_p order, OrderEvent order_event)
{
	auto found = symbol_row_index_map_.find(order->symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	get_init_accepted_order_count(future_info);
	show_value(found->second, 2, future_info);
	enable_show_ = true;
}

void DmFutureView::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	_Account = val;
	position_control_->set_account(_Account);

	if (_Account->is_subaccount())
		order_type_ = OrderType::SubAccount;
	else
		order_type_ = OrderType::MainAccount;

	for (auto& future_info : symbol_vec_) {
		get_future_info(future_info);
		if (!future_info.symbol_p) continue;
		auto found = symbol_row_index_map_.find(future_info.symbol_p->SymbolCode());
		if (found == symbol_row_index_map_.end()) continue;
		show_value(found->second, 2, future_info);
	}
	
	enable_show_ = true;
}

void DmFutureView::OnLButtonDown(UINT nFlags, CPoint point)
{
	auto cell_pos = _Grid->FindRowCol(point.x, point.y);
	if (cell_pos.first < 0 || cell_pos.first > 4) return;
	mainApp.event_hub()->process_symbol_event(order_window_id_, symbol_vec_[cell_pos.first].symbol_p);

	CBCGPStatic::OnLButtonDown(nFlags, point);
}

void DmFutureView::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	CreateResource();
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 5, 3);
	int colWidth[3] = { 60, 55, 41 };
	int width_sum = 0;
	for (int i = 0; i < 3; i++) {
		_Grid->SetColWidth(i, colWidth[i]);
		width_sum += colWidth[i];
	}
	width_sum -= colWidth[2];
	_Grid->SetColWidth(2, rect.Width() - width_sum);

	_Grid->MakeRowHeightMap();
	_Grid->MakeColWidthMap();
	_Grid->RecalRowCount(rect.Height(), true);

	_Grid->CreateGrids();

	_Grid->HeaderMode(SmHeaderMode::None);

	
	for (int i = 0; i < 5; i++) {
		std::shared_ptr<SmCell> cell = _Grid->FindCell(i, 0);
		if (cell) cell->CellType(CT_FUTURE);
	}
	

	SetTimer(1, 40, NULL);
}

void DmFutureView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rect;
	GetClientRect(rect);

	if (m_pGM == NULL) return;

	m_pGM->BindDC(pDC, rect);

	if (!m_pGM->BeginDraw()) return;

	m_pGM->FillRectangle(rect, _Resource.GridNormalBrush);
	rect.right -= 1;
	rect.bottom -= 1;

	_Grid->DrawGrid(m_pGM, rect);
	_Grid->draw_cells(m_pGM, rect, false, true);
	//_Grid->DrawVerticalHeader(m_pGM, _HeaderTitles, 0);
	_Grid->DrawBorder(m_pGM, rect);

	m_pGM->EndDraw();
}

void DmFutureView::UpdateSymbolInfo()
{
	if (!_Symbol) return;

	std::shared_ptr<SmCell> cell = _Grid->FindCell(1, 0);
	if (cell) cell->Text(_Symbol->SymbolCode());
	cell = _Grid->FindCell(1, 4);
	if (cell) cell->Text(std::to_string(_Symbol->Qoute.close));

	Invalidate();
}

void DmFutureView::UpdateAssetInfo()
{
	_Mode == 0 ? UpdateAccountAssetInfo() : UpdateFundAssetInfo();
}

void DmFutureView::OnQuoteEvent(const std::string& symbol_code)
{
	enable_show_ = true;
}

void DmFutureView::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	
}

void DmFutureView::update_expected(std::shared_ptr<SmQuote> quote)
{
	if (view_mode_ != ViewMode::VM_Expected) return;

	auto found = symbol_row_index_map_.find(quote->symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.expected = quote->expected;
	show_value(found->second, 2, future_info);
	enable_show_ = true;
}

void DmFutureView::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	_Fund = val;
	order_type_ = OrderType::Fund;
	position_control_->set_fund(_Fund);
	for (auto& future_info : symbol_vec_) {
		get_future_info(future_info);
		if (!future_info.symbol_p) continue;
		auto found = symbol_row_index_map_.find(future_info.symbol_p->SymbolCode());
		if (found == symbol_row_index_map_.end()) continue;
		show_value(found->second, 2, future_info);
	}

	enable_show_ = true;
}

void DmFutureView::update_close(const DarkHorse::VmQuote& quote)
{
	auto found = symbol_row_index_map_.find(quote.symbol_code);
	if (found == symbol_row_index_map_.end()) return;
	DarkHorse::VmFuture& future_info = symbol_vec_[found->second];
	future_info.close = quote.close;
	show_value(found->second, 2, future_info);
}

void DmFutureView::show_values()
{
	for (int i = 0; i < _Grid->RowCount(); i++) {
		const VmFuture& future_info = symbol_vec_[i];
		show_value(i, 2, future_info);
	}
}

void DmFutureView::show_value(const int row, const int col, const DarkHorse::VmFuture& future_info)
{
	auto cell = _Grid->FindCell(row, col);
	if (!cell) return;

	std::string value;
	if (view_mode_ == ViewMode::VM_Close) {
		value = std::to_string(future_info.close);
		SmUtil::insert_decimal(value, future_info.decimal);
	}
	else if (view_mode_ == ViewMode::VM_Expected) {
		value = std::to_string(future_info.expected);
		SmUtil::insert_decimal(value, future_info.decimal);
	}
	else {
		value = get_position_text(future_info);
	}
	set_background_color(cell, future_info);
	cell->Text(value);
}

void DmFutureView::set_background_color(std::shared_ptr<DarkHorse::SmCell> cell, const DarkHorse::VmFuture& future_info)
{
	if (future_info.accepted_count > 0)
		cell->CellType(CT_OE);
	else if (future_info.position != 0)
		cell->CellType(CT_PE);
	else if (future_info.ordered)
		cell->CellType(CT_EE);
	else
		cell->CellType(CT_DEFAULT);
}

void DmFutureView::register_symbol_to_server(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;
	mainApp.SymMgr()->RegisterSymbolToServer(symbol->SymbolCode(), true);
}

void DmFutureView::register_symbols()
{
	if (registered_) return;

	for (auto& symbol : symbol_vec_) {
		register_symbol_to_server(symbol.symbol_p);
	}
	registered_ = true;
}

void DmFutureView::init_dm_future()
{
	const std::vector<DmFuture>& future_vec = mainApp.SymMgr()->get_dm_future_vec();
	for (size_t i = 0; i < future_vec.size(); i++) {
		auto cell = _Grid->FindCell(i, 0);
		if (cell) {
			const std::map<std::string, std::shared_ptr<SmProductYearMonth>>& year_month_map = future_vec[i].product->get_yearmonth_map();
			std::shared_ptr<SmProductYearMonth> year_month = year_month_map.begin()->second;
			auto symbol = year_month->get_first_symbol();
			if (symbol) {
				auto quote = mainApp.QuoteMgr()->get_quote(symbol->SymbolCode());
				VmFuture future_info;
				future_info.decimal = symbol->decimal();
				future_info.close = quote->close;
				future_info.expected = quote->expected;
				future_info.ordered = false;
				future_info.position = 0;
				future_info.symbol_id = symbol->Id();
				future_info.symbol_p = symbol;
				future_info.symbol_code = symbol->SymbolCode();
				get_future_info(future_info);
				symbol_vec_.push_back(future_info);
				symbol_row_index_map_[symbol->SymbolCode()] = i;
			}
		}
		std::string value = future_vec[i].future_name;
		if (cell) cell->Text(value);
		cell = _Grid->FindCell(i, 1);
		value = future_vec[i].product_code;
		if (cell) cell->Text(value);
	}
	register_symbols();
	show_values();
}

void DmFutureView::UpdateAccountAssetInfo()
{
	if (!_Account) return;

	auto cell = _Grid->FindCell(0, 1);
	std::string value;
	value = std::format("{0:.2f}", _Account->Asset.OpenProfitLoss);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(1, 1);
	value = std::format("{0:.2f}", _Account->Asset.TradeProfitLoss);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(2, 1);
	value = std::format("{0:.2f}", _Account->Asset.Fee);
	if (cell) cell->Text(value);
	cell = _Grid->FindCell(3, 1);
	const double pure_profit = _Account->Asset.OpenProfitLoss + _Account->Asset.TradeProfitLoss - abs(_Account->Asset.Fee);
	value = std::format("{0:.2f}", pure_profit);
	if (cell) cell->Text(value);
}

void DmFutureView::UpdateFundAssetInfo()
{
	if (!_Fund) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();

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

void DmFutureView::CreateResource()
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

void DmFutureView::InitHeader()
{
	_HeaderTitles.push_back("평가손익");
	_HeaderTitles.push_back("실현손익");
	_HeaderTitles.push_back("수수료");
	_HeaderTitles.push_back("총손익");
}



void DmFutureView::OnTimer(UINT_PTR nIDEvent)
{
	bool needDraw = false;
	if (enable_show_) {
		if (view_mode_ == ViewMode::VM_Close) {
			update_quote();
			update_position();
		}
		else
			update_position();
		
		enable_show_ = false;
		needDraw = true;
	}

	if (needDraw) Invalidate();

	CBCGPStatic::OnTimer(nIDEvent);
}
