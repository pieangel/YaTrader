#include "stdafx.h"
#include "SmSymbolOrderManager.h"
#include "SmOrder.h"
#include "../Global/SmTotalManager.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "SmOrderViewer.h"
#include "../Util/SmUtil.h"
#include "SmOrderRequestManager.h"
#include "SmOrderRequest.h"
#include "../Account/SmAccountManager.h"
#include "../Account/SmAccount.h"
#include "../Order/SmStopOrderManager.h"
#include "../Log/MyLogger.h"
#include <sstream>

DarkHorse::SmSymbolOrderManager::SmSymbolOrderManager(const std::string& account_no, const std::string& symbol_code)
	: _AccountNo(account_no), _SymbolCode(symbol_code)
{
	_BuyOrderViewer = std::make_shared<SmOrderViewer>();
	_SellOrderViewer = std::make_shared<SmOrderViewer>();
	_BuyStopOrderMgr = std::make_shared<SmStopOrderManager>();
	_SellStopOrderMgr = std::make_shared<SmStopOrderManager>();
}

void DarkHorse::SmSymbolOrderManager::ClearStopOrders()
{
	_BuyStopOrderMgr->Clear();
	_SellStopOrderMgr->Clear();
}

void DarkHorse::SmSymbolOrderManager::ClearBuyStopOrders()
{
	_BuyStopOrderMgr->Clear();
}

void DarkHorse::SmSymbolOrderManager::ClearSellStopOrders()
{
	_SellStopOrderMgr->Clear();
}

void DarkHorse::SmSymbolOrderManager::RemoveStopOrder(const SmPositionType& position, const int& req_id)
{
	if (position == SmPositionType::Buy)
		_BuyStopOrderMgr->RemoveOrderRequest(req_id);
	else
		_SellStopOrderMgr->RemoveOrderRequest(req_id);

	mainApp.OrderReqMgr()->RemoveOrderRequest(req_id);
}

void DarkHorse::SmSymbolOrderManager::RemoveStopOrder(const int& req_id)
{
	mainApp.OrderReqMgr()->RemoveOrderRequest(req_id);
	_BuyStopOrderMgr->RemoveOrderRequest(req_id);
	_SellStopOrderMgr->RemoveOrderRequest(req_id);
}

void DarkHorse::SmSymbolOrderManager::AddBuyStopOrder(const std::shared_ptr<SmOrderRequest>& order_req)
{
	_BuyStopOrderMgr->AddOrderRequest(order_req);
}

void DarkHorse::SmSymbolOrderManager::AddSellStopOrder(const std::shared_ptr<SmOrderRequest>& order_req)
{
	_SellStopOrderMgr->AddOrderRequest(order_req);
}

void DarkHorse::SmSymbolOrderManager::AddCutStopOrder(const std::shared_ptr<SmOrder>& filled_order)
{
	if (filled_order->FilledCount == 0) return;

	auto old_req = mainApp.OrderReqMgr()->FindOrderRequest(filled_order->OrderReqId);
	if (!old_req || old_req->CutMode == 0) return;
	const auto symbol = mainApp.SymMgr()->FindSymbol(filled_order->SymbolCode);
	if (!symbol) return;
	const auto account = mainApp.AcntMgr()->FindAccount(filled_order->AccountNo);
	if (!account) return;

	const int int_tick_size = static_cast<int>(symbol->TickSize() * pow(10, symbol->decimal()));
	int loss_cut_price = 0, profit_cut_price = 0;
	if (filled_order->PositionType == SmPositionType::Buy) {
		profit_cut_price = filled_order->FilledPrice + old_req->ProfitCutTick * int_tick_size;
		loss_cut_price = filled_order->FilledPrice - old_req->LossCutTick * int_tick_size;
		if (old_req->CutMode == 1) {
			std::shared_ptr<SmOrderRequest> profit_order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), profit_cut_price);
			profit_order_req->CutMode = 0;
			profit_order_req->CutSlip = old_req->CutSlip;
			profit_order_req->ProfitCutTick = old_req->ProfitCutTick;
			profit_order_req->OrderAmount = abs(filled_order->FilledCount);
			_SellStopOrderMgr->AddOrderRequest(profit_order_req);
		}
		else if (old_req->CutMode == 2) {
			std::shared_ptr<SmOrderRequest> loss_order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), loss_cut_price);
			loss_order_req->CutMode = 0;
			loss_order_req->CutSlip = old_req->CutSlip;
			loss_order_req->LossCutTick = old_req->LossCutTick;
			loss_order_req->OrderAmount = abs(filled_order->FilledCount);
			_SellStopOrderMgr->AddOrderRequest(loss_order_req);
		}
		else if (old_req->CutMode == 3) {
			std::shared_ptr<SmOrderRequest> profit_order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), profit_cut_price);
			std::shared_ptr<SmOrderRequest> loss_order_req = SmOrderRequestManager::MakeDefaultSellOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), loss_cut_price);
			profit_order_req->CutMode = 0;
			profit_order_req->CutSlip = old_req->CutSlip;
			profit_order_req->OrderAmount = abs(filled_order->FilledCount);
			profit_order_req->ProfitCutTick = old_req->ProfitCutTick;

			loss_order_req->CutMode = 0;
			loss_order_req->CutSlip = old_req->CutSlip;
			loss_order_req->OrderAmount = abs(filled_order->FilledCount);
			loss_order_req->LossCutTick = old_req->LossCutTick;

			profit_order_req->CounterRequestId = loss_order_req->RequestId;
			loss_order_req->CounterRequestId = profit_order_req->RequestId;
			_SellStopOrderMgr->AddOrderRequest(profit_order_req);
			_SellStopOrderMgr->AddOrderRequest(loss_order_req);
		}
	}
	else if (filled_order->PositionType == SmPositionType::Sell) {
		profit_cut_price = filled_order->FilledPrice - old_req->ProfitCutTick * int_tick_size;
		loss_cut_price = filled_order->FilledPrice + old_req->LossCutTick * int_tick_size;
		if (old_req->CutMode == 1) {
			std::shared_ptr<SmOrderRequest> profit_order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), profit_cut_price);
			profit_order_req->CutMode = 0;
			profit_order_req->CutSlip = old_req->CutSlip;
			profit_order_req->OrderAmount = abs(filled_order->FilledCount);
			profit_order_req->ProfitCutTick = old_req->ProfitCutTick;
			_BuyStopOrderMgr->AddOrderRequest(profit_order_req);
		}
		else if (old_req->CutMode == 2) {
			std::shared_ptr<SmOrderRequest> loss_order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), loss_cut_price);
			loss_order_req->CutMode = 0;
			loss_order_req->CutSlip = old_req->CutSlip;
			loss_order_req->LossCutTick = old_req->LossCutTick;
			loss_order_req->OrderAmount = abs(filled_order->FilledCount);
			_BuyStopOrderMgr->AddOrderRequest(loss_order_req);
		}
		else if (old_req->CutMode == 3) {
			std::shared_ptr<SmOrderRequest> profit_order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), profit_cut_price);
			std::shared_ptr<SmOrderRequest> loss_order_req = SmOrderRequestManager::MakeDefaultBuyOrderRequest(account->No(), account->Pwd(), symbol->SymbolCode(), loss_cut_price);
			profit_order_req->CutMode = 0;
			profit_order_req->CutSlip = old_req->CutSlip;
			profit_order_req->OrderAmount = abs(filled_order->FilledCount);
			profit_order_req->ProfitCutTick = old_req->ProfitCutTick;
			loss_order_req->CutMode = 0;
			loss_order_req->CutSlip = old_req->CutSlip;
			loss_order_req->OrderAmount = abs(filled_order->FilledCount);
			loss_order_req->LossCutTick = old_req->LossCutTick;
			profit_order_req->CounterRequestId = loss_order_req->RequestId;
			loss_order_req->CounterRequestId = profit_order_req->RequestId;
			_BuyStopOrderMgr->AddOrderRequest(profit_order_req);
			_BuyStopOrderMgr->AddOrderRequest(loss_order_req);
		}
	}

}

void DarkHorse::SmSymbolOrderManager::AddAcceptedOrder(std::shared_ptr<SmOrder> order)
{
	const std::lock_guard<std::mutex> lock(_m);

	if (!order) return;
	_AcceptedOrderMap[order->OrderNo] = order;

	// 시그널을 발생 시킨다.
	order_update(order->SymbolCode);
}

void DarkHorse::SmSymbolOrderManager::RemoveAcceptedOrder(const std::string& order_no)
{
	const std::lock_guard<std::mutex> lock(_m);

	auto it = _AcceptedOrderMap.find(order_no);
	if (it != _AcceptedOrderMap.end()) {
		const std::string symbol_code = it->second->SymbolCode;
		_AcceptedOrderMap.erase(it);

		// 시그널을 발생 시킨다.
		order_update(symbol_code);
	}
}

void DarkHorse::SmSymbolOrderManager::AddFilledOrder(std::shared_ptr<SmOrder> order)
{
	if (!order) return;
	
	const std::lock_guard<std::mutex> lock(_m_filled);

	

	_FilledOrderMap[order->OrderNo] = order;
		
}

void DarkHorse::SmSymbolOrderManager::RemoveFilledOrder(const std::string& order_no)
{
	const std::lock_guard<std::mutex> lock(_m_filled);

	auto it = _FilledOrderMap.find(order_no);
	if (it != _FilledOrderMap.end()) _FilledOrderMap.erase(it);
}

void DarkHorse::SmSymbolOrderManager::ClearFilledOrder()
{
	const std::lock_guard<std::mutex> lock(_m_filled);
	_FilledOrderMap.clear();
}

void DarkHorse::SmSymbolOrderManager::AddSettledOrder(std::shared_ptr<SmOrder> order)
{
	if (!order) return;
	_SettledOrderMap[order->OrderNo] = order;
}

void DarkHorse::SmSymbolOrderManager::RemoveSettledOrder(const std::string& order_no)
{
	auto it = _SettledOrderMap.find(order_no);
	if (it != _SettledOrderMap.end()) _SettledOrderMap.erase(it);
}

void DarkHorse::SmSymbolOrderManager::OnFilledOrderByEach(const std::shared_ptr<SmOrder>& new_filled_order)
{
	if (!new_filled_order) return;
	if (new_filled_order->SymbolCode.compare(_SymbolCode) != 0) return;

	new_filled_order->State = SmOrderState::Filled;
	// 접수 확인 목록에서 없애 준다.
	RemoveAcceptedOrder(new_filled_order->OrderNo);
	// 미결제 주문이 없다면 맨뒤에 추가하고 나간다.
	if (_FilledOrderMap.size() == 0) {
		_FilledOrderMap[new_filled_order->OrderNo] = new_filled_order;
		return;
	}
	auto it = _FilledOrderMap.begin();
	// 잔고 주문이 남아 있을 경우 처리 - 주문 번호가 빠른 것 부터 처리한다.
	while ( it != _FilledOrderMap.end()) {
		std::shared_ptr<SmOrder> old_filled_order = it->second;

		// 현재 들어온 주문이 맨 마지막 주문과 포지션이 같으면 바로 나간다.
		// 새로운 주문은 결과를 보고 밑에서 다시 처리한다.
		if (old_filled_order->PositionType == new_filled_order->PositionType) {
			// 새로운 주문을 추가하고 빠져 나간다.
			_FilledOrderMap[new_filled_order->OrderNo] = new_filled_order;
			it++;
			break;
		}
		// 들어온 주문과 현재 주문과 갯수가 미결제 수량이 같을 경우
		if (std::abs(new_filled_order->UnsettledQty) == std::abs(old_filled_order->UnsettledQty)) {
			// 기존 주문에 대한 처리
			// 미결제 수량 초기화
			old_filled_order->UnsettledQty = 0;
			// 청산확인 설정
			old_filled_order->State = SmOrderState::Settled;
			// 체결 목록에서 지워준다.
			it = _FilledOrderMap.erase(it);
			// 청산 확인 목록에 넣어 준다.
			_SettledOrderMap[old_filled_order->OrderNo] = old_filled_order;
			// 새로운 주문에 대한 처리
			// 미결제 수량 갱신
			new_filled_order->UnsettledQty = 0;
			// 청산확인 설정
			new_filled_order->State = SmOrderState::Settled;
			// 청산확인 목록에 추가해 준다.
			_SettledOrderMap[new_filled_order->OrderNo] = new_filled_order;
			break;
		}
		// 신규 주문 미결제 수량이 더 클 경우 - 이 경우 신규 주문의 미결제 수량은 줄어든다.
		// 기존 주문은 미결제 수량이 0이 된다.
		else if (std::abs(new_filled_order->UnsettledQty) > std::abs(old_filled_order->UnsettledQty)) {
			// 신규 주문 미결제 수량을 조절해 준다. 덧셈을 해주는 이유는 포지션이 다르기 때문에 덧셈을 하면 미결제 수량이 상쇄 되기 때문이다.
			new_filled_order->UnsettledQty += old_filled_order->UnsettledQty;
			// 기존 주문에 대한 처리
			// 미결제 수량 초기화
			old_filled_order->UnsettledQty = 0;
			// 청산확인 설정
			old_filled_order->State = SmOrderState::Settled;
			// 기존 주문을 체결 목록에서 지워준다.
			it = _FilledOrderMap.erase(it);
			// 청산확인 목록에 추가해 준다.
			_SettledOrderMap[old_filled_order->OrderNo] = old_filled_order;
		}
		// 신규 주문 미결제 수량이 더 작을 경우 - 이 경우 신규 주문은 미결제 수량이 0이된다.
		// 기존 주문은 신규 주문으로 미결제 수량이 줄어든다.
		else if (std::abs(new_filled_order->UnsettledQty) < std::abs(old_filled_order->UnsettledQty)) {
			// 기존 주문에 대한 처리
			old_filled_order->UnsettledQty += new_filled_order->UnsettledQty;
			// 신규 주문 처리
			// 신규 주문 잔고 초기화
			new_filled_order->UnsettledQty = 0;
			// 청산확인 설정
			new_filled_order->State = SmOrderState::Settled;
			// 청산확인 목록에 추가해 준다.
			_SettledOrderMap[new_filled_order->OrderNo] = new_filled_order;
			it++;
			break;
		}
	}
}

void DarkHorse::SmSymbolOrderManager::OnAcceptedOrder(const std::shared_ptr<SmOrder>& accepted_order)
{
	if (!accepted_order) return;

	std::ostringstream ss;

	ss << std::this_thread::get_id();

	std::string idstr = ss.str();

	LOGINFO(CMyLogger::getInstance(), "OnAcceptedOrder:: thread id = %s", idstr.c_str());


	// 이미 체결되었거나 청산된 주문은 처리하지 않는다.
	// 주문 갯수와 체결 갯수가 일치하고 이미 체결된 상태일 때는 더이상 처리하지 않는다.
	if ((accepted_order->State == SmOrderState::Filled || accepted_order->State == SmOrderState::Settled ) && 
		(accepted_order->OrderAmount == accepted_order->FilledCount)) {
		RemoveAcceptedOrder(accepted_order->OrderNo);
		RemoveAcceptedOrder(accepted_order->OriOrderNo);
		return;
	}
	// Add the order to the accepted order list if only the order is new and modified
	if (accepted_order->OrderType == SmOrderType::New || accepted_order->OrderType == SmOrderType::Modify) {
		// 접수된 주문을 목록에 추가한다.
		AddAcceptedOrder(accepted_order);
	}
	
	accepted_order->State = SmOrderState::Accepted;

}

void DarkHorse::SmSymbolOrderManager::OnUnfilledOrder(const std::shared_ptr<SmOrder>& unfilled_order)
{
	if (!unfilled_order) return;

	RemoveAcceptedOrder(unfilled_order->OriOrderNo);
}

void DarkHorse::SmSymbolOrderManager::AddHistoryOrder(const std::shared_ptr<SmOrder>& order)
{
	if (!order) return;
	_HistoryOrderMap[order->OrderNo] = order;
}
/*
std::shared_ptr<DarkHorse::SmPosition> DarkHorse::SmSymbolOrderManager::CalcPosition()
{
	if (_FilledOrderMap.size() == 0) return nullptr;

	std::shared_ptr<SmSymbol> symbol = mainApp.SymMgr()->FindSymbol(_SymbolCode);

	std::shared_ptr<DarkHorse::SmPosition> position = mainApp.TotalPosiMgr()->FindAddPosition(_AccountNo, _SymbolCode);
	double total_filled_order_qty{ 0 };
	double total_price{ 0 };
	for (auto it = _FilledOrderMap.begin(); it != _FilledOrderMap.end(); ++it) {
		const auto& filled_order = it->second;
		total_filled_order_qty += filled_order->UnsettledQty;
		total_price += (filled_order->FilledPrice * filled_order->UnsettledQty);
	}

	if (total_filled_order_qty == 0)
		return position;

	const double avg_price = ROUNDING(total_price / total_filled_order_qty, 0);
	
	position->AccountNo = _AccountNo;
	position->SymbolCode = _SymbolCode;
	position->AvgPrice = avg_price;
	position->OpenQty = static_cast<int>(total_filled_order_qty);
	position->Position = total_filled_order_qty > 0 ? SmPositionType::Buy : SmPositionType::Sell;
	position->OpenPL = total_filled_order_qty * (symbol->Qoute.close - avg_price) * symbol->SeungSu();

	CString msg;
	msg.Format("CalcPosition:: strAccountNo = [%s] symbolcode = [%s], total_price = [%.2f], openqty = [%d], avg_price = [%.2f],  open_pl = [%.2f],  openpl = [%.02f] \n", _AccountNo.c_str(), _SymbolCode.c_str(), total_price, position->OpenQty, position->AvgPrice,  position->OpenPL);
	TRACE(msg);

	return position;
}
*/

void DarkHorse::SmSymbolOrderManager::OnFilledOrder(const std::shared_ptr<SmOrder>& new_filled_order)
{
	// 접수 주문 목록에서 제거해 준다.
	RemoveAcceptedOrder(new_filled_order->OrderNo);
	LOGINFO(CMyLogger::getInstance(), "체결 이벤트");
	// 이미 처리된 주문은 아무 처리도 하지 않는다.
	//if (new_filled_order->State == SmOrderState::Settled)
	//	return;
	if (new_filled_order->State == SmOrderState::Filled || new_filled_order->State == SmOrderState::Settled) {
		if (new_filled_order->OrderAmount != new_filled_order->FilledCount) {
			new_filled_order->State = SmOrderState::PartialFilled;

			LOGINFO(CMyLogger::getInstance(), "부분체결:: order_no = %s, account_no = %s, symbol_code = %s, filled_amount = %d", 
				new_filled_order->OrderNo.c_str(), 
				new_filled_order->AccountNo.c_str(),
				new_filled_order->SymbolCode.c_str(),
				new_filled_order->FilledCount);

		}
		else {
			return;
		}
	}

	// Change the order state. After the order state could be changed to settled.
	new_filled_order->State = SmOrderState::Filled;

	auto symbol = mainApp.SymMgr()->FindSymbol(new_filled_order->SymbolCode);
	if (!symbol) return;

	auto position = mainApp.TotalPosiMgr()->FindPosition(new_filled_order->AccountNo, new_filled_order->SymbolCode);
	//double curClose = symbol->Qoute.close / std::pow(10, symbol->Decimal());
	const int curClose = symbol->Qoute.close;
	
	// 주문 포지션에 따른 부호 결정
	int buho = new_filled_order->PositionType == SmPositionType::Buy ? 1 : -1;
	if (!position) { // 포지션이 없는 경우
		position = mainApp.TotalPosiMgr()->FindAddPosition(new_filled_order->AccountNo, new_filled_order->SymbolCode);

		position->Position = new_filled_order->PositionType;
		position->AvgPrice = new_filled_order->FilledPrice;
		position->OpenQty = buho * new_filled_order->FilledCount;
		position->OpenPL = position->OpenQty * (curClose - position->AvgPrice) * symbol->seung_su();
		new_filled_order->UnsettledQty = position->OpenQty;
		AddFilledOrder(new_filled_order);
		// 포지션을 업데이트 해준다.
		mainApp.TotalPosiMgr()->UpdatePosition(new_filled_order->SymbolCode);
		return;
	}
	else { // 포지션이 있는 경우
		const SmPositionType old_position = position->Position;
		const int old_open_qty = position->OpenQty;
		double trade_pl = 0.0, open_pl = 0.0;

		if (new_filled_order->PositionType == SmPositionType::Sell) { //체결수량이 매도	
			if (position->OpenQty > 0) { // 보유수량이 매수			
				if (position->OpenQty >= new_filled_order->FilledCount) { //보유수량이 크거나 같은 경우
					position->OpenQty = position->OpenQty - new_filled_order->FilledCount;
					trade_pl = double(-new_filled_order->FilledCount * (position->AvgPrice - new_filled_order->FilledPrice) * symbol->seung_su());
					trade_pl = trade_pl / pow(10, symbol->decimal());
					position->TradePL += trade_pl;
					//new_filled_order->State = SmOrderState::Settled;
					new_filled_order->FilledCount = 0;
					new_filled_order->UnsettledQty = 0;
					if (position->OpenQty == 0) { // 매수 포지션 청산
						position->AvgPrice = 0;
						position->OpenPL = 0;
					}
				}
				else { //체결수량이 큰 경우
					position->TradePL += double(position->OpenQty * (new_filled_order->FilledPrice - position->AvgPrice) * symbol->seung_su());
					position->AvgPrice = new_filled_order->FilledPrice;
					position->OpenQty = position->OpenQty - new_filled_order->FilledCount;
					open_pl = position->OpenQty * (curClose - position->AvgPrice) * symbol->seung_su();
					open_pl = open_pl / pow(10, symbol->decimal());
					position->OpenPL = open_pl;
					// 보유수량과 상쇄되고 남은 갯수가 잔고가 된다. - 여기서는 잔고가 매도 포지션이 된다.
					new_filled_order->FilledCount = position->OpenQty;
					new_filled_order->UnsettledQty += position->OpenQty;
				}
			}
			else { // 보유수량이 매도 ( 보유수량이매도/체결수량이매도 인 경우)
				position->AvgPrice = double((position->OpenQty * position->AvgPrice - new_filled_order->FilledCount * new_filled_order->FilledPrice) / (position->OpenQty - new_filled_order->FilledCount));
				position->OpenQty = position->OpenQty - new_filled_order->FilledCount;
				open_pl = position->OpenQty * (curClose - position->AvgPrice) * symbol->seung_su();
				open_pl = open_pl / pow(10, symbol->decimal());
				position->OpenPL = open_pl;
				// 이경우 포지션이 같으므로 더해 주지 않는다.
				// 잔고 수량에 더해준다. 매도는 음수, 매수는 양수, 포지션 없으면 0
				new_filled_order->FilledCount = new_filled_order->FilledCount;
				new_filled_order->UnsettledQty += buho * new_filled_order->FilledCount;
			}
		}
		else { //체결수량이 매수
			if (position->OpenQty >= 0) { // 보유수량이 매수/체결수량이매수 인 경우
				position->AvgPrice = double((position->OpenQty * position->AvgPrice + new_filled_order->FilledCount * new_filled_order->FilledPrice) / (position->OpenQty + new_filled_order->FilledCount));
				position->OpenQty = position->OpenQty + new_filled_order->FilledCount;
				open_pl = position->OpenQty * (curClose - position->AvgPrice) * symbol->seung_su();
				open_pl = open_pl / pow(10, symbol->decimal());
				position->OpenPL = open_pl;
				// 이경우 포지션이 같으므로 더해 주지 않는다.
				// 잔고 수량에 더해 준다. 매도는 음수, 매수는 양수, 포지션 없으면 0
				new_filled_order->UnsettledQty += buho * new_filled_order->FilledCount;
				new_filled_order->FilledCount = new_filled_order->FilledCount;
			}
			else { //보유수량이 매도
				if (abs(position->OpenQty) >= new_filled_order->FilledCount) { //보유수량이 큰경우
					position->OpenQty = position->OpenQty + new_filled_order->FilledCount;
					trade_pl = double(new_filled_order->FilledCount * (position->AvgPrice - new_filled_order->FilledPrice) * symbol->seung_su());
					trade_pl = trade_pl / pow(10, symbol->decimal());
					position->TradePL += trade_pl;
					//new_filled_order->State = SmOrderState::Settled;
					new_filled_order->FilledCount = 0;
					// 들어온 주문은 상쇄되고 포지션을 상실한다.
					new_filled_order->UnsettledQty = 0;
					if (position->OpenQty == 0) { // 매도 포지션 청산
						position->AvgPrice = 0;
						position->OpenPL = 0;
					}
				}
				else { //체결수량이 큰 경우				
					position->TradePL += double(position->OpenQty * (new_filled_order->FilledPrice - position->AvgPrice) * symbol->seung_su());
					position->AvgPrice = new_filled_order->FilledPrice;
					position->OpenQty = position->OpenQty + new_filled_order->FilledCount;
					open_pl = position->OpenQty * (curClose - position->AvgPrice) * symbol->seung_su();
					open_pl = open_pl / pow(10, symbol->decimal());
					position->OpenPL = open_pl;

					// 이경우 기존의 매도 포지션 잔고 갯수가 상쇄되고 남은 주문의 잔고는 매수 포지션이 된다.
					new_filled_order->FilledCount = position->OpenQty;
					new_filled_order->UnsettledQty += position->OpenQty;
				}
			}
		}

		if (new_filled_order->UnsettledQty == 0)
			new_filled_order->State = SmOrderState::Settled;

		// 여기서 스탑 주문을 검사해 준다.
		AddCutStopOrder(new_filled_order);

		// 잔고 수량에 따라 포지션을 다시 결정해 준다.
		if (position->OpenQty > 0)  // 매수 포지션 설정 : 양수임
			position->Position = SmPositionType::Buy;
		else if (position->OpenQty < 0)  // 매도 포지션 설정 : 음수임
			position->Position = SmPositionType::Sell;
		else  // 모든 포지션 상실 0임
			position->Position = SmPositionType::None;

		if (position->Position == old_position) {
			if (abs(position->OpenQty) > abs(old_open_qty)) {
				// 미결제 수량이 남아 있다면 체결 목록에 등록해 준다.
				if (new_filled_order->UnsettledQty != 0) AddFilledOrder(new_filled_order);
			}
			else if (abs(position->OpenQty) < abs(old_open_qty)) {
				const int delta = abs(old_open_qty) - abs(position->OpenQty);
				RemoveFilledOrder(position->Position == SmPositionType::Buy ? -1 * delta : delta);
			}
		}
		else {
			ClearFilledOrder();
			// 미결제 수량이 남아 있다면 체결 목록에 등록해 준다.
			if (new_filled_order->UnsettledQty != 0) AddFilledOrder(new_filled_order);
		}
		// 포지션을 업데이트 해준다.
		mainApp.TotalPosiMgr()->UpdatePosition(new_filled_order->SymbolCode);
	}
}



const std::map<std::string, std::shared_ptr<DarkHorse::SmOrder>>& DarkHorse::SmSymbolOrderManager::GetAcceptedOrders()
{
	std::ostringstream ss;

	ss << std::this_thread::get_id();

	std::string idstr = ss.str();

	//LOGINFO(CMyLogger::getInstance(), "GetAcceptedOrders:: thread id = %s", idstr.c_str());

	return _AcceptedOrderMap;
}

void DarkHorse::SmSymbolOrderManager::UpdateOrder(std::shared_ptr<DarkHorse::SmOrderViewer>& buy_order_viewer, std::shared_ptr<DarkHorse::SmOrderViewer>& sell_order_viewer)
{
	buy_order_viewer->Clear();
	sell_order_viewer->Clear();

	for (auto it = _AcceptedOrderMap.begin(); it != _AcceptedOrderMap.end(); ++it) {
		auto order = it->second;
		if (order->PositionType == SmPositionType::Buy)
			buy_order_viewer->AddOrder(order);
		else
			sell_order_viewer->AddOrder(order);
	}
}

int DarkHorse::SmSymbolOrderManager::GetUnsettledCount()
{
	int sum = 0;
	for (auto it = _FilledOrderMap.begin(); it != _FilledOrderMap.end(); it++) {
		sum += it->second->UnsettledQty;
	}
	return sum;
}

void DarkHorse::SmSymbolOrderManager::CheckStopOrder(const int& close)
{
	;
}

void DarkHorse::SmSymbolOrderManager::CheckStopOrder(std::shared_ptr<SmSymbol> symbol)
{
	_BuyStopOrderMgr->CheckStopOrder(symbol);
	_SellStopOrderMgr->CheckStopOrder(symbol);
}

void DarkHorse::SmSymbolOrderManager::UpdateOrder()
{
	_BuyOrderViewer->Clear();
	_SellOrderViewer->Clear();

	for (auto it = _AcceptedOrderMap.begin(); it != _AcceptedOrderMap.end(); ++it) {
		auto order = it->second;
		if (order->PositionType == SmPositionType::Buy)
			_BuyOrderViewer->AddOrder(order);
		else
			_SellOrderViewer->AddOrder(order);
	}
}

void DarkHorse::SmSymbolOrderManager::RemoveFilledOrder(int remove_count)
{
	const std::lock_guard<std::mutex> lock(_m_filled);

	int remain_count = remove_count;
	
	auto it = _FilledOrderMap.begin();
	// 잔고 주문이 남아 있을 경우 처리 - 주문 번호가 빠른 것 부터 처리한다.
	while (it != _FilledOrderMap.end()) {
		if (_FilledOrderMap.empty()) break;

		remain_count = it->second->UnsettledQty + remain_count;
		if (remove_count < 0) {
			if (remain_count > 0) {
				it->second->UnsettledQty = remain_count;
				break;
			}
			else {
				
				it = _FilledOrderMap.erase(it);
				if (remain_count == 0) break;
			}
		}
		else {
			if (remain_count < 0) {
				it->second->UnsettledQty = remain_count;
				break;
			}
			else {
				it = _FilledOrderMap.erase(it);
				
				if (remain_count == 0) break;
			}
		}
	}
}
