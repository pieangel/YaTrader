//#include "stdafx.h"
//#include "SymbolOrderView.h"

#include "stdafx.h"
#include "SymbolOrderView.h"
#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmOrderGridConst.h"
#include <math.h>
#include "../Log/MyLogger.h"
#include "../SmGrid/SmCell.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Order/SmStopOrderManager.h"
#include "../SmGrid/SmCellType.h"
#include "../Order/OrderUi/DmAccountOrderCenterWindow.h"
#include <windows.h>
#include "../Global/SmTotalManager.h"
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "../Account/SmAccount.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Order/SmOrder.h"
#include "../Order/SmOrderViewer.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/OrderUi/DmAccountOrderWindow.h"
#include "../Util/SmNumberFunc.h"
#include "../Event/SmCallbackManager.h"
#include "../Order/SmOrderRequest.h"
#include "../Quote/SmQuoteManager.h"
#include "../Quote/SmQuote.h"
#include "../Hoga/SmHoga.h"
#include "../Hoga/SmHogaManager.h"
#include "../Controller/QuoteControl.h"
#include "../Controller/HogaControl.h"
#include "../Controller/ProductControl.h"
#include "../Controller/OrderControl.h"
#include "../Controller/SubOrderControl.h"
#include "../Controller/SymbolPositionControl.h"
#include "../ViewModel/VmQuote.h"
#include "../ViewModel/VmHoga.h"
#include "../ViewModel/VmProduct.h"
#include "../Event/EventHub.h"
#include "../Util/SmUtil.h"
#include "../Util/IdGenerator.h"
#include "../Order/OrderRequest/OrderRequestManager.h"
#include "../Order/OrderRequest/OrderRequest.h"
#include "../Controller/StopOrderControl.h"
#include "../Symbol/MarketDefine.h"
#include "../Position/TotalPositionManager.h"
#include "../Position/Position.h"
#include "../Order/OrderProcess/TotalOrderManager.h"
#include "../Order/OrderProcess/AccountOrderManager.h"
#include "../Order/OrderProcess/SymbolOrderManager.h"
#include "../Event/EventHub.h"
#include "../Account/SmAccountManager.h"
#include "../Fund/SmFund.h"

#include <sstream>
#include <format>
#include <functional>

using namespace std;
using namespace std::placeholders;

constexpr int Round(double x) { return static_cast<int>(x + 0.5f); }

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SymbolOrderView, CBCGPStatic)
	//{{AFX_MSG_MAP(CBCGPTextPreviewCtrl)
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_SYMBOL_MASTER_RECEIVED, &SymbolOrderView::OnWmSymbolMasterReceived)
	ON_MESSAGE(WM_QUOTE_RECEIVED, &SymbolOrderView::OnWmQuoteReceived)
	ON_MESSAGE(WM_HOGA_RECEIVED, &SymbolOrderView::OnWmHogaReceived)
	ON_MESSAGE(WM_ORDER_CHANGED, &SymbolOrderView::OnUmOrderChanged)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL SymbolOrderView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (_FixedMode) return TRUE;

	// TODO: Add your message handler code here and/or call default
	int distance = zDelta / 60;
	if (abs(zDelta) > 120)
		distance = zDelta / 120;
	else
		distance = zDelta / 40;
	CString msg;
	msg.Format("OnMouseWheel delta = %d\n", zDelta);
	//TRACE(msg);

	ClearOldQuote();
	ClearOldHoga();
	ClearOrders();
	ClearStopOrders();
	clear_buy_stop_order();
	clear_sell_stop_order();
	clear_fixed_selected_cell();

	increase_close_row(distance);

	SetCenterValues();

	update_quote();
	update_hoga();
	draw_order();
	update_position();

	update_buy_stop_order();
	update_sell_stop_order();

	Invalidate();

	return CBCGPStatic::OnMouseWheel(nFlags, zDelta, pt);
}

void SymbolOrderView::set_stop_as_real_order(bool enable)
{
	if (!buy_stop_order_control_) return;
	if (!sell_stop_order_control_) return;

	buy_stop_order_control_->set_stop_as_real_order(enable);
	sell_stop_order_control_->set_stop_as_real_order(enable);
}

void SymbolOrderView::Clear()
{
	ClearOldQuote();
	ClearOldHoga();
	ClearOrders();

	for (int row = 2; row < _Grid->RowCount() - 1; row++)
		for (int col = 0; col < _Grid->ColCount(); col++) {
			auto cell = _Grid->FindCell(row, col);
			if (cell) cell->Text("");
		}
	Invalidate();
}

SymbolOrderView::SymbolOrderView()
	: id_(IdGenerator::get_id())
{
	_BuyStopOrderMgr = std::make_shared<DarkHorse::SmStopOrderManager>();
	_SellStopOrderMgr = std::make_shared<DarkHorse::SmStopOrderManager>();
	//_BuyOrderViewer = std::make_shared<SmOrderViewer>();
	//_SellOrderViewer = std::make_shared<SmOrderViewer>();
	hoga_control_ = std::make_shared<DarkHorse::HogaControl>();
	hoga_control_->set_event_handler(std::bind(&SymbolOrderView::on_update_hoga, this));

	quote_control_ = std::make_shared<DarkHorse::QuoteControl>();
	quote_control_->set_event_handler(std::bind(&SymbolOrderView::on_update_quote, this));

	order_control_ = std::make_shared<DarkHorse::OrderControl>();
	order_control_->set_event_handler(std::bind(&SymbolOrderView::on_update_order, this));

	position_control_ = std::make_shared<DarkHorse::SymbolPositionControl>();
	position_control_->set_event_handler(std::bind(&SymbolOrderView::on_update_position, this));

	product_control_ = std::make_shared<DarkHorse::ProductControl>();

	buy_stop_order_control_ = std::make_shared<DarkHorse::StopOrderControl>();
	buy_stop_order_control_->set_event_handler(std::bind(&SymbolOrderView::on_update_buy_stop_order, this));
	buy_stop_order_control_->set_order_control_id(id_);
	sell_stop_order_control_ = std::make_shared<DarkHorse::StopOrderControl>();
	sell_stop_order_control_->set_event_handler(std::bind(&SymbolOrderView::on_update_sell_stop_order, this));
	sell_stop_order_control_->set_order_control_id(id_);
	m_pGM = CBCGPGraphicsManager::CreateInstance();
	mainApp.event_hub()->subscribe_symbol_master_event_handler
	(
		id_,
		std::bind(&SymbolOrderView::on_update_symbol_master, this, std::placeholders::_1)
	);

	//mainApp.event_hub()->add_parameter_event(id_, std::bind(&SymbolOrderView::on_paramter_event, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	OrderGridHeaderInfo header_info;
	header_info.index = OrderHeader::SELL_STOP;
	header_info.title = "STOP";
	header_info.width = 40;
	grid_header_vector_.push_back(header_info);

	header_info.index = OrderHeader::SELL_ORDER;
	header_info.title = "주문";
	header_info.width = 55;
	grid_header_vector_.push_back(header_info);

	header_info.index = OrderHeader::SELL_CNT;
	header_info.title = "건수";
	header_info.width = 35;
	grid_header_vector_.push_back(header_info);

	header_info.index = OrderHeader::SELL_QTY;
	header_info.title = "잔량";
	header_info.width = 35;
	grid_header_vector_.push_back(header_info);

	header_info.index = OrderHeader::QUOTE;
	header_info.title = "시세";
	header_info.width = 80;
	grid_header_vector_.push_back(header_info);

	header_info.index = OrderHeader::BUY_QTY;
	header_info.title = "잔량";
	header_info.width = 35;
	grid_header_vector_.push_back(header_info);

	header_info.index = OrderHeader::BUY_CNT;
	header_info.title = "건수";
	header_info.width = 35;
	grid_header_vector_.push_back(header_info);

	header_info.index = OrderHeader::BUY_ORDER;
	header_info.title = "주문";
	header_info.width = 55;
	grid_header_vector_.push_back(header_info);

	header_info.index = OrderHeader::BUY_STOP;
	header_info.title = "STOP";
	header_info.width = 40;
	grid_header_vector_.push_back(header_info);
}

void SymbolOrderView::on_update_symbol_master(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol_ || symbol_->Id() != symbol->Id()) return;
	if (symbol->symbol_type() != symbol_type_) return;
	center_valued_ = false;
	_EnableQuoteShow = true;
	_EnableHogaShow = true;
}



SymbolOrderView::~SymbolOrderView()
{
	//KillTimer(1);
	mainApp.event_hub()->unsubscribe_symbol_master_event_handler( id_ );
	
	if (m_pGM != NULL) delete m_pGM;
}

void SymbolOrderView::on_update_quote()
{
	_EnableQuoteShow = true;
	enable_position_show_ = true;
}
void SymbolOrderView::on_update_hoga()
{
	_EnableHogaShow = true;
	enable_position_show_ = true;
}

void SymbolOrderView::on_update_order()
{
	_EnableOrderShow = true;
}

void SymbolOrderView::on_update_position()
{
	enable_position_show_ = true;
}

void SymbolOrderView::set_order_request_type(DarkHorse::OrderRequestType order_req_type)
{
	order_request_type_ = order_req_type;
}

void SymbolOrderView::update_buy_stop_order()
{
	buy_stop_order_rect_vector_.clear();
	const std::map<int, price_order_request_map_p>& order_req_map = buy_stop_order_control_->get_order_req_map();
	draw_stop_order_by_price(order_req_map, SmPositionType::Buy);
}

void SymbolOrderView::update_sell_stop_order()
{
	sell_stop_order_rect_vector_.clear();
	const std::map<int, price_order_request_map_p>& order_req_map = sell_stop_order_control_->get_order_req_map();
	draw_stop_order_by_price(order_req_map, SmPositionType::Sell);
}

void SymbolOrderView::on_update_buy_stop_order()
{
	enable_buy_stop_order_show_ = true;
}

void SymbolOrderView::on_update_sell_stop_order()
{
	enable_sell_stop_order_show_ = true;
}

void SymbolOrderView::on_paramter_event(const DarkHorse::OrderSetEvent& event, const std::string& event_message, const bool enable)
{
	_Grid->SetAllRowHeight(event.grid_height);
	
	grid_header_vector_[0].width = event.stop_width;
	grid_header_vector_[1].width = event.order_width;
	grid_header_vector_[2].width = event.count_width;
	grid_header_vector_[3].width = event.qty_width;
	grid_header_vector_[4].width = event.quote_width;
	grid_header_vector_[5].width = event.qty_width;
	grid_header_vector_[6].width = event.count_width;
	grid_header_vector_[7].width = event.order_width;
	grid_header_vector_[8].width = event.stop_width;
	CRect rect;
	GetClientRect(rect);
	ResetHeaderWidth(rect.Width());
	//_Grid->CreateGrids();
}

void SymbolOrderView::set_fixed_selected_cell()
{
	if (selected_cell_) {
		selected_cell_->set_show_move_rect(true);
	}
}

void SymbolOrderView::clear_fixed_selected_cell()
{
	if (selected_cell_) {
		selected_cell_->set_show_move_rect(false);
		selected_cell_ = nullptr;
	}
}

void SymbolOrderView::on_order_set_event(const DarkHorse::OrderSetEvent& event, const bool flag)
{
	int i = 0;
	i = i + 0;
}

void SymbolOrderView::update_position()
{
	if (!symbol_ || !position_control_) return;
	const VmPosition& position = position_control_->get_position();

	if (position.open_quantity == 0) return;
	int average_price = Round(position.average_price);
	const int int_tick_size = static_cast<int>(symbol_->TickSize() * pow(10, symbol_->decimal()));
	const int mod_avg_price = average_price % int_tick_size;
	const double half_tick_size = int_tick_size / 2.0;
	if (mod_avg_price > half_tick_size)
		average_price += (int_tick_size - mod_avg_price);
	else
		average_price -= mod_avg_price;

	const int position_price_row = FindRow(average_price);

	std::shared_ptr<SmCell> cell = _Grid->FindCell(position_price_row, DarkHorse::OrderHeader::QUOTE);
	if (position_price_row >= 2 && cell) {
		if (position.open_quantity > 0)
			cell->CellType(SmCellType::CT_POSITION_BUY);
		else if (position.open_quantity < 0)
			cell->CellType(SmCellType::CT_POSITION_SELL);
		cell->set_show_position(true);
		quote_priece_set.insert(std::make_pair(cell->Row(), cell->Col()));
	}
}

void SymbolOrderView::set_filled_condition(DarkHorse::OrderRequestType order_req_type)
{
	if (order_req_type == DarkHorse::OrderRequestType::Domestic)
		fill_condition_ = DarkHorse::SmFilledCondition::Fas;
	else
		fill_condition_ = DarkHorse::SmFilledCondition::Day;
}

std::shared_ptr<DarkHorse::SmCell> SymbolOrderView::draw_cell(const int row, const int col, const int value)
{
	auto cell = _Grid->FindCell(row, col);
	if (!cell) return nullptr;
	cell->Text(std::to_string(value));
	return cell;
}

void SymbolOrderView::draw_order_cell(
	DarkHorse::SmPositionType position, 
	const int price, 
	const int count)
{
	if (price == 0 || count == 0) return;

	int row = FindRow(price);
	if (row < price_start_row_ || row > price_end_row_) return;

	const int col = (position == SmPositionType::Buy) ?
		OrderHeader::BUY_ORDER :
		OrderHeader::SELL_ORDER;
	auto cell = draw_cell(row, col, count);
	if (position == SmPositionType::Buy) {
		if (cell) cell->CellType(CT_ORDER_BUY);
		_OldOrderBuyRowIndex.insert(row);
	}
	else {
		if (cell) cell->CellType(CT_ORDER_SELL);
		_OldOrderSellRowIndex.insert(row);
	}
}

void SymbolOrderView::draw_stop_order_cell(
	const DarkHorse::SmPositionType position,
	price_order_request_map_p price_order_req_map)
{
	if (price_order_req_map->count() == 0) return;
	int row = FindRow(price_order_req_map->get_price());
	if (row < price_start_row_ || row > price_end_row_) return;

	const int col = (position == SmPositionType::Buy) ?
		OrderHeader::BUY_STOP :
		OrderHeader::SELL_STOP;
	auto cell = draw_cell(row, col, price_order_req_map->count());
	if (position == SmPositionType::Buy) {
		old_stop_buy_order_index_.insert(row);
		draw_stop_order_line(cell, position, price_order_req_map->get_order_request_map());
	}
	else {
		old_stop_sell_order_index_.insert(row);
		draw_stop_order_line(cell, position, price_order_req_map->get_order_request_map());
	}
}

void SymbolOrderView::draw_stop_order_line(
	std::shared_ptr<DarkHorse::SmCell> cell,
	const DarkHorse::SmPositionType position,
	const std::map<int, order_request_p>& order_req_map)
{
	for (auto it = order_req_map.begin(); it != order_req_map.end(); it++) {
		if (position == SmPositionType::Buy) {
			std::shared_ptr<SmCell> pOrderCell = _Grid->FindCell(cell->Row() - it->second->cut_slip, DarkHorse::OrderHeader::BUY_ORDER);
			if (pOrderCell && pOrderCell->Row() > 1 && pOrderCell->Row() < price_end_row_) {
				buy_stop_order_rect_vector_.push_back(std::make_pair(cell->GetCellRect(), pOrderCell->GetCellRect()));
			}
		}
		else {
			std::shared_ptr<SmCell> pOrderCell = _Grid->FindCell(cell->Row() + it->second->cut_slip, DarkHorse::OrderHeader::SELL_ORDER);
			if (pOrderCell && pOrderCell->Row() > 1 && pOrderCell->Row() < price_end_row_) {
				sell_stop_order_rect_vector_.push_back(std::make_pair(cell->GetCellRect(), pOrderCell->GetCellRect()));
			}
		}
	}
}

void SymbolOrderView::draw_order_by_price(
	DarkHorse::SubOrderControl& sub_order_control,
	DarkHorse::SmPositionType position)
{
	const std::map<int, std::shared_ptr<PriceOrderMap>>& price_order_map = sub_order_control.get_order_map();
	for (auto it = price_order_map.begin(); it != price_order_map.end(); it++) {
		draw_order_cell(position, it->second->get_price(), it->second->count());
	}
}

void SymbolOrderView::draw_total_order(
	const DarkHorse::SubOrderControl& sub_order_control,
	DarkHorse::SmPositionType position)
{
	const int col = (position == SmPositionType::Buy) ?
		OrderHeader::BUY_ORDER :
		OrderHeader::SELL_ORDER;
	const int row = 1;
	draw_cell(row, col, sub_order_control.total_count());
}


void SymbolOrderView::draw_stop_order_by_price(
	const std::map<int, price_order_request_map_p>& order_req_map,
	const DarkHorse::SmPositionType position)
{
	for (auto it = order_req_map.begin(); it != order_req_map.end(); it++)
		draw_stop_order_cell(position, it->second);
}

void SymbolOrderView::draw_total_stop_order(
	const int count,
	const DarkHorse::SmPositionType position)
{
	const int col = (position == SmPositionType::Buy) ?
		OrderHeader::BUY_STOP :
		OrderHeader::SELL_STOP;
	const int row = 1;
	draw_cell(row, col, count);
}

void SymbolOrderView::draw_order()
{
	if (!order_control_) return;
	if (_MovingOrder) return;

	DarkHorse::SubOrderControl& buy_order_control = order_control_->get_buy_order_control();
	draw_order_by_price(buy_order_control, SmPositionType::Buy);
	draw_total_order(buy_order_control, SmPositionType::Buy);
	DarkHorse::SubOrderControl& sell_order_control = order_control_->get_sell_order_control();
	draw_order_by_price(sell_order_control, SmPositionType::Sell);
	draw_total_order(sell_order_control, SmPositionType::Sell);
}

void SymbolOrderView::update_quote()
{
	if (!quote_control_ || !product_control_) return;
	const VmQuote& quote = quote_control_->get_quote();
	if (!center_valued_) SetCenterValues();
	//SetCenterValues();
	set_quote_color();
	set_quote_value(quote.close, SmCellType::CT_QUOTE_CLOSE);
	set_quote_value(quote.open, SmCellType::CT_QUOTE_OPEN);
	set_quote_value(quote.high, SmCellType::CT_QUOTE_HIGH);
	set_quote_value(quote.low, SmCellType::CT_QUOTE_LOW);
	set_quote_value(quote.pre_day_close, SmCellType::CT_QUOTE_PRECLOSE);
	//update_position();
	//Invalidate();
	//_EnableQuoteShow = true;
}

void SymbolOrderView::update_hoga()
{
	if (!hoga_control_ || price_to_row_.empty()) return;

	const VmHoga hoga = hoga_control_->get_hoga();

	std::shared_ptr<SmCell> pCell = nullptr;

	for (int i = 0; i < 5; i++) {
		int row_index = FindRow(hoga.Ary[i].SellPrice);
		pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::SELL_CNT);
		if (pCell && row_index > 1 && row_index < price_end_row_) {
			pCell->Text(std::to_string(hoga.Ary[i].SellCnt));
			switch (i) {
			case 0: pCell->CellType(SmCellType::CT_SELL_HOGA1); break;
			case 1: pCell->CellType(SmCellType::CT_SELL_HOGA2); break;
			case 2: pCell->CellType(SmCellType::CT_SELL_HOGA3); break;
			case 3: pCell->CellType(SmCellType::CT_SELL_HOGA4); break;
			case 4: pCell->CellType(SmCellType::CT_SELL_HOGA5); break;
			default: break;
			}
			//pCell->CellType(SmCellType::CT_HOGA_SELL);
			_OldHogaSellRowIndex.insert(row_index);
		}
		pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::SELL_QTY);
		if (pCell && row_index > 1 && row_index < price_end_row_) {
			//pCell->CellType(SmCellType::CT_HOGA_SELL);
			switch (i) {
			case 0: pCell->CellType(SmCellType::CT_SELL_HOGA1); break;
			case 1: pCell->CellType(SmCellType::CT_SELL_HOGA2); break;
			case 2: pCell->CellType(SmCellType::CT_SELL_HOGA3); break;
			case 3: pCell->CellType(SmCellType::CT_SELL_HOGA4); break;
			case 4: pCell->CellType(SmCellType::CT_SELL_HOGA5); break;
			default: break;
			}
			pCell->Text(std::to_string(hoga.Ary[i].SellQty));
		}
		row_index = FindRow(hoga.Ary[i].BuyPrice);
		pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::BUY_QTY);
		if (pCell && row_index > 1 && row_index < price_end_row_) {
			//pCell->CellType(SmCellType::CT_HOGA_BUY);
			switch (i) {
			case 0: pCell->CellType(SmCellType::CT_BUY_HOGA1); break;
			case 1: pCell->CellType(SmCellType::CT_BUY_HOGA2); break;
			case 2: pCell->CellType(SmCellType::CT_BUY_HOGA3); break;
			case 3: pCell->CellType(SmCellType::CT_BUY_HOGA4); break;
			case 4: pCell->CellType(SmCellType::CT_BUY_HOGA5); break;
			default: break;
			}
			pCell->Text(std::to_string(hoga.Ary[i].BuyQty));
			_OldHogaBuyRowIndex.insert(row_index);
		}
		pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::BUY_CNT);
		if (pCell && row_index > 1 && row_index < price_end_row_) {
			//pCell->CellType(SmCellType::CT_HOGA_BUY);
			switch (i) {
			case 0: pCell->CellType(SmCellType::CT_BUY_HOGA1); break;
			case 1: pCell->CellType(SmCellType::CT_BUY_HOGA2); break;
			case 2: pCell->CellType(SmCellType::CT_BUY_HOGA3); break;
			case 3: pCell->CellType(SmCellType::CT_BUY_HOGA4); break;
			case 4: pCell->CellType(SmCellType::CT_BUY_HOGA5); break;
			default: break;
			}
			pCell->Text(std::to_string(hoga.Ary[i].BuyCnt));
		}
	}

	pCell = _Grid->FindCell(price_end_row_, DarkHorse::OrderHeader::SELL_CNT);
	if (pCell) pCell->Text(std::to_string(hoga.TotSellCnt));
	pCell = _Grid->FindCell(price_end_row_, DarkHorse::OrderHeader::SELL_QTY);
	if (pCell) pCell->Text(std::to_string(hoga.TotSellQty));
	pCell = _Grid->FindCell(price_end_row_, DarkHorse::OrderHeader::BUY_CNT);
	if (pCell) pCell->Text(std::to_string(hoga.TotBuyCnt));
	pCell = _Grid->FindCell(price_end_row_, DarkHorse::OrderHeader::BUY_QTY);
	if (pCell) pCell->Text(std::to_string(hoga.TotBuyQty));

	const int delta_hoga = hoga.TotBuyQty - hoga.TotSellQty;
	pCell = _Grid->FindCell(price_end_row_, DarkHorse::OrderHeader::QUOTE);
	if (pCell) pCell->Text(std::to_string(delta_hoga));

	_TotalHogaMap.insert(std::make_pair(price_end_row_, DarkHorse::OrderHeader::SELL_CNT));
	_TotalHogaMap.insert(std::make_pair(price_end_row_, DarkHorse::OrderHeader::SELL_QTY));
	_TotalHogaMap.insert(std::make_pair(price_end_row_, DarkHorse::OrderHeader::QUOTE));
	_TotalHogaMap.insert(std::make_pair(price_end_row_, DarkHorse::OrderHeader::BUY_CNT));
	_TotalHogaMap.insert(std::make_pair(price_end_row_, DarkHorse::OrderHeader::BUY_QTY));

	//Invalidate(FALSE);

	//_EnableHogaShow = true;
}

void SymbolOrderView::ClearOldHoga(DarkHorse::Hoga_Type hoga_type) const noexcept
{
	std::shared_ptr<SmCell> pCell = nullptr;
	if (hoga_type == Hoga_Type::SELL) {
		for (auto it = _OldHogaSellRowIndex.begin(); it != _OldHogaSellRowIndex.end(); ++it) {
			const int& row_index = *it;
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::SELL_CNT);
			if (pCell) {
				pCell->Text(""); pCell->CellType(SmCellType::CT_NORMAL);
			}
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::SELL_QTY);
			if (pCell) {
				pCell->Text(""); pCell->CellType(SmCellType::CT_NORMAL);
			}
		}
	}
	else {
		for (auto it = _OldHogaBuyRowIndex.begin(); it != _OldHogaBuyRowIndex.end(); ++it) {
			const int& row_index = *it;
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::BUY_QTY);
			if (pCell) {
				pCell->Text(""); pCell->CellType(SmCellType::CT_NORMAL);
			}
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::BUY_CNT);
			if (pCell) {
				pCell->Text(""); pCell->CellType(SmCellType::CT_NORMAL);
			}
		}
	}
}

void SymbolOrderView::ClearOldHoga()
{
	// 이전 호가를 지운다.
	ClearOldHoga(Hoga_Type::SELL);
	ClearOldHoga(Hoga_Type::BUY);
	_OldHogaSellRowIndex.clear();
	_OldHogaBuyRowIndex.clear();
	std::shared_ptr<SmCell> pCell = nullptr;
	for (auto it = _TotalHogaMap.begin(); it != _TotalHogaMap.end(); ++it) {
		pCell = _Grid->FindCell(it->first, it->second);
		if (pCell) {
			pCell->Text(""); pCell->CellType(SmCellType::CT_NORMAL);
		}
	}
	_TotalHogaMap.clear();
}

void SymbolOrderView::ClearOldQuote()
{
	std::shared_ptr<SmCell> pCell = nullptr;
	for (auto it = quote_priece_set.begin(); it != quote_priece_set.end(); ++it) {
		pCell = _Grid->FindCell(it->first, it->second);
		if (pCell) {
			pCell->CellType(SmCellType::CT_NORMAL);
			pCell->set_show_quote_sign(false);
			pCell->set_show_position(false);
		}
	}
	quote_priece_set.clear();
}

void SymbolOrderView::ClearStopOrders()
{
	std::shared_ptr<SmCell> pCell = nullptr;
	for (auto it = _OldStopSellRowIndex.begin(); it != _OldStopSellRowIndex.end(); ++it) {
		pCell = _Grid->FindCell(*it, DarkHorse::OrderHeader::SELL_STOP);
		if (pCell) {
			pCell->ClearOrderReq();
			pCell->Text("");
		}
	}
	_OldStopSellRowIndex.clear();

	for (auto it = _OldStopBuyRowIndex.begin(); it != _OldStopBuyRowIndex.end(); ++it) {
		pCell = _Grid->FindCell(*it, DarkHorse::OrderHeader::BUY_STOP);
		if (pCell) {
			pCell->ClearOrderReq();
			pCell->Text("");
		}
	}
	_OldStopBuyRowIndex.clear();
}

void SymbolOrderView::ClearOrders()
{
	if (_MovingOrder) return;

	std::shared_ptr<SmCell> pCell = nullptr;
	for (auto it = _OldOrderSellRowIndex.begin(); it != _OldOrderSellRowIndex.end(); ++it) {
		pCell = _Grid->FindCell(*it, DarkHorse::OrderHeader::SELL_ORDER);
		if (pCell) {
			pCell->CellType(CT_ORDER_SELL_BACK);
			pCell->ClearOrder();
			pCell->Text("");
		}
	}
	_OldOrderSellRowIndex.clear();

	for (auto it = _OldOrderBuyRowIndex.begin(); it != _OldOrderBuyRowIndex.end(); ++it) {
		pCell = _Grid->FindCell(*it, DarkHorse::OrderHeader::BUY_ORDER);
		if (pCell) {
			pCell->CellType(CT_ORDER_BUY_BACK);
			pCell->ClearOrder();
			pCell->Text("");
		}
	}
	_OldOrderBuyRowIndex.clear();
}

int SymbolOrderView::FindRow(const int& value) const noexcept
{
	int result = -1;
	auto it = price_to_row_.find(value);
	if (it != price_to_row_.end())
		return it->second;
	else
		return -1;
}

int SymbolOrderView::FindRowFromCenterValue(const int& value)
{
	if (!product_control_) return -1;
	if (!quote_control_) return -1;
	if (price_to_row_.empty() || value == 0) return -1;

	const int int_tick_size = product_control_->get_product().int_tick_size;
	if (int_tick_size == 0) return -1;
	auto it = price_to_row_.find(value);
	if (it != price_to_row_.end()) { // 값이 보이는 범위 안에 있을 때
		return it->second;
	}
	else { // 값이 보이는 범위 밖에 있을 때
		auto itr = price_to_row_.rbegin();
		int big_val = itr->first;
		int big_row = itr->second;


		int delta = big_val - value;
		int delta_row = delta / int_tick_size;

		int target_row = big_row + delta_row;
		return target_row;
	}
}

void SymbolOrderView::set_quote_value(const int value, const DarkHorse::SmCellType cell_type)
{
	const int row = FindRow(value);
	if (row < 2) return;
	std::shared_ptr<SmCell> cell = _Grid->FindCell(row, DarkHorse::OrderHeader::QUOTE);
	if (!cell) return;
	cell->set_show_quote_sign(true);
	const int symbol_decimal = product_control_->get_product().decimal;
	std::string value_string;
	cell->CellType(cell_type);
	value_string = std::format("{0}", value);
	SmUtil::insert_decimal(value_string, symbol_decimal);
	cell->Text(value_string);
	quote_priece_set.insert(std::make_pair(cell->Row(), cell->Col()));
}

void SymbolOrderView::set_quote_cell(const int row, const bool show_mark, const DarkHorse::SmMarkType mark_type)
{
	auto cell = _Grid->FindCell(row, DarkHorse::OrderHeader::QUOTE);
	if (cell) { 
		//cell->ShowMark(show_mark); 
		//cell->MarkType(mark_type); 
		switch (mark_type)
		{
		case MT_BUY: 
			cell->CellType(CT_MARK_BUY);
			break;
		case MT_SELL: 
			cell->CellType(CT_MARK_SELL);
			break;
		case MT_HILO: 
			cell->CellType(CT_MARK_HILO);
			break;
		default: 
			cell->CellType(CT_MARK_NORMAL);
			break;
		}
	}
}

void SymbolOrderView::set_quote_color()
{
	try {
		if (!quote_control_) return;

		const VmQuote quote = quote_control_->get_quote();
		int lowRow = find_row(quote.low);
		int highRow = find_row(quote.high);
		int closeRow = find_row(quote.close);
		int openRow = find_row(quote.open);
		
		if (quote.close > quote.open) { // 양봉
			for (auto it = price_to_row_.rbegin(); it != price_to_row_.rend(); ++it) {
				if (it->second < highRow) set_quote_cell(it->second, true, SmMarkType::MT_NONE);
				else if (it->second < closeRow) set_quote_cell(it->second, true, SmMarkType::MT_HILO);
				else if (it->second <= openRow) set_quote_cell(it->second, true, SmMarkType::MT_BUY);
				else if (it->second < lowRow + 1) set_quote_cell(it->second, true, SmMarkType::MT_HILO);
				else set_quote_cell(it->second, true, SmMarkType::MT_NONE);
			}

		}
		else if (quote.close < quote.open) { // 음봉
			for (auto it = price_to_row_.rbegin(); it != price_to_row_.rend(); ++it) {
				if (it->second < highRow) set_quote_cell(it->second, true, SmMarkType::MT_NONE);
				else if (it->second < openRow) set_quote_cell(it->second, true, SmMarkType::MT_HILO);
				else if (it->second <= closeRow) set_quote_cell(it->second, true, SmMarkType::MT_SELL);
				else if (it->second < lowRow + 1) set_quote_cell(it->second, true, SmMarkType::MT_HILO);
				else set_quote_cell(it->second, true, SmMarkType::MT_NONE);
			}
		}
		else { // 도지
			for (auto it = price_to_row_.rbegin(); it != price_to_row_.rend(); ++it) {
				if (it->second < highRow) set_quote_cell(it->second, true, SmMarkType::MT_NONE);
				else if (it->second < closeRow) set_quote_cell(it->second, true, SmMarkType::MT_HILO);
				else if (it->second <= openRow) set_quote_cell(it->second, true, SmMarkType::MT_HILO);
				else if (it->second < lowRow + 1) set_quote_cell(it->second, true, SmMarkType::MT_HILO);
				else set_quote_cell(it->second, true, SmMarkType::MT_NONE);
			}
		}
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

int SymbolOrderView::FindValue(const int& row) const noexcept
{
	auto it = row_to_price_.find(row);
	if (it == row_to_price_.end()) return -1;
	return it->second;
}

void SymbolOrderView::Refresh()
{
	_EnableOrderShow = true;
	_EnableQuoteShow = true;
	_EnableStopShow = true;
	_EnableHogaShow = true;
	ClearOldQuote();
	ClearOldHoga();
	ClearStopOrders();
	ClearOrders();
	clear_buy_stop_order();
	clear_sell_stop_order();
	update_quote();
	update_hoga();
	draw_order();
	update_buy_stop_order();
	update_sell_stop_order();
	Invalidate(FALSE);
}

void SymbolOrderView::DrawHogaLine(const CRect& rect)
{
	if (!hoga_control_ || price_to_row_.empty()) return;

	const VmHoga hoga = hoga_control_->get_hoga();

	int row_index = FindRow(hoga.Ary[0].SellPrice);
	auto pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::SELL_CNT);
	if (pCell && row_index > 1) 
		m_pGM->DrawLine(
			0, 
			pCell->Y() + pCell->Height() + 1, 
			rect.Width(), 
			pCell->Y() + pCell->Height() + 1, 
			_Resource.DefaultTextBrush,
			2.0f
		);
}

void SymbolOrderView::DrawFixedSelectedCell()
{
	const int row_index = FindRow(_SelectedValue.first);
	auto pCell = _Grid->FindCell(row_index, _SelectedValue.second);
	if (pCell) {
		_Grid->DrawCell(
			pCell, 
			m_pGM, 
			_Resource.SelectedBrush, 
			_Resource.SelectedBrush, 
			_Resource.TextBrush, 
			_Resource.TextFormat, 
			false, true, true);
	}
}

void SymbolOrderView::DrawMovingOrder()
{
	if (_DraggingOrder) {

		auto start_cell = _Grid->FindCellByPos(_StartX, _StartY);
		if (!start_cell) return;
		if (start_cell->Col() == DarkHorse::OrderHeader::BUY_ORDER ||
			start_cell->Col() == DarkHorse::OrderHeader::SELL_ORDER ||
			start_cell->Col() == DarkHorse::OrderHeader::SELL_STOP ||
			start_cell->Col() == DarkHorse::OrderHeader::BUY_STOP) {


			const CBCGPRect rect_start(start_cell->X(), start_cell->Y(), start_cell->X() + start_cell->Width(), start_cell->Y() + start_cell->Height());
			const CBCGPPoint start_point = rect_start.CenterPoint();

			const CBCGPPoint ex_point(_EndX, _EndY);

			const auto it = _Grid->FindRowCol(_EndX, _EndY);
			const auto end_cell = _Grid->FindCell(it.first, it.second);
			CBCGPPoint end_point;
			if (end_cell && end_cell->Col() == start_cell->Col()) {
				const CBCGPRect rect_end(end_cell->X(), end_cell->Y(), end_cell->X() + end_cell->Width(), end_cell->Y() + end_cell->Height());
				end_point = rect_end.CenterPoint();
			}
			else {
				end_point.x = _EndX;
				end_point.y = _EndY;
			}

			DrawArrow(start_point, end_point, 2.0f, 12);
		}
	}
}

void SymbolOrderView::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	//rect.right -= 1;
	rect.bottom -= 1;

	CreateResource();


	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 60, 9);


	set_stop_as_real_order(order_set_.stop_as_real_order);
	SetAllRowHeight(order_set_.grid_height);
	reset_col_widths(order_set_);

	ResetHeaderWidth(rect.Width());
	_Grid->HeaderMode(SmHeaderMode::HeaderColOnly);
	//_Grid->SetOrderHeaderTitles();
	_Grid->MakeRowHeightMap();
	_Grid->RecalRowCount(rect.Height(), true);
	// _MergedCellMap[std::make_pair(1, 2)] = std::make_pair(1, 2);
	//_MergedCellMap[std::make_pair(1, 5)] = std::make_pair(1, 2);
	_Grid->AddMergeCell(1, 2, 1, 2);
	_Grid->AddMergeCell(1, 5, 1, 2);
	_Grid->MakeColWidthMap();

	_Grid->CreateGrids();

	{

		_OrderTableHeader.push_back(grid_header_vector_[DarkHorse::OrderHeader::SELL_STOP].title);
		_OrderTableHeader.push_back(grid_header_vector_[DarkHorse::OrderHeader::SELL_ORDER].title);
		_OrderTableHeader.push_back(grid_header_vector_[DarkHorse::OrderHeader::SELL_CNT].title);
		_OrderTableHeader.push_back(grid_header_vector_[DarkHorse::OrderHeader::SELL_QTY].title);
		_OrderTableHeader.push_back(grid_header_vector_[DarkHorse::OrderHeader::QUOTE].title);
		_OrderTableHeader.push_back(grid_header_vector_[DarkHorse::OrderHeader::BUY_QTY].title);
		_OrderTableHeader.push_back(grid_header_vector_[DarkHorse::OrderHeader::BUY_CNT].title);
		_OrderTableHeader.push_back(grid_header_vector_[DarkHorse::OrderHeader::BUY_ORDER].title);
		_OrderTableHeader.push_back(grid_header_vector_[DarkHorse::OrderHeader::BUY_STOP].title);

		_Grid->SetColHeaderTitles(_OrderTableHeader);
	}

	_Grid->RegisterOrderButtons(_ButtonMap);
	set_order_area();
	index_row_ = get_center_row();

	SetTimer(1, 10, NULL);

	return;
}


int SymbolOrderView::RecalRowCount(const int& height)
{
	CRect rect;

	GetWindowRect(rect);

	//rect.right -= 1;
	rect.bottom -= 1;

	ClearOldQuote();
	ClearOldHoga();
	ClearOrders();
	ClearStopOrders();
	clear_buy_stop_order();
	clear_sell_stop_order();

	_Grid->ReleaseOrderButtons(_ButtonMap);
	const int extra_height = _Grid->RecalRowCount(height, false);
	price_start_row_ = 2;
	price_end_row_ = _Grid->RowCount() - 2;
	_Grid->CreateGrids();
	_Grid->RegisterOrderButtons(_ButtonMap);
	index_row_ = get_center_row();
	SetCenterValues();
	set_order_area();
	update_quote();
	update_hoga();
	draw_order();
	update_buy_stop_order();
	update_sell_stop_order();

	//Invalidate(FALSE);

	DmAccountOrderCenterWindow::DeltaOrderArea = extra_height;

	return extra_height;
}

void SymbolOrderView::fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	if (!val || !position_control_ || !order_control_) return;

	fund_ = val;
	position_type_ = DarkHorse::PositionType::Fund;
	order_type_ = DarkHorse::OrderType::Fund;

	order_control_->set_fund(val);
	position_control_->set_fund(val);

	set_order();
	set_position();
}


void SymbolOrderView::ArrangeCenterValue()
{
	ClearOldQuote();
	ClearOldHoga();
	ClearOrders();
	clear_buy_stop_order();
	clear_sell_stop_order();

	index_row_ = get_center_row();

	SetCenterValues();

	update_quote();
	update_hoga();
	draw_order();
	update_buy_stop_order();
	update_sell_stop_order();
}

void SymbolOrderView::BuyByMarketPrice()
{
	put_order(SmPositionType::Buy, 0, SmPriceType::Market);
}

void SymbolOrderView::SellByMarketPrice()
{
	put_order(SmPositionType::Sell, 0, SmPriceType::Market);
}

void SymbolOrderView::CancelSellStop()
{
	sell_stop_order_control_->clear();
	enable_sell_stop_order_show_ = true;
}

void SymbolOrderView::set_order_area()
{
	for (int i = 1; i < _Grid->RowCount(); i++) {
		auto cell = _Grid->FindCell(i, DarkHorse::OrderHeader::SELL_ORDER);
		if (cell) {
			cell->CellType(SmCellType::CT_ORDER_SELL_BACK);
		}
		cell = _Grid->FindCell(i, DarkHorse::OrderHeader::BUY_ORDER);
		if (cell) {
			cell->CellType(SmCellType::CT_ORDER_BUY_BACK);
		}
	}
}

void SymbolOrderView::reset_window_size()
{
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	GetParent()->ScreenToClient(rcWnd);
	SetWindowPos(nullptr, 0, 0, get_entire_width(), rcWnd.Height(), SWP_NOMOVE | SWP_NOZORDER);
	Invalidate();
}

void SymbolOrderView::trigger_resize_event()
{
	mainApp.event_hub()->trigger_window_resize_event(id_);
}

void SymbolOrderView::CancelSellOrder()
{
	if (account_)
		CancelSellOrder(account_);
	else if (fund_) {
		for (auto& account : fund_->GetAccountVector()) {
			CancelSellOrder(account);
		}	
	}
}

void SymbolOrderView::CancelSellOrder(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account || !symbol_) return;
	auto account_order_manager = mainApp.total_order_manager()->get_account_order_manager(account->No());
	auto symbol_order_manager = account_order_manager->get_symbol_order_manager(symbol_->SymbolCode());
	//const std::map<std::string, order_p>& order_map = symbol_order_manager->get_accepted_order_map();

	std::vector<order_p> order_vec;
	symbol_order_manager->get_accepted_order_vector(order_vec);
	if (order_vec.empty()) return;

	for (auto it = order_vec.begin(); it != order_vec.end(); ++it) {
		const auto& order = *it;
		if (order->position == SmPositionType::Buy) continue;
		auto order_req = OrderRequestManager::make_cancel_order_request(
			order->account_no,
			account->Pwd(),
			order->symbol_code,
			order->order_no,
			order->order_price,
			order->position,
			order->order_amount,
			SmOrderType::Cancel,
			order->price_type,
			fill_condition_);
		SetProfitLossCut(order_req);
		order_req->request_type = order_request_type_;
		order_req->price_type = price_type_;
		mainApp.order_request_manager()->add_order_request(order_req);
	}

	DarkHorse::SubOrderControl& sell_order_control = order_control_->get_sell_order_control();
	sell_order_control.clear();
}

void SymbolOrderView::CancelAllOrder()
{
	CancelBuyStop();
	CancelSellStop();
	CancelSellOrder();
	CancelBuyOrder();
}

void SymbolOrderView::CancelBuyStop()
{
	buy_stop_order_control_->clear();
	enable_buy_stop_order_show_ = true;
}

void SymbolOrderView::CancelBuyOrder()
{
	if (account_) CancelBuyOrder(account_);
	else if (fund_) {
		for (auto& account : fund_->GetAccountVector()) {
			CancelBuyOrder(account);
		}
	}
}

void SymbolOrderView::CancelBuyOrder(std::shared_ptr<DarkHorse::SmAccount> account)
{
	if (!account || !symbol_) return;
	auto account_order_manager = mainApp.total_order_manager()->get_account_order_manager(account->No());
	auto symbol_order_manager = account_order_manager->get_symbol_order_manager(symbol_->SymbolCode());
	//const std::map<std::string, order_p>& order_map = symbol_order_manager->get_accepted_order_map();

	std::vector<order_p> order_vec;
	symbol_order_manager->get_accepted_order_vector(order_vec);
	if (order_vec.empty()) return;

	for (auto it = order_vec.begin(); it != order_vec.end(); ++it) {
		const auto& order = *it;
		if (order->position == SmPositionType::Sell) continue;
		auto order_req = OrderRequestManager::make_cancel_order_request(
			order->account_no,
			account->Pwd(),
			order->symbol_code,
			order->order_no,
			order->order_price,
			order->position,
			order->order_amount,
			SmOrderType::Cancel,
			order->price_type,
			fill_condition_);
		SetProfitLossCut(order_req);
		order_req->request_type = order_request_type_;
		order_req->price_type = price_type_;
		mainApp.order_request_manager()->add_order_request(order_req);
	}
}

void SymbolOrderView::ProcesButtonClick(const std::shared_ptr<SmCell>& cell)
{
	if (!cell) return;

	auto found = _ButtonMap.find(cell);
	if (found == _ButtonMap.end()) return;

	ProcessButtonMsg(found->second);
}

bool SymbolOrderView::ProcesButtonClickByPos(const CPoint& point)
{
	for (auto it = _ButtonMap.begin(); it != _ButtonMap.end(); ++it) {
		auto cell = it->first;
		RECT r{ cell->X(), cell->Y(), cell->X() + cell->Width(), cell->Y() + cell->Height() };
		if (PtInRect(&r, point) == TRUE) {
			ProcessButtonMsg(it->second);
			return true;
		}
	}

	return false;
}

std::pair<int, int> SymbolOrderView::GetOrderCount(const std::shared_ptr<SmCell>& cell)
{
	auto found = row_to_price_.find(cell->Row());
	if (found == row_to_price_.end()) return std::make_pair(0, 0);
	if (cell->Col() == DarkHorse::OrderHeader::BUY_ORDER ||
		cell->Col() == DarkHorse::OrderHeader::SELL_ORDER) {
		return std::make_pair(found->second, cell->GetOrderCount());
	}
	else if (cell->Col() == DarkHorse::OrderHeader::BUY_STOP ||
		cell->Col() == DarkHorse::OrderHeader::SELL_STOP) {
		return std::make_pair(found->second, cell->GetOrderReqCount());
	}
	return std::make_pair(0, 0);
}

std::pair<int, int> SymbolOrderView::get_order_count(const std::shared_ptr<DarkHorse::SmCell>& source_cell)
{
	if (!source_cell ||
		!order_control_ ||
		!buy_stop_order_control_ ||
		!sell_stop_order_control_) return std::make_pair(0, 0);

	auto it_row = row_to_price_.find(source_cell->Row());
	if (it_row == row_to_price_.end()) return std::make_pair(0, 0);

	if (source_cell->Col() == DarkHorse::OrderHeader::SELL_ORDER)
		return order_control_->get_order_count(
			DarkHorse::SmPositionType::Sell,
			it_row->second
		);
	else if (source_cell->Col() == DarkHorse::OrderHeader::BUY_ORDER)
		return order_control_->get_order_count(
			DarkHorse::SmPositionType::Buy,
			it_row->second
		);
	else if (source_cell->Col() == DarkHorse::OrderHeader::SELL_STOP)
		return sell_stop_order_control_->get_order_count(
			it_row->second
		);
	else if (source_cell->Col() == DarkHorse::OrderHeader::BUY_STOP)
		return buy_stop_order_control_->get_order_count(
			it_row->second
		);
	return std::make_pair(0, 0);
}

void SymbolOrderView::ResetHeaderWidth(const int& wnd_width)
{
	_Grid->SetColWidth(DarkHorse::OrderHeader::SELL_STOP, grid_header_vector_[DarkHorse::OrderHeader::SELL_STOP].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::SELL_ORDER, grid_header_vector_[DarkHorse::OrderHeader::SELL_ORDER].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::SELL_CNT, grid_header_vector_[DarkHorse::OrderHeader::SELL_CNT].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::SELL_QTY, grid_header_vector_[DarkHorse::OrderHeader::SELL_QTY].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::QUOTE, grid_header_vector_[DarkHorse::OrderHeader::QUOTE].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::BUY_QTY, grid_header_vector_[DarkHorse::OrderHeader::BUY_QTY].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::BUY_CNT, grid_header_vector_[DarkHorse::OrderHeader::BUY_CNT].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::BUY_ORDER, grid_header_vector_[DarkHorse::OrderHeader::BUY_ORDER].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::BUY_STOP, grid_header_vector_[DarkHorse::OrderHeader::BUY_STOP].width);
}

void SymbolOrderView::SetCenterValues(const bool& make_row_map /*= true*/)
{
	if (!quote_control_ || !product_control_) return;

	const int& close = quote_control_->get_quote().close;
	if (close == 0) return;
	const int start_value = find_start_value();
	try {
		if (make_row_map) {
			price_to_row_.clear();
			row_to_price_.clear();
		}
		int next_value = start_value;
		for (int i = price_start_row_; i < price_end_row_; i++) {
			std::string value_string;
			value_string = std::format("{0}", next_value);
			SmUtil::insert_decimal(value_string, product_control_->get_product().decimal);
			_Grid->SetCellText(i, DarkHorse::OrderHeader::QUOTE, value_string);
			price_to_row_[next_value] = i;
			row_to_price_[i] = next_value;

			next_value = product_control_->get_next_down_value(next_value);
		}
		center_valued_ = true;
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SymbolOrderView::set_center_values(const bool make_row_map /*= true*/)
{

}

int SymbolOrderView::get_start_value()
{
	//int startValue = sym->Quote.intClose + (sym->intTickSize * (_IndexRow - _StartRowForValue));
	return 0;
}

void SymbolOrderView::PutOrderBySpaceBar()
{
	if (!selected_cell_ ) return;

	const int price = FindValue(selected_cell_->Row());

	if (selected_cell_->Col() == DarkHorse::OrderHeader::SELL_ORDER)
		put_order(SmPositionType::Sell, price);
	else if (selected_cell_->Col() == DarkHorse::OrderHeader::BUY_ORDER)
		put_order(SmPositionType::Buy, price);
	else if (selected_cell_->Col() == DarkHorse::OrderHeader::BUY_STOP)
		put_stop_order(SmPositionType::Buy, price);
	else if (selected_cell_->Col() == DarkHorse::OrderHeader::SELL_STOP)
		put_stop_order(SmPositionType::Sell, price);

	_EnableOrderShow = true;
	_EnableStopShow = true;
}

// void SymbolOrderView::PutOrderBySpaceBar(std::shared_ptr<DarkHorse::SmAccount> account)
// {
// 	if (!account || !symbol_) return;
// 
// 	if (!selected_cell_) return;
// }

void SymbolOrderView::ChangeOrderByKey(const int up_down)
{
	if (!account_ || !symbol_) return;
	Invalidate();
}

void SymbolOrderView::UpdateOrderSettings(SmOrderSettings settings)
{
	_OrderSettings = settings;
}

void SymbolOrderView::SetAllRowHeight(const int& row_height)
{
	_Grid->SetAllRowHeight(row_height);
}

void SymbolOrderView::reset_col_widths(const DarkHorse::OrderSetEvent& order_set_event)
{
	grid_header_vector_[0].width = order_set_event.stop_width;
	grid_header_vector_[1].width = order_set_event.order_width;
	grid_header_vector_[2].width = order_set_event.count_width;
	grid_header_vector_[3].width = order_set_event.qty_width;
	grid_header_vector_[4].width = order_set_event.quote_width;
	grid_header_vector_[5].width = order_set_event.qty_width;
	grid_header_vector_[6].width = order_set_event.count_width;
	grid_header_vector_[7].width = order_set_event.order_width;
	grid_header_vector_[8].width = order_set_event.stop_width;

	ResetHeaderWidth(0);
}

int SymbolOrderView::get_entire_width()
{
	int width_sum = 0;
	width_sum += 2;
	width_sum += grid_header_vector_[DarkHorse::OrderHeader::SELL_STOP].width;
	width_sum += 1;
	width_sum += grid_header_vector_[DarkHorse::OrderHeader::SELL_ORDER].width;
	width_sum += 1;
	width_sum += grid_header_vector_[DarkHorse::OrderHeader::SELL_CNT].width;
	width_sum += 1;
	width_sum += grid_header_vector_[DarkHorse::OrderHeader::SELL_QTY].width;
	width_sum += 1;
	width_sum += grid_header_vector_[DarkHorse::OrderHeader::QUOTE].width;
	width_sum += 1;
	width_sum += grid_header_vector_[DarkHorse::OrderHeader::BUY_QTY].width;
	width_sum += 1;
	width_sum += grid_header_vector_[DarkHorse::OrderHeader::BUY_CNT].width;
	width_sum += 1;
	width_sum += grid_header_vector_[DarkHorse::OrderHeader::BUY_ORDER].width;
	width_sum += 1;
	width_sum += grid_header_vector_[DarkHorse::OrderHeader::BUY_STOP].width;
	width_sum += 2;

	return width_sum;
}

void SymbolOrderView::FixedMode(bool val)
{
	_FixedMode = val;
	ArrangeCenterValue();
	Refresh();
}

void SymbolOrderView::init_quote_control(const std::string& symbol_code)
{
	auto quote = mainApp.QuoteMgr()->get_quote(symbol_code);
}

void SymbolOrderView::init_hoga_control(const std::string& symbol_code)
{
	auto hoga = mainApp.HogaMgr()->get_hoga(symbol_code);
}
/// <summary>
/// 개선의 여지가 있음. 
/// </summary>
void SymbolOrderView::set_position() {
	if (!position_control_ || !symbol_) return;
	position_control_->reset_position();
}

void SymbolOrderView::set_order() {
	if (!order_control_ || !symbol_) return;
	
	if (account_) {
		if (account_->is_subaccount()) {
			order_control_->load_from_account(true, account_->No(), symbol_->SymbolCode());
		}
		else {
			order_control_->load_from_account(false, account_->No(), symbol_->SymbolCode());
		}
	}
	else if (fund_) {
		order_control_->load_from_fund(fund_->Name(), symbol_->SymbolCode());
	}
}

void SymbolOrderView::Account(std::shared_ptr<DarkHorse::SmAccount> val)
{
	if (!val || !position_control_ || !order_control_) return;
	
	account_ = val;
	if (account_->is_subaccount()) {
		position_type_ = DarkHorse::PositionType::SubAccount;
		order_type_ = DarkHorse::OrderType::SubAccount;
	}
	else {
		position_type_ = DarkHorse::PositionType::MainAccount;
		order_type_ = DarkHorse::OrderType::MainAccount;
	}
	order_control_->set_account(val);
	position_control_->set_account(val);
	set_position();
	set_order();
}

void SymbolOrderView::Symbol(std::shared_ptr<DarkHorse::SmSymbol> val)
{
	if (!val || !position_control_ || !order_control_) return;
	center_valued_ = false;
	symbol_ = val;
	auto quote = mainApp.QuoteMgr()->get_quote(symbol_->SymbolCode());
	quote->symbol_id = val->Id();
	quote_control_->set_symbol_id(val->Id());
	quote_control_->update_quote(quote);
	auto hoga = mainApp.HogaMgr()->get_hoga(symbol_->SymbolCode());
	hoga->symbol_id = val->Id();
	hoga_control_->set_symbol_id(val->Id());
	hoga_control_->update_hoga(hoga);
	product_control_->update_product(symbol_);

	position_control_->set_symbol(val);
	order_control_->set_symbol(val);
	set_position();
	//set_order();
	sell_stop_order_control_->set_control_type(SmPositionType::Sell);
	sell_stop_order_control_->set_symbol_id(symbol_->Id());
	buy_stop_order_control_->set_control_type(SmPositionType::Buy);
	buy_stop_order_control_->set_symbol_id(symbol_->Id());
	ArrangeCenterValue();
	on_update_quote();
	on_update_hoga();
	Invalidate();
}

void SymbolOrderView::DrawStopOrder()
{
	for (size_t i = 0; i < _StopRectVector.size(); ++i) {
		CBCGPPoint start_point = _StopRectVector[i].first.CenterPoint();
		CBCGPPoint end_point = _StopRectVector[i].second.CenterPoint();
		DrawArrow(start_point, end_point, 1.0f, 6);
	}
}

void SymbolOrderView::draw_buy_stop_order()
{
	for (size_t i = 0; i < buy_stop_order_rect_vector_.size(); ++i) {
		CBCGPPoint start_point = buy_stop_order_rect_vector_[i].first.CenterPoint();
		CBCGPPoint end_point = buy_stop_order_rect_vector_[i].second.CenterPoint();
		DrawArrow(start_point, end_point, 1.0f, 6);
	}
}

void SymbolOrderView::draw_sell_stop_order()
{
	for (size_t i = 0; i < sell_stop_order_rect_vector_.size(); ++i) {
		CBCGPPoint start_point = sell_stop_order_rect_vector_[i].first.CenterPoint();
		CBCGPPoint end_point = sell_stop_order_rect_vector_[i].second.CenterPoint();
		DrawArrow(start_point, end_point, 1.0f, 6);
	}
}

void SymbolOrderView::DrawArrow(const CBCGPPoint& start_point, const CBCGPPoint& end_point, const double& stroke_width, const int& head_width)
{
	//const double stroke_width = 2.0f;
	m_pGM->DrawLine(start_point, end_point, _Resource.SelectedBrush, stroke_width);

	CBCGPPointsArray arrow_array;

	const int head_length = head_width; // , head_width = 6;

	const float dx = static_cast<float>(end_point.x - start_point.x);
	const float dy = static_cast<float>(end_point.y - start_point.y);
	const auto length = std::sqrt(dx * dx + dy * dy);

	// ux,uy is a unit vector parallel to the line.
	const auto ux = dx / length;
	const auto uy = dy / length;

	// vx,vy is a unit vector perpendicular to ux,uy
	const auto vx = -uy;
	const auto vy = ux;
	const auto half_width = 0.5f * head_width;

	const CBCGPPoint arrow[3] =
	{ end_point,
		CBCGPPoint{ (end_point.x - head_length * ux + half_width * vx),
		(end_point.y - head_length * uy + half_width * vy) },
		CBCGPPoint{ (end_point.x - head_length * ux - half_width * vx),
		(end_point.y - head_length * uy - half_width * vy) }
	};
	for (int i = 0; i < 3; i++) arrow_array.Add(arrow[i]);

	m_pGM->FillGeometry(CBCGPPolygonGeometry(arrow_array), _Resource.SelectedBrush);
}

int SymbolOrderView::find_close_row_from_end_row()
{
	const int close_value = quote_control_->get_quote().close;
	int next_value = 0;
	int target_row = price_end_row_;

	do {
		next_value = product_control_->get_next_up_value(next_value);
		target_row--;
	} while (next_value < close_value);

	return target_row;
}

int SymbolOrderView::find_zero_value_row()
{
	if (!product_control_) return -1;
	return product_control_->get_row(0, index_row_, quote_control_->get_quote().close);
}

int SymbolOrderView::find_row(const int value)
{
	if (!product_control_ || price_to_row_.size() == 0)
		return 0;
	auto it = price_to_row_.find(value);
	if (it != price_to_row_.end()) { // 값이 보이는 범위 안에 있을 때
		return it->second;
	}
	else { // 값이 보이는 범위 밖에 있을 때
		const VmProduct& product = product_control_->get_product();
		auto itr = price_to_row_.rbegin();
		int big_value = itr->first;
		int big_row = itr->second;
		int thousand_row = 0;
		// 가격이 10인 행을 찾는 과정 - 10이상이면 승수가 변한다.
		if (big_value >= 1000) { // 최상위 값이 10보다 이상일 경우
			int delta = big_value - 1000;
			int delta_row = delta / product.int_tick_size;
			thousand_row = big_row + delta_row;
		}
		else { // 최상위 값이 10미만인 경우
			int delta = 1000 - big_value;
			thousand_row = big_row - delta;
		}

		if (value >= 1000) { // 가격이 10 이상인 있는 경우 - 종목의 틱크기 만큼 변함
			int delta = value - 1000;
			int delta_row = delta / product.int_tick_size;
			return thousand_row - delta_row;
		}
		else { // 가격이 10 미만인 경우 - 종목에 관계없이 1씩 변함
			int delta = 1000 - value;
			return thousand_row + delta;
		}
	}
}

int SymbolOrderView::find_row2(const int target_value)
{
	assert(!quote_control_ || !product_control_);

	const int& close = quote_control_->get_quote().close;
	if (close == 0) return 0;
	int next_value = close;
	int next_row = _Grid->index_row();
	if (target_value == next_value) return next_row;
	return target_value;
}
int SymbolOrderView::find_value(const int target_row)
{
	return -1;
}

int SymbolOrderView::find_start_value()
{
	if (!quote_control_ || !product_control_) return 0;
	const int& close = quote_control_->get_quote().close;
	if (close == 0) return 0;
	const DarkHorse::ValueType value_type = product_control_->get_value_type();
	const VmProduct& product = product_control_->get_product();
	if (value_type == ValueType::KospiOption ||
		value_type == ValueType::KospiWeekly ||
		value_type == ValueType::MiniKospiOption) {
		int endValue = close;
		int endRow = price_end_row_ - 1;
		int zeroRow = price_end_row_;
		if (index_row_ < endRow) {
			for (int r = index_row_; r < endRow; ++r) {
				// 0.01 밑으로 안나오게 함
				if (/*endValue == 1 ||*/ endValue == 0) {
					zeroRow = r;
					break;
				}
				if (endValue <= 1000)
					endValue -= product.int_tick_size;
				else
					endValue -= 5;
			}

			if (zeroRow < endRow) {
				index_row_ = endRow - (zeroRow - index_row_) + 1;
			}
		}

		int startValue = close;
		if (index_row_ > price_start_row_) {
			for (int r = index_row_; r > price_start_row_; --r) {
				if (startValue < 1000)
					startValue += product.int_tick_size;
				else
					startValue += 5;
			}
		}
		else if (index_row_ <= price_start_row_) {
			for (int r = index_row_; r < price_start_row_; ++r) {
				if (startValue <= 1000)
					startValue -= product.int_tick_size;
				else
					startValue -= 5;
			}
		}
		return startValue;
	}
	else {
		return  close + product.int_tick_size * (index_row_ - price_start_row_);
	}

}

int SymbolOrderView::get_center_row()
{
	return static_cast<int>(_Grid->RowCount() / 2);
}
void SymbolOrderView::ProcessFixedMode()
{
	index_row_ = get_center_row();

	SetCenterValues(false);

	_EnableOrderShow = true;
	_EnableQuoteShow = true;
	_EnableHogaShow = true;
	_EnableStopShow = true;
}



void SymbolOrderView::SetProfitLossCut(std::shared_ptr<SmOrderRequest> order_req)
{
	order_req->CutSlip = _OrderSettings.SlipTick;
	order_req->ProfitCutTick = _OrderSettings.ProfitCutTick;
	order_req->LossCutTick = _OrderSettings.LossCutTick;
	order_req->CutPriceType = _OrderSettings.PriceType;
	if (_OrderSettings.ProfitCut) {
		if (_OrderSettings.LossCut)
			order_req->CutMode = 3;
		else
			order_req->CutMode = 1;
	}
	else if (_OrderSettings.LossCut) {
		if (_OrderSettings.ProfitCut)
			order_req->CutMode = 3;
		else
			order_req->CutMode = 2;
	}
	else
		order_req->CutMode = 0;
}

void SymbolOrderView::SetProfitLossCut(std::shared_ptr<OrderRequest> order_req)
{
	order_req->cut_slip = _OrderSettings.SlipTick;
	order_req->profit_cut_tick = _OrderSettings.ProfitCutTick;
	order_req->loss_cut_tick = _OrderSettings.LossCutTick;
	order_req->cut_price_type = _OrderSettings.PriceType;
	if (_OrderSettings.ProfitCut) {
		if (_OrderSettings.LossCut)
			order_req->cut_mode = SmCutMode::BothCut;
		else
			order_req->cut_mode = SmCutMode::ProfitCut;
	}
	else if (_OrderSettings.LossCut) {
		if (_OrderSettings.ProfitCut)
			order_req->cut_mode = SmCutMode::BothCut;
		else
			order_req->cut_mode = SmCutMode::LossCut;
	}
	else
		order_req->cut_mode = SmCutMode::None;
}

void SymbolOrderView::SetStopOrderCut(std::shared_ptr<SmOrderRequest> order_req)
{
	order_req->CutSlip = _OrderSettings.SlipTick;
}

void SymbolOrderView::set_order_close(std::shared_ptr<DarkHorse::OrderRequest> order_req)
{
	if (!quote_control_ ) return;
	const VmQuote quote = quote_control_->get_quote();
	order_req->order_context.close = quote.close;
}

void SymbolOrderView::set_virtual_filled_value(std::shared_ptr<DarkHorse::OrderRequest> order_req)
{
	if (!quote_control_ || !product_control_) return;
	const VmQuote quote = quote_control_->get_quote();
	order_req->order_context.virtual_filled_price = quote.close + product_control_->get_product().int_tick_size * 4;
}


void SymbolOrderView::clear_buy_stop_order()
{
	for (auto it = old_stop_buy_order_index_.begin(); it != old_stop_buy_order_index_.end(); ++it) {
		auto pCell = _Grid->FindCell(*it, DarkHorse::OrderHeader::BUY_STOP);
		if (pCell) {
			pCell->ClearOrderReq();
			pCell->Text("");
		}
	}
	old_stop_buy_order_index_.clear();
}

void SymbolOrderView::clear_sell_stop_order()
{
	for (auto it = old_stop_sell_order_index_.begin(); it != old_stop_sell_order_index_.end(); ++it) {
		auto pCell = _Grid->FindCell(*it, DarkHorse::OrderHeader::SELL_STOP);
		if (pCell) {
			pCell->ClearOrderReq();
			pCell->Text("");
		}
	}
	old_stop_sell_order_index_.clear();
}

void SymbolOrderView::put_stop_order(const DarkHorse::SmPositionType& type, const int& price)
{
	if (!account_ || !symbol_) return;
	if (price <= 0) return;

	std::shared_ptr<SmOrderRequest> order_req = nullptr;
	if (type == SmPositionType::Sell)
		sell_stop_order_control_->add_stop_order_request(
			account_,
			symbol_,
			type,
			price,
			_OrderAmount,
			_OrderSettings.SlipTick
		);
	else buy_stop_order_control_->add_stop_order_request(
		account_,
		symbol_,
		type,
		price,
		_OrderAmount,
		_OrderSettings.SlipTick
	);
}

void SymbolOrderView::put_order(const SmPositionType& type, const int& price, const SmPriceType& price_type)
{
	if (order_type_ == OrderType::MainAccount || order_type_ == OrderType::SubAccount) {
		put_order(account_, symbol_->SymbolCode(), type, price, _OrderAmount, price_type);
	}
	else if (order_type_ == OrderType::Fund) {
		if (!fund_) return;
		for (auto& account : fund_->GetAccountVector()) {
			put_order(account, symbol_->SymbolCode(), type, price, _OrderAmount, price_type);
		}
	}
}

void SymbolOrderView::put_order(
	std::shared_ptr<DarkHorse::SmAccount> account, 
	const std::string& symbol_code, 
	const DarkHorse::SmPositionType& type, 
	const int price, 
	const int amount, 
	const DarkHorse::SmPriceType price_type)
{
	if (!symbol_ || !account) return;

	if (symbol_->SymbolCode() != symbol_code) return;
	if (price < 0) return;

	auto parent_account = mainApp.AcntMgr()->FindAccountById(account->parent_id());

	std::shared_ptr<OrderRequest> order_req = nullptr;
	order_req = OrderRequestManager::make_order_request(
		parent_account ? parent_account->No() : account->No(),
		mainApp.AcntMgr()->get_password(account->No()),
		price,
		amount,
		symbol_->SymbolCode(),
		type,
		SmOrderType::New,
		price_type,
		fill_condition_);
	if (order_req) {
		order_req->request_type = order_request_type_;
		order_req->order_context.order_control_id = id_;
		order_req->order_context.order_source_type = OrderType::MainAccount;
		if (parent_account) {
			order_req->order_context.parent_account_id = parent_account->id();
			order_req->order_context.parent_account_no = parent_account->No();
			order_req->order_context.sub_account_no = account->No();
			order_req->order_context.order_source_type = OrderType::SubAccount;
		}
		if (fund_) {
			order_req->order_context.order_source_type = OrderType::Fund;
			order_req->order_context.fund_id = fund_->Id();
			order_req->order_context.fund_name = fund_->Name();
		}

		if (symbol_->gubun_code() == "21")
			order_req->future_or_option = 0;
		else if (symbol_->gubun_code() == "61")
			order_req->future_or_option = 0;
		else if (symbol_->gubun_code() == "84")
			order_req->future_or_option = 0;
		else if (symbol_->gubun_code() == "45")
			order_req->future_or_option = 0;
		else
			order_req->future_or_option = 1;

		SetProfitLossCut(order_req);
		mainApp.order_request_manager()->add_order_request(order_req);
	}
}

void SymbolOrderView::CreateResource()
{
	//_Resource.QMBuyBrush.SetOpacity(0.5);
	//_Resource.QMSellBrush.SetOpacity(0.5);
	//_Resource.QMHighLowBrush.SetOpacity(0.5);
	_Resource.OrderStroke.SetStartCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_ROUND);
	_Resource.OrderStroke.SetEndCap(CBCGPStrokeStyle::BCGP_CAP_STYLE::BCGP_CAP_STYLE_TRIANGLE);
	CBCGPTextFormat fmt3(_T("굴림"), globalUtils.ScaleByDPI(30.0f));

	fmt3.SetFontSize(12);
	fmt3.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt3.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt3.SetWordWrap();
	fmt3.SetClipText();

	_Resource.TextFormat = fmt3;

	CBCGPTextFormat fmt(_T("Tahoma"), globalUtils.ScaleByDPI(30.0f));

	fmt.SetFontSize(14);
	fmt.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt.SetFontWeight(FW_BOLD);
	fmt.SetWordWrap();
	fmt.SetClipText();

	_Resource.QuoteTextFormat = fmt;


	CBCGPTextFormat fmt2(_T("굴림"), globalUtils.ScaleByDPI(30.0f));

	fmt2.SetFontSize(12);
	fmt2.SetTextAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt2.SetWordWrap();
	fmt2.SetClipText();
	fmt2.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	//fmt2.SetFontWeight(FW_BOLD);

	_Resource.QuoteNumberFormat = fmt2;
}

void SymbolOrderView::increase_close_row(const int& delta)
{
	index_row_ += delta;
	if (quote_control_->get_quote().close == 0) return;
	_Grid->index_row(index_row_);
}

void SymbolOrderView::set_close_row()
{
	if (quote_control_->get_quote().close == 0) return;
}

void SymbolOrderView::ClearOldSelectedCells()
{
	for (size_t i = 0; i < _SelectedCells.size(); i++) {
		auto cell = _SelectedCells[i];
		//_Grid->DrawCell(cell, m_pGM, _Resource.GridNormalBrush, _Resource.GridNormalBrush, _Resource.TextBrush, _Resource.TextFormat, true, true, true);
		cell->set_show_move_rect(false);
	}
	_SelectedCells.clear();
}

void SymbolOrderView::DrawMovingRect()
{
	if (_MouseMode == MouseMode::Normal) {
		ClearOldSelectedCells();
		auto cell = _Grid->FindCellByPos(_X, _Y);
		if (!cell) return;
		if (cell->Row() <= 0) return;

		if (cell->Col() == DarkHorse::OrderHeader::SELL_ORDER) {
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::BUY_ORDER);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::QUOTE);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
		}
		else if (cell->Col() == DarkHorse::OrderHeader::BUY_ORDER) {
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::SELL_ORDER);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::QUOTE);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
		}
		else if (cell->Col() == DarkHorse::OrderHeader::QUOTE) {
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
		}
		else if (cell->Col() == DarkHorse::OrderHeader::BUY_STOP) {
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::SELL_STOP);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::QUOTE);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
		}
		else if (cell->Col() == DarkHorse::OrderHeader::SELL_STOP) {
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);

			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::BUY_STOP);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::QUOTE);
			if (!cell) return;
			_SelectedCells.push_back(cell);
			_Grid->DrawCell(cell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
		}
	}
}

void SymbolOrderView::set_moving_rect()
{
	if (_MouseMode == MouseMode::Normal) {
		reset_moving_rect();
		auto cell = _Grid->FindCellByPos(_X, _Y);
		if (!cell) return;
		if (cell->Row() <= 0) return;

		if (cell->Col() == DarkHorse::OrderHeader::SELL_ORDER) {
			add_moving_rect(cell);
			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::BUY_ORDER);
			add_moving_rect(cell);
			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::QUOTE);
			add_moving_rect(cell);
		}
		else if (cell->Col() == DarkHorse::OrderHeader::BUY_ORDER) {
			add_moving_rect(cell);
			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::SELL_ORDER);
			add_moving_rect(cell);
			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::QUOTE);
			add_moving_rect(cell);
		}
		else if (cell->Col() == DarkHorse::OrderHeader::QUOTE) {
			add_moving_rect(cell);
		}
		else if (cell->Col() == DarkHorse::OrderHeader::BUY_STOP) {
			add_moving_rect(cell);
			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::SELL_STOP);
			add_moving_rect(cell);
			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::QUOTE);
			add_moving_rect(cell);
		}
		else if (cell->Col() == DarkHorse::OrderHeader::SELL_STOP) {
			add_moving_rect(cell);
			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::BUY_STOP);
			add_moving_rect(cell);
			cell = _Grid->FindCell(cell->Row(), DarkHorse::OrderHeader::QUOTE);
			add_moving_rect(cell);
		}
	}
}

void SymbolOrderView::reset_moving_rect()
{
	for (size_t i = 0; i < _SelectedCells.size(); i++) {
		auto cell = _SelectedCells[i];
		cell->set_show_move_rect(false);
	}
	_SelectedCells.clear();
}

void SymbolOrderView::add_moving_rect(const std::shared_ptr<DarkHorse::SmCell>& src_cell)
{
	if (!src_cell) return;
	src_cell->set_show_move_rect(true);
	_SelectedCells.push_back(src_cell);
}

void SymbolOrderView::CancelOrder(const std::shared_ptr<DarkHorse::SmCell>& src_cell)
{
	if (!src_cell) return;

	if (src_cell->GetOrderCount() == 0) return;

	const std::map<int, std::shared_ptr<SmOrder>>& order_map = src_cell->GetOrderMap();
	for (auto it = order_map.begin(); it != order_map.end(); it++) {
		auto order_req = SmOrderRequestManager::MakeChangeOrderRequest(
			account_->No(),
			account_->Pwd(), symbol_->SymbolCode(),
			it->second->OrderNo, it->second->OrderPrice, it->second->PositionType,
			it->second->OrderAmount);
		mainApp.Client()->CancelOrder(order_req);
	}

	src_cell->ClearOrder();
}

void SymbolOrderView::cancel_order(const std::shared_ptr<DarkHorse::SmCell>& src_cell)
{
	if (!src_cell) return;
	auto it_row = row_to_price_.find(src_cell->Row());
	if (it_row == row_to_price_.end()) return;
	std::vector<std::shared_ptr<Order>> order_vec;
	if (src_cell->Col() == DarkHorse::OrderHeader::SELL_ORDER) {
		order_control_->get_order(SmPositionType::Sell, it_row->second, order_vec);
		if (order_vec.empty()) return;
		cancel_order(order_vec);
		order_control_->clear_order(SmPositionType::Sell, it_row->second);
	}
	else {
		order_control_->get_order(SmPositionType::Buy, it_row->second, order_vec);
		if (order_vec.empty()) return;
		cancel_order(order_vec);
		order_control_->clear_order(SmPositionType::Buy, it_row->second);
	}
	_EnableOrderShow = true;
}

void SymbolOrderView::cancel_order(std::vector<std::shared_ptr<DarkHorse::Order>> order_vec)
{
	for (auto it = order_vec.begin(); it != order_vec.end(); ++it) {
		const auto& order = *it;
		auto parent_account = mainApp.AcntMgr()->get_parent_account(order->account_no);
		auto order_req = OrderRequestManager::make_cancel_order_request(
			mainApp.AcntMgr()->get_account_no(order->account_no),
			mainApp.AcntMgr()->get_password(order->account_no),
			order->symbol_code,
			order->order_no,
			order->order_price,
			order->position,
			order->order_amount,
			SmOrderType::Cancel,
			order->price_type,
			fill_condition_);
		order_req->order_context.order_source_type = OrderType::MainAccount;
		if (parent_account) {
			order_req->order_context.parent_account_id = parent_account->id();
			order_req->order_context.parent_account_no = parent_account->No();
			order_req->order_context.sub_account_no = order->account_no;
			order_req->order_context.order_source_type = OrderType::SubAccount;
		}
		if (fund_) {
			order_req->order_context.order_source_type = OrderType::Fund;
			order_req->order_context.fund_id = fund_->Id();
			order_req->order_context.fund_name = fund_->Name();
		}
		SetProfitLossCut(order_req);
		order_req->request_type = order_request_type_;
		order_req->price_type = price_type_;
		order_req->order_context.order_control_id = id_;
		mainApp.order_request_manager()->add_order_request(order_req);
	}
}

void SymbolOrderView::ChangeOrder(const std::shared_ptr<DarkHorse::SmCell>& src_cell, const int& tgt_price)
{
	if (!src_cell) return;
	if (src_cell->GetOrderCount() == 0) return;
	const std::map<int, std::shared_ptr<SmOrder>>& order_map = src_cell->GetOrderMap();
	for (auto it = order_map.begin(); it != order_map.end(); it++) {
		auto order_req = SmOrderRequestManager::MakeChangeOrderRequest(
			account_->No(),
			account_->Pwd(), symbol_->SymbolCode(),
			it->second->OrderNo, tgt_price, it->second->PositionType,
			it->second->OrderAmount);
		SetProfitLossCut(order_req);
		mainApp.Client()->ChangeOrder(order_req);
	}

	src_cell->ClearOrder();
	_EnableOrderShow = true;
}

void SymbolOrderView::change_order(const std::shared_ptr<DarkHorse::SmCell>& src_cell, const int& target_price)
{
	if (!src_cell) return;
	auto it_row = row_to_price_.find(src_cell->Row());
	if (it_row == row_to_price_.end()) return;
	std::vector<std::shared_ptr<Order>> order_vec;
	if (src_cell->Col() == DarkHorse::OrderHeader::SELL_ORDER) {
		order_control_->get_order(SmPositionType::Sell, it_row->second, order_vec);
		if (order_vec.empty()) return;
		change_order(order_vec, target_price);
		order_control_->clear_order(SmPositionType::Sell, it_row->second);
	}
	else {
		order_control_->get_order(SmPositionType::Buy, it_row->second, order_vec);
		if (order_vec.empty()) return;
		change_order(order_vec, target_price);
		order_control_->clear_order(SmPositionType::Buy, it_row->second);
	}
	_EnableOrderShow = true;
}

void SymbolOrderView::change_order(const std::vector<std::shared_ptr<DarkHorse::Order>>& order_vec, const int& target_price)
{
	for (auto it = order_vec.begin(); it != order_vec.end(); ++it) {
		const auto& order = *it;
		// 잔량이 설정되지 않으면 주문 정정을 못하게 막는다. 
		if (order->remain_count == 0) continue;
		auto parent_account = mainApp.AcntMgr()->get_parent_account(order->account_no);
		auto order_req = OrderRequestManager::make_change_order_request(
			mainApp.AcntMgr()->get_account_no(order->account_no),
			mainApp.AcntMgr()->get_password(order->account_no),
			order->symbol_code,
			order->order_no, 
			target_price, 
			order->position,
			order->order_amount,
			SmOrderType::Modify,
			order->price_type,
			fill_condition_);
		order_req->order_context.order_source_type = OrderType::MainAccount;
		if (parent_account) {
			order_req->order_context.parent_account_id = parent_account->id();
			order_req->order_context.parent_account_no = parent_account->No();
			order_req->order_context.sub_account_no = order->account_no;
			order_req->order_context.order_source_type = OrderType::SubAccount;
		}
		if (fund_) {
			order_req->order_context.order_source_type = OrderType::Fund;
			order_req->order_context.fund_id = fund_->Id();
			order_req->order_context.fund_name = fund_->Name();
		}
		SetProfitLossCut(order_req);
		set_order_close(order_req);
		order_req->request_type = order_request_type_;
		order_req->price_type = price_type_;
		order_req->order_context.virtual_filled_price = target_price;
		order_req->order_context.order_control_id = id_;
		//set_virtual_filled_value(order_req);
		mainApp.order_request_manager()->add_order_request(order_req);
	}
}

void SymbolOrderView::change_stop(const std::shared_ptr<DarkHorse::SmCell>& src_cell, const std::shared_ptr<DarkHorse::SmCell>& tgt_cell, const int& src_price, const int& target_price)
{
	if (!src_cell || !account_) return;
	auto it_row = row_to_price_.find(src_cell->Row());
	if (it_row == row_to_price_.end()) return;
	price_order_request_map_p price_order_req_map = nullptr;
	if (src_cell->Col() == DarkHorse::OrderHeader::SELL_STOP) {
		enable_sell_stop_order_show_ = true;
		price_order_req_map = sell_stop_order_control_->get_price_order_req_map(it_row->second);
		const std::map<int, order_request_p>& req_map = price_order_req_map->get_order_request_map();
		for (auto it = req_map.begin(); it != req_map.end(); it++)
			sell_stop_order_control_->add_stop_order_request(target_price, it->second);
		price_order_req_map->clear();
		sell_stop_order_control_->remove_stop_order_request(it_row->second);
	}
	else {
		enable_buy_stop_order_show_ = true;
		price_order_req_map = buy_stop_order_control_->get_price_order_req_map(it_row->second);
		const std::map<int, order_request_p>& req_map = price_order_req_map->get_order_request_map();
		for (auto it = req_map.begin(); it != req_map.end(); it++)
			buy_stop_order_control_->add_stop_order_request(target_price, it->second);
		price_order_req_map->clear();
		buy_stop_order_control_->remove_stop_order_request(it_row->second);
	}
}


void SymbolOrderView::change_stop(price_order_request_map_p order_req_map, const int target_price)
{
	
}

void SymbolOrderView::cancel_stop(const std::shared_ptr<DarkHorse::SmCell>& src_cell)
{
	if (!src_cell) return;
	if (!sell_stop_order_control_ || !buy_stop_order_control_) return;
	auto it = row_to_price_.find(src_cell->Row());
	if (it == row_to_price_.end()) return;

	if (src_cell->Col() == DarkHorse::OrderHeader::SELL_STOP)
		sell_stop_order_control_->remove_stop_order_request(it->second);
	else
		buy_stop_order_control_->remove_stop_order_request(it->second);
}

void SymbolOrderView::ProcessButtonMsg(const BUTTON_ID& id)
{
	switch (id)
	{
	case BUTTON_ID::ARRANGE: ArrangeCenterValue(); break;
	case BUTTON_ID::BUY: BuyByMarketPrice(); break;
	case BUTTON_ID::SELL: SellByMarketPrice(); break;
	case BUTTON_ID::CANCEL_SELL_STOP: CancelSellStop(); break;
	case BUTTON_ID::CANCEL_SELL_ORDER: CancelSellOrder(); break;
	case BUTTON_ID::CANCEL_ALL: CancelAllOrder(); break;
	case BUTTON_ID::CANCEL_BUY_ORDER: CancelBuyOrder(); break;
	case BUTTON_ID::CANCEL_BUY_STOP: CancelBuyStop(); break;
	}
}

void SymbolOrderView::OnPaint()
{
	if (_Editing) return;

	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rect;
	GetClientRect(rect);

	if (m_pGM == NULL) return;

	try {
		m_pGM->BindDC(pDC, rect);
		if (!m_pGM->BeginDraw()) return;

		m_pGM->Clear();

		m_pGM->FillRectangle(rect, _Resource.GridNormalBrush);
		rect.right -= 1;
		rect.bottom -= 1;
		_Grid->SetColHeaderTitles(_OrderTableHeader);
		_Grid->DrawGrid(m_pGM, rect);
		set_moving_rect();
		set_fixed_selected_cell();
		_Grid->draw_cells(m_pGM, rect);
		//rect.right += 1;
		_Grid->DrawBorder(m_pGM, rect, _Selected);
		DrawMovingOrder();

		draw_buy_stop_order();
		draw_sell_stop_order();

		DrawHogaLine(rect);


		m_pGM->EndDraw();
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SymbolOrderView::OnMouseMove(UINT nFlags, CPoint point)
{
	CString msg;
	msg.Format("OnMouseMove %d, %d\n", point.x, point.y);
	//TRACE(msg);

	_X = point.x;
	_Y = point.y;

	_EndX = point.x;
	_EndY = point.y;

	if (!m_bTrackMouse)
	{

		TRACKMOUSEEVENT tme;

		tme.cbSize = sizeof(tme);

		tme.hwndTrack = m_hWnd;

		tme.dwFlags = TME_LEAVE | TME_HOVER;

		tme.dwHoverTime = 1;

		if (TrackMouseEvent(&tme))

		{

			m_bTrackMouse = TRUE;

		}

	}
	_EnableOrderShow = true;
	_EnableQuoteShow = true;
	_EnableHogaShow = true;
	_EnableStopShow = true;
	//Invalidate();
}

void SymbolOrderView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString msg;
	msg.Format("OnKeyDown %d, %d\n", nChar, nRepCnt);
	TRACE(msg);
	Invalidate();
}

void SymbolOrderView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString msg;
	msg.Format("OnChar %d, %d\n", nChar, nRepCnt);
	TRACE(msg);
	Invalidate();
}


void SymbolOrderView::OnMouseHover(UINT nFlags, CPoint point)
{
	_Hover = true;
	Invalidate();
}

void SymbolOrderView::OnMouseLeave()
{
	if (_MouseMode == MouseMode::Normal) {
		ClearOldSelectedCells();
		clear_fixed_selected_cell();
		_SelectedValue.first = -1;
		_SelectedValue.second = -1;
	}
	m_bTrackMouse = FALSE;
	_Hover = false;
	Invalidate();
}

void SymbolOrderView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	if (ProcesButtonClickByPos(point)) return;

	if (account_)
		mainApp.event_hub()->process_symbol_order_view_event(account_->Type(), center_window_id_, symbol_);
	if (fund_)
		mainApp.event_hub()->process_symbol_order_view_event(fund_->fund_type(), center_window_id_, symbol_);

	auto cell = _Grid->FindCellByPos(_X, _Y);
	if (!cell) return;
	clear_fixed_selected_cell();
	selected_cell_ = cell;
	//selected_cell_->CellType(SmCellType::CT_SELECTED);
	selected_cell_->set_show_move_rect(true);

	auto cell_pos = _Grid->FindRowCol(point.x, point.y);
	if (cell_pos.second == DarkHorse::OrderHeader::SELL_ORDER ||
		cell_pos.second == DarkHorse::OrderHeader::BUY_ORDER ||
		cell_pos.second == DarkHorse::OrderHeader::SELL_STOP ||
		cell_pos.second == DarkHorse::OrderHeader::BUY_STOP) {
		const int price = FindValue(cell_pos.first);
		_SelectedValue.first = price;
		_SelectedValue.second = cell_pos.second;
	}

	const auto order_count = get_order_count(cell);

	if (order_count.second > 0) {
		_MovingOrder = true;
		_OrderStartCol = cell->Col();
		_OrderStartRow = cell->Row();
		_OrderStartPrice = order_count.first;
		_DraggingOrder = true;

		_StartX = point.x;
		_StartY = point.y;

		SetCapture();
	}

	CBCGPStatic::OnLButtonDown(nFlags, point);
}


void SymbolOrderView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (_MouseMode == MouseMode::Normal) {
		auto cell = _Grid->FindCellByPos(_X, _Y);
		if (cell) { 
			clear_fixed_selected_cell();
			selected_cell_ = cell; 
			//selected_cell_->CellType(CT_NORMAL);
			selected_cell_->set_show_move_rect(true);
		}

		const auto order_count = get_order_count(cell);

		if (order_count.second > 0) {
			if (cell->Col() == DarkHorse::OrderHeader::SELL_ORDER)
				cancel_order(selected_cell_);
			else if (cell->Col() == DarkHorse::OrderHeader::BUY_ORDER)
				cancel_order(selected_cell_);
			else if (cell->Col() == DarkHorse::OrderHeader::BUY_STOP)
				cancel_stop(selected_cell_);
			else if (cell->Col() == DarkHorse::OrderHeader::SELL_STOP)
				cancel_stop(selected_cell_);

			_EnableOrderShow = true;
			_EnableStopShow = true;
		}
	}

	CBCGPStatic::OnRButtonDown(nFlags, point);
}


void SymbolOrderView::OnLButtonUp(UINT nFlags, CPoint point)
{

	if (_DraggingOrder) {
		_EndX = point.x;
		_EndY = point.y;

		auto cell = _Grid->FindCellByPos(_EndX, _EndY);

		if (!cell) {
			if (_OrderStartCol == DarkHorse::OrderHeader::SELL_ORDER)
				cancel_order(selected_cell_);
			else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_ORDER)
				cancel_order(selected_cell_);
			else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_STOP)
				cancel_stop(selected_cell_);
			else if (_OrderStartCol == DarkHorse::OrderHeader::SELL_STOP)
				cancel_stop(selected_cell_);
		}
		else {
			if (cell->Row() == selected_cell_->Row()) {
				_DraggingOrder = false;
				ReleaseCapture();
				_MovingOrder = false;
				CBCGPStatic::OnLButtonUp(nFlags, point);
				return;
			}
			if (cell->Col() == _OrderStartCol) {
				auto found = row_to_price_.find(cell->Row());
				if (found == row_to_price_.end()) return;

				if (_OrderStartCol == DarkHorse::OrderHeader::SELL_ORDER)
					change_order(selected_cell_, found->second);
				else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_ORDER)
					change_order(selected_cell_, found->second);
				else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_STOP)
					change_stop(selected_cell_, cell, _OrderStartPrice, found->second);
				else if (_OrderStartCol == DarkHorse::OrderHeader::SELL_STOP)
					change_stop(selected_cell_, cell, _OrderStartPrice, found->second);
			}
			else {
				if (_OrderStartCol == DarkHorse::OrderHeader::SELL_ORDER)
					cancel_order(selected_cell_);
				else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_ORDER)
					cancel_order(selected_cell_);
				else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_STOP)
					cancel_stop(selected_cell_);
				else if (_OrderStartCol == DarkHorse::OrderHeader::SELL_STOP)
					cancel_stop(selected_cell_);
			}
		}

		_DraggingOrder = false;
		ReleaseCapture();
	}
	_EnableStopShow = true;
	_EnableOrderShow = true;
	_MovingOrder = false;
	Invalidate();
	CBCGPStatic::OnLButtonUp(nFlags, point);
}


int SymbolOrderView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rect;
	GetClientRect(rect);
	return 0;
}


void SymbolOrderView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!_Grid) return;
	auto cell_pos = _Grid->FindRowCol(point.x, point.y);
	if (cell_pos.second == DarkHorse::OrderHeader::SELL_STOP) {
		const int price = FindValue(cell_pos.first);
		put_stop_order(SmPositionType::Sell, price);
	}
	else if (cell_pos.second == DarkHorse::OrderHeader::BUY_STOP) {
		const int price = FindValue(cell_pos.first);
		put_stop_order(SmPositionType::Buy, price);
	}
	else if (cell_pos.second == DarkHorse::OrderHeader::SELL_ORDER) {
		const int price = FindValue(cell_pos.first);
		put_order(SmPositionType::Sell, price);
	}
	else if (cell_pos.second == DarkHorse::OrderHeader::BUY_ORDER) {
		const int price = FindValue(cell_pos.first);
		put_order(SmPositionType::Buy, price);
	}

	CBCGPStatic::OnLButtonDblClk(nFlags, point);
}

LRESULT SymbolOrderView::OnWmSymbolMasterReceived(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = static_cast<int>(wParam);
	if (!symbol_ || symbol_->Id() != symbol_id) return 0;
	index_row_ = get_center_row();
	SetCenterValues(true);

	return 1;
}

LRESULT SymbolOrderView::OnWmQuoteReceived(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = static_cast<int>(wParam);
	if (!symbol_ || symbol_->Id() != symbol_id) return 0;

	update_quote();

	return 1;
}

LRESULT SymbolOrderView::OnWmHogaReceived(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = static_cast<int>(wParam);
	if (!symbol_ || symbol_->Id() != symbol_id) return 0;

	update_hoga();

	return 1;
}

LRESULT SymbolOrderView::OnUmOrderChanged(WPARAM wParam, LPARAM lParam)
{
	Invalidate(FALSE);
	return 1;
}



void SymbolOrderView::OnTimer(UINT_PTR nIDEvent)
{
	bool needDraw = false;
	if (_EnableQuoteShow && symbol_) {
		ClearOldQuote();
		update_quote();
		needDraw = true;
		_EnableQuoteShow = false;
	}
	if (_EnableHogaShow && symbol_) {
		ClearOldHoga();
		update_hoga();
		needDraw = true;
		_EnableHogaShow = false;
	}

	if (_EnableOrderShow && symbol_) {
		ClearOrders();
		draw_order();
		ClearOldQuote();
		update_quote();
		needDraw = true;
		_EnableOrderShow = false;
	}

	if (enable_sell_stop_order_show_) {
		clear_sell_stop_order();
		update_sell_stop_order();
		needDraw = true;
		enable_sell_stop_order_show_ = false;
	}

	if (enable_buy_stop_order_show_) {
		clear_buy_stop_order();
		update_buy_stop_order();
		needDraw = true;
		enable_buy_stop_order_show_ = false;
	}

	if (enable_position_show_) {
		enable_position_show_ = false;
		//update_position();
		needDraw = true;
	}
	update_position();
	if (needDraw) Invalidate(FALSE);

	CBCGPStatic::OnTimer(nIDEvent);
}
