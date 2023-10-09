#include "stdafx.h"
#include "SmFundOrderArea.h"
#include "../SmGrid/SmGrid.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmOrderGridConst.h"
#include <math.h>
#include "../Log/MyLogger.h"
#include "../SmGrid/SmCell.h"
#include "../Order/SmOrderRequestManager.h"
#include "../Order/SmStopOrderManager.h"
#include "../SmGrid/SmCellType.h"
#include "../Order/AbAccountOrderCenterWindow.h"
#include <windows.h>
#include "../Global/SmTotalManager.h"
#include "../Client/ViStockClient.h"
#include "../Yuanta/YaStockClient.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Order/SmSymbolOrderManager.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Order/SmOrder.h"
#include "../Order/SmOrderViewer.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/AbAccountOrderWindow.h"
#include "../Util/SmNumberFunc.h"
#include "../Event/SmCallbackManager.h"
#include "../Order/SmOrderRequest.h"
#include "SmFundOrderDialog.h"
#include "SmFundOrderCenterWnd.h"
#include "SmFund.h"
#include "SmFundManager.h"
#include <sstream>
#include <format>

#include <functional>

using namespace std;
using namespace std::placeholders;

constexpr int Round(float x) { return static_cast<int>(x + 0.5f); }

using namespace DarkHorse;

BEGIN_MESSAGE_MAP(SmFundOrderArea, CBCGPStatic)
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
	ON_MESSAGE(WM_SYMBOL_MASTER_RECEIVED, &SmFundOrderArea::OnWmSymbolMasterReceived)
	ON_MESSAGE(WM_QUOTE_RECEIVED, &SmFundOrderArea::OnWmQuoteReceived)
	ON_MESSAGE(WM_HOGA_RECEIVED, &SmFundOrderArea::OnWmHogaReceived)
	ON_MESSAGE(WM_ORDER_CHANGED, &SmFundOrderArea::OnUmOrderChanged)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL SmFundOrderArea::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

	IncreaseCloseRow(distance);

	if (_Symbol) SetCenterValues(_Symbol);

	SetQuote(_Symbol);
	SetHoga(_Symbol);
	SetOrder();
	SetStopOrder();

	//Invalidate();

	return CBCGPStatic::OnMouseWheel(nFlags, zDelta, pt);
}

void SmFundOrderArea::Clear()
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

SmFundOrderArea::SmFundOrderArea()
{
	_BuyStopOrderMgr = std::make_shared<DarkHorse::SmStopOrderManager>();
	_SellStopOrderMgr = std::make_shared<DarkHorse::SmStopOrderManager>();
	//_BuyOrderViewer = std::make_shared<SmOrderViewer>();
	//_SellOrderViewer = std::make_shared<SmOrderViewer>();
	m_pGM = CBCGPGraphicsManager::CreateInstance();

}

SmFundOrderArea::~SmFundOrderArea()
{
	KillTimer(1);
	mainApp.CallbackMgr()->UnsubscribeOrderWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->UnsubscribeSymbolMasterWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->UnsubscribeQuoteWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->UnsubscribeHogaWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->UnsubscribeQuoteCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeHogaCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeMasterCallback((long)this);
	mainApp.CallbackMgr()->UnsubscribeOrderCallback((long)this);
	if (m_pGM != NULL)
	{
		delete m_pGM;
	}
}

void SmFundOrderArea::SetQuote(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol) return;

	_Symbol = symbol;

	if (!_CenterValued) SetCenterValues(symbol, false);

	const int close_row = FindRow(_Symbol->Qoute.close);

	std::shared_ptr<SmCell> cell = _Grid->FindCell(close_row, DarkHorse::OrderHeader::QUOTE);
	std::string value_string;
	if (close_row >= 2 && cell) {
		cell->CellType(SmCellType::CT_QUOTE_CLOSE);
		value_string = std::format("{0}", symbol->Qoute.close);
		if (symbol->decimal() > 0)
			value_string.insert(value_string.length() - symbol->decimal(), 1, '.');
		cell->Text(value_string);
		_QuoteValueMap.insert(std::make_pair(cell->Row(), cell->Col()));
	}

	const int open_row = FindRow(_Symbol->Qoute.open);
	cell = _Grid->FindCell(open_row, DarkHorse::OrderHeader::QUOTE);
	if (open_row >= 2 && cell) {
		cell->CellType(SmCellType::CT_QUOTE_OPEN);
		value_string = std::format("{0}", symbol->Qoute.open);
		if (symbol->decimal() > 0)
			value_string.insert(value_string.length() - symbol->decimal(), 1, '.');
		cell->Text(value_string);
		_QuoteValueMap.insert(std::make_pair(cell->Row(), cell->Col()));
	}

	const int high_row = FindRow(_Symbol->Qoute.high);
	cell = _Grid->FindCell(high_row, DarkHorse::OrderHeader::QUOTE);
	if (high_row >= 2 && cell) {
		cell->CellType(SmCellType::CT_QUOTE_HIGH);
		value_string = std::format("{0}", symbol->Qoute.high);
		if (symbol->decimal() > 0)
			value_string.insert(value_string.length() - symbol->decimal(), 1, '.');
		cell->Text(value_string);
		_QuoteValueMap.insert(std::make_pair(cell->Row(), cell->Col()));
	}

	const int low_row = FindRow(_Symbol->Qoute.low);
	cell = _Grid->FindCell(low_row, DarkHorse::OrderHeader::QUOTE);
	if (low_row >= 2 && cell) {
		cell->CellType(SmCellType::CT_QUOTE_LOW);
		value_string = std::format("{0}", symbol->Qoute.low);
		if (symbol->decimal() > 0)
			value_string.insert(value_string.length() - symbol->decimal(), 1, '.');
		cell->Text(value_string);
		_QuoteValueMap.insert(std::make_pair(cell->Row(), cell->Col()));
	}

	SetPosition();
	SetQuoteColor(symbol);
	Invalidate();
}

void SmFundOrderArea::SetHoga(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	if (!symbol || _QuoteToRowIndexMap.size() == 0) return;

	try {
		std::shared_ptr<SmCell> pCell = nullptr;

		for (int i = 0; i < 5; i++) {
			int row_index = FindRow(symbol->Hoga.Ary[i].SellPrice);
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::SELL_CNT);
			if (pCell && row_index > 1 && row_index < (_Grid->RowCount() - 2)) {
				pCell->Text(std::to_string(symbol->Hoga.Ary[i].SellCnt));
				pCell->CellType(SmCellType::CT_HOGA_SELL);
				_OldHogaSellRowIndex.insert(row_index);
			}
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::SELL_QTY);
			if (pCell && row_index > 1 && row_index < (_Grid->RowCount() - 2)) {
				pCell->CellType(SmCellType::CT_HOGA_SELL);
				pCell->Text(std::to_string(symbol->Hoga.Ary[i].SellQty));
			}
			row_index = FindRow(symbol->Hoga.Ary[i].BuyPrice);
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::BUY_QTY);
			if (pCell && row_index > 1 && row_index < (_Grid->RowCount() - 2)) {
				pCell->CellType(SmCellType::CT_HOGA_BUY);
				pCell->Text(std::to_string(symbol->Hoga.Ary[i].BuyQty));
				_OldHogaBuyRowIndex.insert(row_index);
			}
			pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::BUY_CNT);
			if (pCell && row_index > 1 && row_index < (_Grid->RowCount() - 2)) {
				pCell->CellType(SmCellType::CT_HOGA_BUY);
				pCell->Text(std::to_string(symbol->Hoga.Ary[i].BuyCnt));
			}
		}

		pCell = _Grid->FindCell(_Grid->RowCount() - 2, DarkHorse::OrderHeader::SELL_CNT);
		if (pCell) pCell->Text(std::to_string(symbol->Hoga.TotSellCnt));
		pCell = _Grid->FindCell(_Grid->RowCount() - 2, DarkHorse::OrderHeader::SELL_QTY);
		if (pCell) pCell->Text(std::to_string(symbol->Hoga.TotSellQty));
		pCell = _Grid->FindCell(_Grid->RowCount() - 2, DarkHorse::OrderHeader::BUY_CNT);
		if (pCell) pCell->Text(std::to_string(symbol->Hoga.TotBuyCnt));
		pCell = _Grid->FindCell(_Grid->RowCount() - 2, DarkHorse::OrderHeader::BUY_QTY);
		if (pCell) pCell->Text(std::to_string(symbol->Hoga.TotBuyQty));

		const int delta_hoga = symbol->Hoga.TotBuyQty = symbol->Hoga.TotSellQty;
		pCell = _Grid->FindCell(_Grid->RowCount() - 2, DarkHorse::OrderHeader::QUOTE);
		if (pCell) pCell->Text(std::to_string(delta_hoga));

		_TotalHogaMap.insert(std::make_pair(_Grid->RowCount() - 2, DarkHorse::OrderHeader::SELL_CNT));
		_TotalHogaMap.insert(std::make_pair(_Grid->RowCount() - 2, DarkHorse::OrderHeader::SELL_QTY));
		_TotalHogaMap.insert(std::make_pair(_Grid->RowCount() - 2, DarkHorse::OrderHeader::QUOTE));
		_TotalHogaMap.insert(std::make_pair(_Grid->RowCount() - 2, DarkHorse::OrderHeader::BUY_CNT));
		_TotalHogaMap.insert(std::make_pair(_Grid->RowCount() - 2, DarkHorse::OrderHeader::BUY_QTY));

	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmFundOrderArea::SetPosition()
{
	if (!_Fund || !_Symbol) return;
	int fund_avg_price = 0, fund_open_qty = 0;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		std::shared_ptr<SmPosition> position = mainApp.TotalPosiMgr()->FindAddPosition(account->No(), _Symbol->SymbolCode());
		fund_avg_price += static_cast<int>(position->AvgPrice);
		fund_open_qty += position->OpenQty;
	}
	if (fund_open_qty == 0) return;
	
	
	fund_avg_price = static_cast<int>(fund_avg_price / account_vec.size());
	const int int_tick_size = static_cast<int>(_Symbol->TickSize() * pow(10, _Symbol->decimal()));
	const int mod = fund_avg_price % int_tick_size;
	const int half_tick_size = (int)(int_tick_size / 2);
	// 나머지 보다 작으면 빼주고 아니면 틱크기에서 나머지를 뺀 값을 더해 준다.
	if (mod < half_tick_size) fund_avg_price = fund_avg_price - fund_avg_price % int_tick_size;
	else fund_avg_price += (int_tick_size - mod);

	const int close_row = FindRow(fund_avg_price);

	std::shared_ptr<SmCell> cell = _Grid->FindCell(close_row, DarkHorse::OrderHeader::QUOTE);
	if (close_row >= 2 && cell) {
		if (fund_open_qty > 0)
			cell->CellType(SmCellType::CT_POSITION_BUY);
		else if (fund_open_qty < 0)
			cell->CellType(SmCellType::CT_POSITION_SELL);
		_QuoteValueMap.insert(std::make_pair(cell->Row(), cell->Col()));
	}
}

void SmFundOrderArea::SetStopOrder()
{
	if (!_Fund || !_Symbol) return;

	int sell_stop_count = 0, buy_stop_count = 0;

	std::shared_ptr<DarkHorse::SmSymbolOrderManager> symbol_order_mgr = nullptr;

	_StopRectVector.clear();

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);

		symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), _Symbol->SymbolCode());

		const std::map<int, std::shared_ptr<DarkHorse::SmOrderRequest>>& sell_order_req_map = symbol_order_mgr->SellStopOrderMgr()->GetOrderReqMap();


		for (auto it = sell_order_req_map.begin(); it != sell_order_req_map.end(); ++it) {
			int row_index = FindRow(it->second->OrderPrice);
			sell_stop_count += it->second->OrderAmount;
			std::shared_ptr<SmCell> pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::SELL_STOP);
			if (pCell && row_index > 1 && row_index < (_Grid->RowCount() - 2)) {
				pCell->AddOrderReq(it->second);
				pCell->Text(std::to_string(pCell->GetOrderReqCount()));
				_OldStopSellRowIndex.insert(row_index);

				std::shared_ptr<SmCell> pOrderCell = _Grid->FindCell(row_index + it->second->CutSlip, DarkHorse::OrderHeader::SELL_ORDER);
				if (pOrderCell && pOrderCell->Row() > 1 && pOrderCell->Row() < _Grid->RowCount() - 2) {
					_StopRectVector.push_back(std::make_pair(pCell->GetCellRect(), pOrderCell->GetCellRect()));
				}
			}
		}

		

		const std::map<int, std::shared_ptr<SmOrderRequest>>& buy_order_req_map = symbol_order_mgr->BuyStopOrderMgr()->GetOrderReqMap();
		for (auto it = buy_order_req_map.begin(); it != buy_order_req_map.end(); ++it) {
			int row_index = FindRow(it->second->OrderPrice);
			buy_stop_count += it->second->OrderAmount;
			std::shared_ptr<SmCell> pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::BUY_STOP);
			if (pCell && row_index > 1 && row_index < (_Grid->RowCount() - 2)) {
				pCell->AddOrderReq(it->second);
				pCell->Text(std::to_string(pCell->GetOrderReqCount()));
				_OldStopBuyRowIndex.insert(row_index);

				std::shared_ptr<SmCell> pOrderCell = _Grid->FindCell(row_index - it->second->CutSlip, DarkHorse::OrderHeader::BUY_ORDER);
				if (pOrderCell && pOrderCell->Row() > 1 && pOrderCell->Row() < _Grid->RowCount() - 2) {
					_StopRectVector.push_back(std::make_pair(pCell->GetCellRect(), pOrderCell->GetCellRect()));
				}
			}
		}

		
	}


	std::shared_ptr<SmCell> order_cell = _Grid->FindCell(1, DarkHorse::OrderHeader::SELL_STOP);
	if (order_cell) order_cell->Text(std::to_string(sell_stop_count));
	
	order_cell = _Grid->FindCell(1, DarkHorse::OrderHeader::BUY_STOP);
	if (order_cell) order_cell->Text(std::to_string(buy_stop_count));
}

void SmFundOrderArea::SetOrder()
{
	if (!_Fund || !_Symbol) return;
	if (_MovingOrder) return;

	std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = nullptr;
	int buy_count = 0, sell_count = 0;
	std::shared_ptr<SmCell> cell = nullptr;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), _Symbol->SymbolCode());
		const std::map<std::string, std::shared_ptr<SmOrder>>& accepted_order_map = symbol_order_mgr->GetAcceptedOrders();
		for (auto it = accepted_order_map.begin(); it != accepted_order_map.end(); ++it) {
			auto order = it->second;

			if (order->PositionType == SmPositionType::Buy) buy_count++;
			else sell_count++;

			int row_index = FindRow(order->OrderPrice);
			if (row_index > 1 && row_index < (_Grid->RowCount() - 2)) {
				if (order->PositionType == SmPositionType::Buy) {
					cell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::BUY_ORDER);
					if (cell) {
						cell->AddOrder(order);
						cell->Text(std::to_string(cell->GetOrderCount()));
						_OldOrderBuyRowIndex.insert(row_index);
					}
					//buy_count++;
				}
				else {
					cell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::SELL_ORDER);
					if (cell) {
						cell->AddOrder(order);
						cell->Text(std::to_string(cell->GetOrderCount()));
						_OldOrderSellRowIndex.insert(row_index);
					}
					//sell_count++;
				}
			}

		}
	}

	

	cell = _Grid->FindCell(1, DarkHorse::OrderHeader::BUY_ORDER);
	if (cell) cell->Text(std::to_string(buy_count));
	cell = _Grid->FindCell(1, DarkHorse::OrderHeader::SELL_ORDER);
	if (cell) cell->Text(std::to_string(sell_count));
}

void SmFundOrderArea::ClearOldHoga(DarkHorse::Hoga_Type hoga_type) const noexcept
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

void SmFundOrderArea::ClearOldHoga()
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

void SmFundOrderArea::ClearOldQuote()
{
	std::shared_ptr<SmCell> pCell = nullptr;
	for (auto it = _QuoteValueMap.begin(); it != _QuoteValueMap.end(); ++it) {
		pCell = _Grid->FindCell(it->first, it->second);
		if (pCell) {
			pCell->CellType(SmCellType::CT_NORMAL);
		}
	}
	_QuoteValueMap.clear();
}

void SmFundOrderArea::ClearStopOrders()
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

void SmFundOrderArea::ClearOrders()
{
	if (_MovingOrder) return;

	std::shared_ptr<SmCell> pCell = nullptr;
	for (auto it = _OldOrderSellRowIndex.begin(); it != _OldOrderSellRowIndex.end(); ++it) {
		pCell = _Grid->FindCell(*it, DarkHorse::OrderHeader::SELL_ORDER);
		if (pCell) {
			pCell->ClearOrder();
			pCell->Text("");
		}
	}
	_OldOrderSellRowIndex.clear();

	for (auto it = _OldOrderBuyRowIndex.begin(); it != _OldOrderBuyRowIndex.end(); ++it) {
		pCell = _Grid->FindCell(*it, DarkHorse::OrderHeader::BUY_ORDER);
		if (pCell) {
			pCell->ClearOrder();
			pCell->Text("");
		}
	}
	_OldOrderBuyRowIndex.clear();
}

int SmFundOrderArea::FindRow(const int& value) const noexcept
{
	int result = -1;
	auto it = _QuoteToRowIndexMap.find(value);
	if (it != _QuoteToRowIndexMap.end())
		return it->second;
	else
		return -1;
}

int SmFundOrderArea::FindValue(const int& row) const noexcept
{
	int result = -1;
	auto it = _RowIndexToPriceMap.find(row);
	if (it != _RowIndexToPriceMap.end())
		return it->second;
	else
		return -1;
}

void SmFundOrderArea::Refresh()
{
	_EnableOrderShow = true;
	_EnableQuoteShow = true;
	_EnableStopShow = true;
	_EnableHogaShow = true;
	ClearOldQuote();
	ClearOldHoga();
	ClearStopOrders();
	ClearOrders();
	SetQuote(_Symbol);
	SetHoga(_Symbol);
	SetOrder();
	SetStopOrder();
	Invalidate(FALSE);
}

void SmFundOrderArea::DrawHogaLine(const CRect& rect)
{
	if (!_Symbol) return;

	int row_index = FindRow(_Symbol->Hoga.Ary[0].SellPrice);
	auto pCell = _Grid->FindCell(row_index, DarkHorse::OrderHeader::SELL_CNT);
	if (pCell && row_index > 1) {
		m_pGM->DrawLine(0, pCell->Y() + pCell->Height() + 1, rect.Width(), pCell->Y() + pCell->Height() + 1, _Resource.SelectedBrush);
	}
}

void SmFundOrderArea::DrawFixedSelectedCell()
{
	const int row_index = FindRow(_SelectedValue.first);
	auto pCell = _Grid->FindCell(row_index, _SelectedValue.second);
	if (pCell) {
		_Grid->DrawCell(pCell, m_pGM, _Resource.SelectedBrush, _Resource.SelectedBrush, _Resource.TextBrush, _Resource.TextFormat, false, true, true);
	}
}

void SmFundOrderArea::DrawMovingOrder()
{
	if (_DraggingOrder) {

		auto start_cell = _Grid->FindCellByPos(_StartX, _StartY);
		if (!start_cell) return;
		if (start_cell->Col() == DarkHorse::OrderHeader::BUY_ORDER ||
			start_cell->Col() == DarkHorse::OrderHeader::SELL_ORDER ||
			start_cell->Col() == DarkHorse::OrderHeader::SELL_STOP ||
			start_cell->Col() == DarkHorse::OrderHeader::BUY_STOP) {

			const double stroke_width = 2.0f;
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
			m_pGM->DrawLine(start_point, end_point, _Resource.SelectedBrush, stroke_width);



			CBCGPPointsArray arrow_array;

			const int head_length = 12, head_width = 12;

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
	}
}

void SmFundOrderArea::SetUp()
{
	CRect rect;
	GetClientRect(rect);

	//rect.right -= 1;
	rect.bottom -= 1;

	CreateResource();


	_Grid = std::make_shared<DarkHorse::SmGrid>(_Resource, 60, 9);

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

		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_STOP].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_ORDER].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_CNT].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_QTY].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::QUOTE].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_QTY].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_CNT].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_ORDER].title);
		_OrderTableHeader.push_back(OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_STOP].title);

		_Grid->SetColHeaderTitles(_OrderTableHeader);
	}

	_Grid->RegisterOrderButtons(_ButtonMap);


	mainApp.CallbackMgr()->SubscribeSymbolMasterWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->SubscribeQuoteWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->SubscribeHogaWndCallback(GetSafeHwnd());
	mainApp.CallbackMgr()->SubscribeOrderWndCallback(GetSafeHwnd());

	mainApp.CallbackMgr()->SubscribeQuoteCallback((long)this, std::bind(&SmFundOrderArea::OnQuoteEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeHogaCallback((long)this, std::bind(&SmFundOrderArea::OnHogaEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeMasterCallback((long)this, std::bind(&SmFundOrderArea::OnSymbolMasterEvent, this, _1));
	mainApp.CallbackMgr()->SubscribeOrderCallback((long)this, std::bind(&SmFundOrderArea::OnOrderEvent, this, _1, _2));

	SetTimer(1, 40, NULL);

	return;
}


int SmFundOrderArea::RecalRowCount(const int& height)
{
	CRect rect;

	GetWindowRect(rect);

	//rect.right -= 1;
	rect.bottom -= 1;

	ClearOldQuote();
	ClearOldHoga();
	_Grid->ReleaseOrderButtons(_ButtonMap);
	const int delta_height = _Grid->RecalRowCount(height, false);



	_Grid->CreateGrids();

	_Grid->RegisterOrderButtons(_ButtonMap);

	if (_Symbol) {
		SetCenterValues(_Symbol);
		SetHoga(_Symbol);
		SetQuote(_Symbol);
	}

	//Invalidate(FALSE);

	AbAccountOrderCenterWindow::DeltaOrderArea = delta_height;

	return delta_height;
}

void SmFundOrderArea::Fund(std::shared_ptr<DarkHorse::SmFund> val)
{
	_Fund = val;
	/*
	_AccountPwdMap.clear();
	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		_AccountPwdMap[account->No()] = account->Pwd();
	}
	*/
}

void SmFundOrderArea::UpdateOrder(const std::string& symbol_code)
{
	if (!_Fund || !_Symbol) return;

	if (_Symbol->SymbolCode().compare(symbol_code) != 0) return;

	//std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());

	//symbol_order_mgr->UpdateOrder(_BuyOrderViewer, _SellOrderViewer);

	//ClearOrders();

	//SetOrder();

	//Invalidate();
}

void SmFundOrderArea::ConnectOrderUpdateSignal()
{
	if (!_Fund || !_Symbol) return;

	//std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(_Account->No(), _Symbol->SymbolCode());
	//symbol_order_mgr->order_update.Connect(this, &SmFundOrderArea::UpdateOrder);
}

void SmFundOrderArea::ArrangeCenterValue()
{
	ClearOldQuote();
	ClearOldHoga();
	ClearOrders();

	_CloseRow = static_cast<int>(_Grid->RowCount() / 2) - 2;

	if (_Symbol) SetCenterValues(_Symbol, true);

	SetQuote(_Symbol);
	SetHoga(_Symbol);
	SetOrder();
}

void SmFundOrderArea::BuyByMarketPrice()
{
	PutOrder(SmPositionType::Buy, 0, SmPriceType::Market);
}

void SmFundOrderArea::SellByMarketPrice()
{
	PutOrder(SmPositionType::Sell, 0, SmPriceType::Market);
}

void SmFundOrderArea::CancelSellStop()
{
	if (!_Fund || !_Symbol) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);

		std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), _Symbol->SymbolCode());

		symbol_order_mgr->SellStopOrderMgr()->Clear();
	}
	//Invalidate();
	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SmFundOrderArea::CancelSellOrder()
{
	if (!_Fund || !_Symbol) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), _Symbol->SymbolCode());

		const std::map<int, std::map<std::string, std::shared_ptr<SmOrder>>>& sell_order_map = symbol_order_mgr->SellOrderViewer()->GetOrderMap();
		for (auto it = sell_order_map.begin(); it != sell_order_map.end(); ++it) {
			const auto& order_vec = it->second;
			for (auto iv = order_vec.begin(); iv != order_vec.end(); iv++) {
				auto order_req = SmOrderRequestManager::MakeCancelOrderRequest(
					account->No(),
					account->Pwd(), _Symbol->SymbolCode(),
					iv->second->OrderNo, iv->second->OrderPrice, iv->second->PositionType,
					iv->second->OrderAmount);
				mainApp.Client()->CancelOrder(order_req);
			}
		}
	}
	//Invalidate();
	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SmFundOrderArea::CancelAllOrder()
{
	CancelBuyStop();
	CancelSellStop();
	CancelSellOrder();
	CancelBuyOrder();
}

void SmFundOrderArea::CancelBuyStop()
{
	if (!_Fund || !_Symbol) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), _Symbol->SymbolCode());

		symbol_order_mgr->BuyStopOrderMgr()->Clear();
	}
	//Invalidate();
	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SmFundOrderArea::CancelBuyOrder()
{
	if (!_Fund || !_Symbol) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);

		std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), _Symbol->SymbolCode());

		const std::map<int, std::map<std::string, std::shared_ptr<SmOrder>>>& sell_order_map = symbol_order_mgr->BuyOrderViewer()->GetOrderMap();
		for (auto it = sell_order_map.begin(); it != sell_order_map.end(); ++it) {
			const auto& order_vec = it->second;
			for (auto iv = order_vec.begin(); iv != order_vec.end(); iv++) {
				auto order_req = SmOrderRequestManager::MakeCancelOrderRequest(
					account->No(),
					account->Pwd(), _Symbol->SymbolCode(),
					iv->second->OrderNo, iv->second->OrderPrice, iv->second->PositionType,
					iv->second->OrderAmount);
				mainApp.Client()->CancelOrder(order_req);
			}
		}
	}
	//Invalidate();
	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SmFundOrderArea::ProcesButtonClick(const std::shared_ptr<SmCell>& cell)
{
	if (!cell) return;

	auto found = _ButtonMap.find(cell);
	if (found == _ButtonMap.end()) return;

	ProcessButtonMsg(found->second);
}

bool SmFundOrderArea::ProcesButtonClickByPos(const CPoint& point)
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

std::pair<int, int> SmFundOrderArea::GetOrderCount(const std::shared_ptr<SmCell>& cell)
{
	auto found = _RowIndexToPriceMap.find(cell->Row());
	if (found == _RowIndexToPriceMap.end()) return std::make_pair(0, 0);
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

void SmFundOrderArea::ResetHeaderWidth(const int& wnd_width)
{
	int width_sum = 0;
	_Grid->SetColWidth(DarkHorse::OrderHeader::SELL_STOP, OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_STOP].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::SELL_ORDER, OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_ORDER].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::SELL_CNT, OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_CNT].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::SELL_QTY, OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_QTY].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::QUOTE, OrderGridHeaderVector[DarkHorse::OrderHeader::QUOTE].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::BUY_QTY, OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_QTY].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::BUY_CNT, OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_CNT].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::BUY_ORDER, OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_ORDER].width);
	_Grid->SetColWidth(DarkHorse::OrderHeader::BUY_STOP, OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_STOP].width);


	width_sum += OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_STOP].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_ORDER].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_CNT].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderHeader::SELL_QTY].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_QTY].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_CNT].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_ORDER].width;
	width_sum += 1;
	width_sum += OrderGridHeaderVector[DarkHorse::OrderHeader::BUY_STOP].width;
	width_sum += 1;
	width_sum += 1;
	width_sum += 2;

	_Grid->SetColWidth(DarkHorse::OrderHeader::QUOTE, wnd_width - width_sum);
}

void SmFundOrderArea::SetCenterValues(std::shared_ptr<DarkHorse::SmSymbol> symbol, const bool& make_row_map)
{
	if (!_Symbol) return;

	if (_SettingCenterValue) return;
	_SettingCenterValue = true;

	const int& close = _Symbol->Qoute.close;
	const int int_tick_size = static_cast<int>(_Symbol->TickSize() * pow(10, _Symbol->decimal()));
	const int start_value = close + (_CloseRow - _ValueStartRow) * int_tick_size;
	try {
		if (make_row_map) {
			_QuoteToRowIndexMap.clear();
			_RowIndexToPriceMap.clear();
		}
		int value = start_value;
		for (int i = 2; i < _Grid->RowCount() - 2; i++) {
			std::string value_string;
			//std::stringstream ss;
			//ss.imbue(std::locale(std::locale::classic(), new SmNumPunct));
			//ss << value;
			//value_string = ss.str();
			value_string = std::format("{0}", value);
			if (_Symbol->decimal() > 0)
				value_string.insert(value_string.length() - _Symbol->decimal(), 1, '.');
			_Grid->SetCellText(i, DarkHorse::OrderHeader::QUOTE, value_string);

			_QuoteToRowIndexMap[value] = i;
			_RowIndexToPriceMap[i] = value;
			value -= int_tick_size;
		}
		SetQuoteColor(_Symbol);
		_CenterValued = true;
	}
	catch (const std::exception& e) {
		const std::string& error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}

	_SettingCenterValue = false;
}

void SmFundOrderArea::PutOrderBySpaceBar()
{
	if (!_Fund || !_Symbol) return;

	if (!_FixedSelectedCell) return;

	const int price = FindValue(_FixedSelectedCell->Row());

	if (_FixedSelectedCell->Col() == DarkHorse::OrderHeader::SELL_ORDER)
		PutOrder(SmPositionType::Sell, price);
	else if (_FixedSelectedCell->Col() == DarkHorse::OrderHeader::BUY_ORDER)
		PutOrder(SmPositionType::Buy, price);
	else if (_FixedSelectedCell->Col() == DarkHorse::OrderHeader::BUY_STOP)
		PutStopOrder(SmPositionType::Buy, price);
	else if (_FixedSelectedCell->Col() == DarkHorse::OrderHeader::SELL_STOP)
		PutStopOrder(SmPositionType::Sell, price);

	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SmFundOrderArea::ChangeOrderByKey(const int up_down)
{
	if (!_Fund || !_Symbol) return;
	Invalidate();
}

void SmFundOrderArea::OnOrderChanged(const int& account_id, const int& symbol_id)
{
	//if (!_Account || !_Symbol) return;
	//if (_Account->Id() != account_id || _Symbol->Id() != symbol_id) return;

	//UpdateOrder(_Symbol->SymbolCode());
	//Invalidate();
}

void SmFundOrderArea::OnOrderEvent(const std::string& account_no, const std::string& symbol_code)
{
	if (!_Symbol) return;
	//if (_Account->No() != account_no) return;
	if (_Symbol->SymbolCode() != symbol_code) return;

	_EnableOrderShow = true;
	_EnableStopShow = true;
}

void SmFundOrderArea::OnQuoteEvent(const std::string& symbol_code)
{
	if (!_Symbol) return;
	if (_Symbol->SymbolCode() != symbol_code) return;
	if (_FixedMode) {
		ProcessFixedMode();
		return;
	}
	_EnableQuoteShow = true;
}

void SmFundOrderArea::OnHogaEvent(const std::string& symbol_code)
{
	if (!_Symbol) return;
	if (_Symbol->SymbolCode() != symbol_code) return;
	_EnableHogaShow = true;
}

void SmFundOrderArea::OnSymbolMasterEvent(const std::string& symbol_code)
{
	if (!_Symbol) return;
	if (_Symbol->SymbolCode() != symbol_code) return;
}

void SmFundOrderArea::UpdateOrderSettings(SmOrderSettings settings)
{
	_OrderSettings = settings;
}

void SmFundOrderArea::FixedMode(bool val)
{
	_FixedMode = val;
	ArrangeCenterValue();
	Refresh();
}

void SmFundOrderArea::Symbol(std::shared_ptr<DarkHorse::SmSymbol> val)
{
	_Symbol = val;
	ArrangeCenterValue();
	Invalidate();
}

void SmFundOrderArea::SetAllRowHeight(const int& row_height)
{
	_Grid->SetAllRowHeight(row_height);
}

void SmFundOrderArea::DrawStopOrder()
{
	for (size_t i = 0; i < _StopRectVector.size(); ++i) {
		CBCGPPoint start_point = _StopRectVector[i].first.CenterPoint();
		CBCGPPoint end_point = _StopRectVector[i].second.CenterPoint();
		DrawArrow(start_point, end_point, 1.0f, 6);
	}
}

void SmFundOrderArea::DrawArrow(const CBCGPPoint& start_point, const CBCGPPoint& end_point, const double& stroke_width, const int& head_width)
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

void SmFundOrderArea::ProcessFixedMode()
{
	_CloseRow = static_cast<int>(_Grid->RowCount() / 2) - 2;

	SetCenterValues(_Symbol, false);

	_EnableOrderShow = true;
	_EnableQuoteShow = true;
	_EnableHogaShow = true;
	_EnableStopShow = true;
}

int SmFundOrderArea::FindRowFromCenterValue(std::shared_ptr<DarkHorse::SmSymbol> symbol, const int& value)
{
	if (!symbol || _QuoteToRowIndexMap.empty())
		return -1;

	const int int_tick_size = static_cast<int>(symbol->TickSize() * pow(10, symbol->decimal()));
	auto it = _QuoteToRowIndexMap.find(value);
	if (it != _QuoteToRowIndexMap.end()) { // 값이 보이는 범위 안에 있을 때
		return it->second;
	}
	else { // 값이 보이는 범위 밖에 있을 때
		auto itr = _QuoteToRowIndexMap.rbegin();
		int big_val = itr->first;
		int big_row = itr->second;


		int delta = big_val - value;
		int delta_row = delta / int_tick_size;

		int target_row = big_row + delta_row;
		return target_row;
	}
}

void SmFundOrderArea::SetQuoteColor(std::shared_ptr<DarkHorse::SmSymbol> symbol)
{
	int lowRow = FindRowFromCenterValue(symbol, symbol->Qoute.low);
	int highRow = FindRowFromCenterValue(symbol, symbol->Qoute.high);
	int closeRow = FindRowFromCenterValue(symbol, symbol->Qoute.close);
	int openRow = FindRowFromCenterValue(symbol, symbol->Qoute.open);
	//int preCloseRow = FindRowFromCenterValue(symbol, symbol->Qoute.preclose);

	if (symbol->Qoute.close > symbol->Qoute.open) { // 양봉
		for (auto it = _QuoteToRowIndexMap.rbegin(); it != _QuoteToRowIndexMap.rend(); ++it) {
			if (it->second < highRow) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(242, 242, 242));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(0); }
			}
			else if (it->second < closeRow) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(255, 255, 255));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(3); }
			}
			else if (it->second <= openRow) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(252, 226, 228));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(1); }
			}
			else if (it->second < lowRow + 1) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(255, 255, 255));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(3); }
			}
			else {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(242, 242, 242));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(0); }
			}
		}

	}
	else if (symbol->Qoute.close < symbol->Qoute.open) { // 음봉
		for (auto it = _QuoteToRowIndexMap.rbegin(); it != _QuoteToRowIndexMap.rend(); ++it) {
			if (it->second < highRow) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(242, 242, 242));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(0); }
			}
			else if (it->second < openRow) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(255, 255, 255));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(3); }
			}
			else if (it->second <= closeRow) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(218, 226, 245));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(2); }
			}
			else if (it->second < lowRow + 1) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(255, 255, 255));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(3); }
			}
			else {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(242, 242, 242));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(0); }
			}
		}
	}
	else { // 도지
		for (auto it = _QuoteToRowIndexMap.rbegin(); it != _QuoteToRowIndexMap.rend(); ++it) {
			if (it->second < highRow) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(242, 242, 242));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(0); }
			}
			else if (it->second < closeRow) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(255, 255, 255));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(3); }
			}
			else if (it->second <= openRow) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(252, 226, 228));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(3); }
			}
			else if (it->second < lowRow + 1) {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(255, 255, 255));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(3); }
			}
			else {
				//CGridCellBase* pCell = GetCell(it->second, CenterCol);
				//pCell->SetBackClr(RGB(242, 242, 242));
				auto cell = _Grid->FindCell(it->second, DarkHorse::OrderHeader::QUOTE);
				if (cell) { cell->ShowMark(true); cell->MarkType(0); }
			}
		}
	}
}

void SmFundOrderArea::PutFilledOrder(const DarkHorse::SmPositionType& type, const int& price, const DarkHorse::SmPriceType& price_type)
{
	if (!_Fund || !_Symbol) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	if (account_vec.empty()) return;

	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), _Symbol->SymbolCode());
		int filled_count = symbol_order_mgr->GetUnsettledCount();
		std::shared_ptr<SmOrderRequest> order_req = nullptr;
		if (filled_count > 0)
			order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), _Symbol->SymbolCode(), price, abs(filled_count), price_type);
		else if (filled_count < 0)
			order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), _Symbol->SymbolCode(), price, abs(filled_count), price_type);

		if (order_req) {
			order_req->CutMode = 0;
			order_req->CutSlip = _OrderSettings.SlipTick;
			mainApp.Client()->NewOrder(order_req);
		}

	}
}

void SmFundOrderArea::SetProfitLossCut(std::shared_ptr<SmOrderRequest> order_req)
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

void SmFundOrderArea::SetStopOrderCut(std::shared_ptr<SmOrderRequest> order_req)
{
	order_req->CutSlip = _OrderSettings.SlipTick;
}

void SmFundOrderArea::PutStopOrder(const DarkHorse::SmPositionType& type, const int& price)
{
	if (!_Fund || !_Symbol) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);

		const auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), _Symbol->SymbolCode());

		std::shared_ptr<SmOrderRequest> order_req = nullptr;
		if (type == SmPositionType::Sell) {
			order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), _Symbol->SymbolCode(), price, _OrderAmount * account->SeungSu());
			if (order_req) {
				SetStopOrderCut(order_req);
				symbol_order_mgr->SellStopOrderMgr()->AddOrderRequest(order_req);
			}
		}
		else {
			order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), _Symbol->SymbolCode(), price, _OrderAmount * account->SeungSu());
			if (order_req) {
				SetStopOrderCut(order_req);
				symbol_order_mgr->BuyStopOrderMgr()->AddOrderRequest(order_req);
			}
		}
	}
}

void SmFundOrderArea::PutOrder(const SmPositionType& type, const int& price, const SmPriceType& price_type)
{
	if (!_Fund || !_Symbol) return;

	if (_EnableFilledOrder) {
		PutFilledOrder(type, price, price_type);
		return;
	}

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);
		std::shared_ptr<SmOrderRequest> order_req = nullptr;
		if (type == SmPositionType::Sell)
			order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), _Symbol->SymbolCode(), price, _OrderAmount * account->SeungSu(), price_type);
		else
			order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), _Symbol->SymbolCode(), price, _OrderAmount * account->SeungSu(), price_type);
		if (order_req) {
			SetProfitLossCut(order_req);
			mainApp.Client()->NewOrder(order_req);
		}
	}
}

void SmFundOrderArea::CreateResource()
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
	fmt2.SetTextVerticalAlignment(CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER);
	fmt2.SetFontWeight(FW_BOLD);
	fmt2.SetWordWrap();
	fmt2.SetClipText();

	_Resource.QuoteNumberFormat = fmt2;
}

void SmFundOrderArea::IncreaseCloseRow(const int& delta)
{
	_CloseRow += delta;
	if (!_Symbol) return;

	_Grid->index_row(_CloseRow);
}

void SmFundOrderArea::ClearOldSelectedCells()
{
	for (size_t i = 0; i < _SelectedCells.size(); i++) {
		auto cell = _SelectedCells[i];
		_Grid->DrawCell(cell, m_pGM, _Resource.GridNormalBrush, _Resource.GridNormalBrush, _Resource.TextBrush, _Resource.TextFormat, true, true, true);
	}
	_SelectedCells.clear();
}

void SmFundOrderArea::DrawMovingRect()
{
	if (_MouseMode == DarkHorse::MouseMode::Normal) {
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


void SmFundOrderArea::CancelOrder(const std::shared_ptr<DarkHorse::SmCell>& src_cell)
{
	if (!src_cell) return;

	if (src_cell->GetOrderCount() == 0) return;

	const std::map<int, std::shared_ptr<SmOrder>>& order_map = src_cell->GetOrderMap();
	for (auto it = order_map.begin(); it != order_map.end(); it++) {
		auto found = mainApp.AcntMgr()->FindAccount(it->second->AccountNo);
		if (!found) continue;

		auto order_req = SmOrderRequestManager::MakeChangeOrderRequest(
			it->second->AccountNo,
			found->Pwd(), _Symbol->SymbolCode(),
			it->second->OrderNo, it->second->OrderPrice, it->second->PositionType,
			it->second->OrderAmount);
		mainApp.Client()->CancelOrder(order_req);
	}

	src_cell->ClearOrder();
}

void SmFundOrderArea::ChangeOrder(const std::shared_ptr<DarkHorse::SmCell>& src_cell, const int& tgt_price)
{
	if (!src_cell) return;
	if (src_cell->GetOrderCount() == 0) return;

	const std::map<int, std::shared_ptr<SmOrder>>& order_map = src_cell->GetOrderMap();
	for (auto it = order_map.begin(); it != order_map.end(); it++) {
		auto found = mainApp.AcntMgr()->FindAccount(it->second->AccountNo);
		if (!found) continue;

		auto order_req = SmOrderRequestManager::MakeChangeOrderRequest(
			it->second->AccountNo,
			found->Pwd(), _Symbol->SymbolCode(),
			it->second->OrderNo, tgt_price, it->second->PositionType,
			it->second->OrderAmount);
		SetProfitLossCut(order_req);
		mainApp.Client()->ChangeOrder(order_req);
	}

	src_cell->ClearOrder();
}

void SmFundOrderArea::ChangeStop(const std::shared_ptr<DarkHorse::SmCell>& src_cell, const std::shared_ptr<DarkHorse::SmCell>& tgt_cell, const int& src_price, const int& tgt_price)
{
	if (!src_cell) return;

	if (src_cell->GetOrderReqCount() == 0) return;

	const std::map<int, std::shared_ptr<SmOrderRequest>>& order_req_map = src_cell->GetOrderReqMap();
	for (auto it = order_req_map.begin(); it != order_req_map.end(); ++it) {
		it->second->OrderPrice = tgt_price;
		tgt_cell->AddOrderReq(it->second);
	}

	src_cell->ClearOrderReq();
}


void SmFundOrderArea::CancelStop(const std::shared_ptr<DarkHorse::SmCell>& src_cell)
{
	if (!src_cell) return;

	if (!_Fund || !_Symbol) return;

	const std::vector<std::shared_ptr<SmAccount>>& account_vec = _Fund->GetAccountVector();
	for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
		auto account = (*it);

		std::shared_ptr<SmSymbolOrderManager> symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), _Symbol->SymbolCode());

		const std::map<int, std::shared_ptr<SmOrderRequest>>& order_req_map = src_cell->GetOrderReqMap();
		for (auto it = order_req_map.begin(); it != order_req_map.end(); ++it) {
			symbol_order_mgr->RemoveStopOrder(it->second->RequestId);
		}
	}
	src_cell->ClearOrderReq();
}

void SmFundOrderArea::ProcessButtonMsg(const BUTTON_ID& id)
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

void SmFundOrderArea::OnPaint()
{
	if (_Editing) return;

	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rect;
	GetClientRect(rect);

	if (m_pGM == NULL)
	{
		return;
	}

	try {

		m_pGM->BindDC(pDC, rect);



		if (!m_pGM->BeginDraw())
		{
			return;
		}

		m_pGM->Clear();

		m_pGM->FillRectangle(rect, _Resource.GridNormalBrush);
		rect.right -= 1;
		rect.bottom -= 1;
		_Grid->SetColHeaderTitles(_OrderTableHeader);
		_Grid->DrawGrid(m_pGM, rect);
		_Grid->DrawCells(m_pGM, rect);
		_Grid->DrawBorder(m_pGM, rect, _Selected);
		DrawFixedSelectedCell();

		if (_Hover) DrawMovingRect();

		DrawMovingOrder();

		DrawStopOrder();

		DrawHogaLine(rect);


		m_pGM->EndDraw();
	}
	catch (const std::exception& e) {
		const std::string error = e.what();
		LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
	}
}

void SmFundOrderArea::OnMouseMove(UINT nFlags, CPoint point)
{
	//CString msg;
	//msg.Format("OnMouseMove %d, %d\n", point.x, point.y);
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

void SmFundOrderArea::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString msg;
	msg.Format("OnKeyDown %d, %d\n", nChar, nRepCnt);
	TRACE(msg);
	Invalidate();
}

void SmFundOrderArea::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString msg;
	msg.Format("OnChar %d, %d\n", nChar, nRepCnt);
	TRACE(msg);
	Invalidate();
}


void SmFundOrderArea::OnMouseHover(UINT nFlags, CPoint point)
{
	_Hover = true;
	Invalidate();
}

void SmFundOrderArea::OnMouseLeave()
{
	if (_MouseMode == DarkHorse::MouseMode::Normal) {
		ClearOldSelectedCells();
		_FixedSelectedCell = nullptr;
		_SelectedValue.first = -1;
		_SelectedValue.second = -1;
	}
	m_bTrackMouse = FALSE;
	_Hover = false;
	Invalidate();
}

void SmFundOrderArea::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	if (ProcesButtonClickByPos(point)) return;

	if (_FundDialog) {

		if (_Symbol) _FundDialog->ChangedSymbol(_Symbol);

		SmFundOrderCenterWnd* wnd = (SmFundOrderCenterWnd*)GetParent();
		_FundDialog->ChangedCenterWindow(wnd->ID());
	}

	auto cell = _Grid->FindCellByPos(_X, _Y);
	if (!cell) return;

	if (cell) { _FixedSelectedCell = cell; }

	auto cell_pos = _Grid->FindRowCol(point.x, point.y);
	if (cell_pos.second == DarkHorse::OrderHeader::SELL_ORDER ||
		cell_pos.second == DarkHorse::OrderHeader::BUY_ORDER ||
		cell_pos.second == DarkHorse::OrderHeader::SELL_STOP ||
		cell_pos.second == DarkHorse::OrderHeader::BUY_STOP) {
		const int price = FindValue(cell_pos.first);
		_SelectedValue.first = price;
		_SelectedValue.second = cell_pos.second;
	}

	const auto order_count = GetOrderCount(cell);

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


void SmFundOrderArea::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (_MouseMode == DarkHorse::MouseMode::Normal) {
		///ClearOldSelectedCells();

		auto cell = _Grid->FindCellByPos(_X, _Y);
		if (cell) { _FixedSelectedCell = cell; }

		const auto order_count = GetOrderCount(cell);

		if (order_count.second > 0) {
			if (cell->Col() == DarkHorse::OrderHeader::SELL_ORDER)
				CancelOrder(_FixedSelectedCell);
			else if (cell->Col() == DarkHorse::OrderHeader::BUY_ORDER)
				CancelOrder(_FixedSelectedCell);
			else if (cell->Col() == DarkHorse::OrderHeader::BUY_STOP)
				CancelStop(_FixedSelectedCell);
			else if (cell->Col() == DarkHorse::OrderHeader::SELL_STOP)
				CancelStop(_FixedSelectedCell);

			_EnableOrderShow = true;
			_EnableStopShow = true;
		}
	}

	CBCGPStatic::OnRButtonDown(nFlags, point);
}


void SmFundOrderArea::OnLButtonUp(UINT nFlags, CPoint point)
{

	if (_DraggingOrder) {
		_EndX = point.x;
		_EndY = point.y;

		auto cell = _Grid->FindCellByPos(_EndX, _EndY);
		if (!cell) {
			if (_OrderStartCol == DarkHorse::OrderHeader::SELL_ORDER)
				CancelOrder(_FixedSelectedCell);
			else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_ORDER)
				CancelOrder(_FixedSelectedCell);
			else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_STOP)
				CancelStop(_FixedSelectedCell);
			else if (_OrderStartCol == DarkHorse::OrderHeader::SELL_STOP)
				CancelStop(_FixedSelectedCell);
		}
		else {
			if (cell->Col() == _OrderStartCol) {
				auto found = _RowIndexToPriceMap.find(cell->Row());
				if (found != _RowIndexToPriceMap.end()) {

					if (_OrderStartCol == DarkHorse::OrderHeader::SELL_ORDER)
						ChangeOrder(_FixedSelectedCell, found->second);
					else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_ORDER)
						ChangeOrder(_FixedSelectedCell, found->second);
					else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_STOP)
						ChangeStop(_FixedSelectedCell, cell, _OrderStartPrice, found->second);
					else if (_OrderStartCol == DarkHorse::OrderHeader::SELL_STOP)
						ChangeStop(_FixedSelectedCell, cell, _OrderStartPrice, found->second);
				}
				else {
					if (_OrderStartCol == DarkHorse::OrderHeader::SELL_ORDER)
						CancelOrder(_FixedSelectedCell);
					else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_ORDER)
						CancelOrder(_FixedSelectedCell);
					else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_STOP)
						CancelStop(_FixedSelectedCell);
					else if (_OrderStartCol == DarkHorse::OrderHeader::SELL_STOP)
						CancelStop(_FixedSelectedCell);
				}
			}
			else {
				if (_OrderStartCol == DarkHorse::OrderHeader::SELL_ORDER)
					CancelOrder(_FixedSelectedCell);
				else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_ORDER)
					CancelOrder(_FixedSelectedCell);
				else if (_OrderStartCol == DarkHorse::OrderHeader::BUY_STOP)
					CancelStop(_FixedSelectedCell);
				else if (_OrderStartCol == DarkHorse::OrderHeader::SELL_STOP)
					CancelStop(_FixedSelectedCell);
			}
		}

		_DraggingOrder = false;
		ReleaseCapture();
	}
	_EnableStopShow = true;
	_EnableOrderShow = true;
	Invalidate();
	_MovingOrder = false;

	CBCGPStatic::OnLButtonUp(nFlags, point);
}


int SmFundOrderArea::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rect;
	GetClientRect(rect);
	return 0;
}


void SmFundOrderArea::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!_Grid) return;

	auto cell_pos = _Grid->FindRowCol(point.x, point.y);
	if (cell_pos.second == DarkHorse::OrderHeader::SELL_STOP) {
		const int price = FindValue(cell_pos.first);
		PutStopOrder(SmPositionType::Sell, price);
	}
	else if (cell_pos.second == DarkHorse::OrderHeader::BUY_STOP) {
		const int price = FindValue(cell_pos.first);
		PutStopOrder(SmPositionType::Buy, price);
	}
	else if (cell_pos.second == DarkHorse::OrderHeader::SELL_ORDER) {
		const int price = FindValue(cell_pos.first);
		PutOrder(SmPositionType::Sell, price);
	}
	else if (cell_pos.second == DarkHorse::OrderHeader::BUY_ORDER) {
		const int price = FindValue(cell_pos.first);
		PutOrder(SmPositionType::Buy, price);
	}

	CBCGPStatic::OnLButtonDblClk(nFlags, point);
}

LRESULT SmFundOrderArea::OnWmSymbolMasterReceived(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = static_cast<int>(wParam);
	if (!_Symbol || _Symbol->Id() != symbol_id) return 0;

	SetCenterValues(_Symbol, false);

	return 1;
}

LRESULT SmFundOrderArea::OnWmQuoteReceived(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = static_cast<int>(wParam);
	if (!_Symbol || _Symbol->Id() != symbol_id) return 0;

	SetQuote(_Symbol);

	return 1;
}

LRESULT SmFundOrderArea::OnWmHogaReceived(WPARAM wParam, LPARAM lParam)
{
	const int symbol_id = static_cast<int>(wParam);
	if (!_Symbol || _Symbol->Id() != symbol_id) return 0;

	SetHoga(_Symbol);

	return 1;
}

LRESULT SmFundOrderArea::OnUmOrderChanged(WPARAM wParam, LPARAM lParam)
{
	UpdateOrder(_Symbol->SymbolCode());
	Invalidate(FALSE);
	return 1;
}



void SmFundOrderArea::OnTimer(UINT_PTR nIDEvent)
{
	bool needDraw = false;
	if (_EnableQuoteShow && _Symbol) {
		ClearOldQuote();
		SetQuote(_Symbol);
		needDraw = true;
		_EnableQuoteShow = false;
	}
	if (_EnableHogaShow && _Symbol) {
		ClearOldHoga();
		SetHoga(_Symbol);
		needDraw = true;
		_EnableHogaShow = false;
	}
	if (_EnableStopShow) {
		ClearStopOrders();
		SetStopOrder();
		needDraw = true;
		_EnableStopShow = false;
	}
	if (_EnableOrderShow && _Symbol) {
		ClearOrders();
		SetOrder();
		ClearOldQuote();
		SetQuote(_Symbol);
		needDraw = true;
		_EnableOrderShow = false;
	}

	

	if (needDraw) Invalidate(FALSE);

	CBCGPStatic::OnTimer(nIDEvent);
}
