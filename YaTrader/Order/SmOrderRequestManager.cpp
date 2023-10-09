#include "stdafx.h"
#include "SmOrderRequestManager.h"
#include "SmOrderRequest.h"

using namespace DarkHorse;

int SmOrderRequestManager::_Id = 0;

SmOrderRequestManager::SmOrderRequestManager()
{
}


SmOrderRequestManager::~SmOrderRequestManager()
{
}

std::shared_ptr<DarkHorse::SmOrderRequest> DarkHorse::SmOrderRequestManager::FindOrderRequest(const int& req_id)
{
	auto found = _OrderReqMap.find(req_id);
	if (found != _OrderReqMap.end())
		return found->second;
	else
		return nullptr;
}

void DarkHorse::SmOrderRequestManager::AddOrderRequest(const std::shared_ptr<SmOrderRequest>& order_req)
{
	if (!order_req) return;
	_OrderReqMap[order_req->RequestId] = order_req;
}

void DarkHorse::SmOrderRequestManager::RemoveOrderRequest(const int& req_id)
{
	auto found = _OrderReqMap.find(req_id);
	if (found == _OrderReqMap.end()) return;
	_OrderReqMap.erase(found);
}

unsigned int DarkHorse::SmOrderRequestManager::ThreadHandlerProc(void)
{
// 	while (true) {
// 		// 종료 신호를 보내면 루프를 나간다.
// 		if (isStop()) {
// 			break;
// 		}
// 		Packet packet;
// 		_PacketQ.take(packet);
// 		ProcessPacket(std::move(packet));
// 	}

	return 1;
}

std::shared_ptr<DarkHorse::SmOrderRequest> SmOrderRequestManager::MakeDefaultBuyOrderRequest(
	const std::string& symbol_code, 
	const int& order_price, 
	const SmPositionType& position_type /*= SmPositionType::Buy*/, 
	const int& order_amount /*= 1*/, 
	const SmOrderType& order_type /*= SmOrderType::New*/, 
	const SmPriceType& price_type /*= SmPriceType::Price*/, 
	const SmFilledCondition& fill_cond /*= SmFilledCondition::Fas*/)
{
	std::shared_ptr<SmOrderRequest> order_req = std::make_shared<SmOrderRequest>();
	order_req->RequestId = GetId();
	order_req->OrderPrice = order_price;
	order_req->OrderAmount = order_amount;
	order_req->SymbolCode = symbol_code;
	order_req->PositionType = position_type;
	order_req->OrderType = order_type;
	order_req->PriceType = price_type;
	order_req->FilledCond = fill_cond;
	return order_req;
}

std::shared_ptr<DarkHorse::SmOrderRequest> SmOrderRequestManager::MakeDefaultBuyOrderRequest(
	const std::string account_no,
	const std::string password,
	const std::string& symbol_code,
	const int& order_price,
	const int& order_amount /*= 1*/,
	const SmPriceType& price_type /*= SmPriceType::Price*/,
	const SmPositionType& position_type /*= SmPositionType::Buy*/,
	
	const SmOrderType& order_type /*= SmOrderType::New*/,
	
	const SmFilledCondition& fill_cond /*= SmFilledCondition::Fas*/)
{
	std::shared_ptr<SmOrderRequest> order_req = std::make_shared<SmOrderRequest>();
	order_req->AccountNo = account_no;
	order_req->Password = password;
	order_req->OrderPrice = order_price;
	order_req->RequestId = GetId();
	order_req->OrderAmount = order_amount;
	order_req->SymbolCode = symbol_code;
	order_req->PositionType = position_type;
	order_req->OrderType = order_type;
	order_req->PriceType = price_type;
	order_req->FilledCond = fill_cond;
	return order_req;
}

std::shared_ptr<DarkHorse::SmOrderRequest> SmOrderRequestManager::MakeDefaultSellOrderRequest(
	const std::string& symbol_code, 
	const int& order_price, 
	const SmPositionType& position_type /*= SmPositionType::Sell*/, 
	const int& order_amount /*= 1*/, 
	const SmOrderType& order_type /*= SmOrderType::New*/, 
	const SmPriceType& price_type /*= SmPriceType::Price*/, 
	const SmFilledCondition& fill_cond /*= SmFilledCondition::Fas*/)
{
	std::shared_ptr<SmOrderRequest> order_req = std::make_shared<SmOrderRequest>();
	order_req->RequestId = GetId();
	order_req->OrderPrice = order_price;
	order_req->OrderAmount = order_amount;
	order_req->SymbolCode = symbol_code;
	order_req->PositionType = position_type;
	order_req->OrderType = order_type;
	order_req->PriceType = price_type;
	order_req->FilledCond = fill_cond;
	return order_req;
}

std::shared_ptr<DarkHorse::SmOrderRequest> SmOrderRequestManager::MakeDefaultSellOrderRequest(
	const std::string account_no,
	const std::string password,
	const std::string& symbol_code,
	const int& order_price,
	const int& order_amount /*= 1*/,
	const SmPriceType& price_type /*= SmPriceType::Price*/,
	const SmPositionType& position_type /*= SmPositionType::Sell*/,
	
	const SmOrderType& order_type /*= SmOrderType::New*/,
	
	const SmFilledCondition& fill_cond /*= SmFilledCondition::Fas*/)
{
	std::shared_ptr<SmOrderRequest> order_req = std::make_shared<SmOrderRequest>();
	order_req->AccountNo = account_no;
	order_req->Password = password;
	order_req->OrderPrice = order_price;
	order_req->RequestId = GetId();
	order_req->OrderAmount = order_amount;
	order_req->SymbolCode = symbol_code;
	order_req->PositionType = position_type;
	order_req->OrderType = order_type;
	order_req->PriceType = price_type;
	order_req->FilledCond = fill_cond;
	return order_req;
}


std::shared_ptr<DarkHorse::SmOrderRequest> SmOrderRequestManager::MakeOrderRequest(
	const int& order_price, 
	const int& order_amount, 
	const std::string& symbol_code, 
	const SmPositionType& position_type, 
	const SmOrderType& order_type, 
	const SmPriceType& price_type, 
	const SmFilledCondition& fill_cond /*= SmFilledCondition::Fas*/)
{
	std::shared_ptr<SmOrderRequest> order_req = std::make_shared<SmOrderRequest>();
	order_req->RequestId = GetId();
	order_req->OrderPrice = order_price;
	order_req->OrderAmount = order_amount;
	order_req->SymbolCode = symbol_code;
	order_req->PositionType = position_type;
	order_req->OrderType = order_type;
	order_req->PriceType = price_type;
	order_req->FilledCond = fill_cond;
	return order_req;
}



std::shared_ptr<DarkHorse::SmOrderRequest> SmOrderRequestManager::MakeOrderRequest(
	const std::string account_no,
	const std::string password,
	const int& order_price, 
	const int& order_amount, 
	const std::string& symbol_code, 
	const SmPositionType& position_type, 
	const SmOrderType& order_type, 
	const SmPriceType& price_type, 
	const SmFilledCondition& fill_cond /*= SmFilledCondition::Fas*/)
{
	std::shared_ptr<SmOrderRequest> order_req = std::make_shared<SmOrderRequest>();
	order_req->AccountNo = account_no;
	order_req->Password = password;
	order_req->OrderPrice = order_price;
	order_req->RequestId = GetId();
	order_req->OrderAmount = order_amount;
	order_req->SymbolCode = symbol_code;
	order_req->PositionType = position_type;
	order_req->OrderType = order_type;
	order_req->PriceType = price_type;
	order_req->FilledCond = fill_cond;
	return order_req;
}


std::shared_ptr<DarkHorse::SmOrderRequest> SmOrderRequestManager::MakeChangeOrderRequest(
	const std::string account_no,
	const std::string password,
	const std::string& symbol_code,
	const std::string& ori_order_no,
	const int& order_price,
	const SmPositionType& position_type,
	const int& order_amount,
	const SmOrderType& order_type,
	const SmPriceType& price_type,
	const SmFilledCondition& fill_cond /*= SmFilledCondition::Fas*/)
{
	std::shared_ptr<SmOrderRequest> order_req = std::make_shared<SmOrderRequest>();
	order_req->AccountNo = account_no;
	order_req->Password = password;
	order_req->OrderPrice = order_price;
	order_req->RequestId = GetId();
	order_req->OrderAmount = order_amount;
	order_req->SymbolCode = symbol_code;
	order_req->PositionType = position_type;
	order_req->OrderType = order_type;
	order_req->PriceType = price_type;
	order_req->OriOrderNo = ori_order_no;
	order_req->FilledCond = fill_cond;
	return order_req;
}

std::shared_ptr<DarkHorse::SmOrderRequest> SmOrderRequestManager::MakeCancelOrderRequest(
	const std::string account_no,
	const std::string password,
	const std::string& symbol_code,
	const std::string& ori_order_no,
	const int& order_price,
	const SmPositionType& position_type,
	const int& order_amount,
	const SmOrderType& order_type,
	const SmPriceType& price_type,
	const SmFilledCondition& fill_cond /*= SmFilledCondition::Fas*/)
{
	std::shared_ptr<SmOrderRequest> order_req = std::make_shared<SmOrderRequest>();
	order_req->AccountNo = account_no;
	order_req->Password = password;
	order_req->OrderPrice = order_price;
	order_req->RequestId = GetId();
	order_req->OrderAmount = order_amount;
	order_req->SymbolCode = symbol_code;
	order_req->PositionType = position_type;
	order_req->OrderType = order_type;
	order_req->PriceType = price_type;
	order_req->OriOrderNo = ori_order_no;
	order_req->FilledCond = fill_cond;
	return order_req;
}
