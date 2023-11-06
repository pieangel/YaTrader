#include "stdafx.h"
#include "SmOutSystem.h"
#include "../Util/IdGenerator.h"
#include "../Global/SmTotalManager.h"
#include "../Order/OrderRequest/OrderRequest.h"
#include "../Order/OrderRequest/OrderRequestManager.h"
#include "../Fund/SmFund.h"
#include "../Account/SmAccount.h"
#include "../Symbol/SmSymbol.h"
#include "../Log/MyLogger.h"
#include "../Controller/SymbolPositionControl.h"
#include "../Controller/QuoteControl.h"
#include "SmOutSystemManager.h"
#include "../Quote/SmQuote.h"
#include "../Quote/SmQuoteManager.h"
#include "../Position/TotalPositionManager.h"
#include "../Position/Position.h"
#include "../Position/AccountPositionManager.h"
#include "../Position/GroupPositionManager.h"
#include "../Account/SmAccountManager.h"

namespace DarkHorse {
	SmOutSystem::SmOutSystem(const std::string& name)
		:name_(name), id_(IdGenerator::get_id())
	{
		position_control_ = std::make_shared<SymbolPositionControl>();
		position_control_->set_out_system_id(id_);
		quote_control_ = std::make_shared<QuoteControl>();
	}

	SmOutSystem::~SmOutSystem()
	{
	}

	void SmOutSystem::account(std::shared_ptr<SmAccount> val)
	{
		if (!val) return;
		account_ = val;
		position_control_->set_account(account_);
	}

	void SmOutSystem::symbol(std::shared_ptr<DarkHorse::SmSymbol> val)
	{
		symbol_ = val;
		position_control_->set_symbol(symbol_);
	}

	void SmOutSystem::fund(std::shared_ptr<DarkHorse::SmFund> val)
	{
		if (!val) return;
		fund_ = val;
		position_control_->set_fund(fund_);
	}

	void SmOutSystem::put_order(const std::string& signal_name, int order_kind, int order_amount)
	{
		if (order_type_ == OrderType::SubAccount ||
			order_type_ == OrderType::MainAccount) {
			if (!account_) return;
			put_order_each(account_, signal_name, order_kind, order_amount);
		}
		else if (order_type_ == OrderType::Fund) {
			if (!fund_) return;
			for (auto& account : fund_->GetAccountVector()) {
				put_order_each(account, signal_name, order_kind, order_amount);
			}
		}
		else return;
	}

	void SmOutSystem::liq_all()
	{
		OrderType position_type = position_control_->Position_type();
		std::map<std::string, std::shared_ptr<Position>> active_position_vector_;
		if (position_type == OrderType::SubAccount) {
			if (!account_) return;
			if (!account_->was_liq()) return;
			auto position_manager = mainApp.total_position_manager()->find_position_manager(account_->No());
			if (!position_manager) return;
			active_position_vector_.clear();
			position_manager->get_active_positions(active_position_vector_);
			account_->was_liq(true);
		}
		else if (position_type == OrderType::Fund) {
			if (!fund_) return;
			if (fund_->was_liq()) return;
			auto position_manager = mainApp.total_position_manager()->find_fund_group_position_manager(fund_->Name());
			if (!position_manager) return;
			active_position_vector_.clear();
			position_manager->get_active_positions(active_position_vector_);
			fund_->was_liq(true);
		}
		else {
			if (!account_) return;
			if (!account_->was_liq()) return;
			auto position_manager = mainApp.total_position_manager()->find_account_group_position_manager(account_->No());
			if (!position_manager) return;
			active_position_vector_.clear();
			position_manager->get_active_positions(active_position_vector_);
			account_->was_liq(true);
		}

		for (auto it = active_position_vector_.begin(); it != active_position_vector_.end(); it++) {
			auto position = it->second;
			if (position->open_quantity > 0) {
				put_order_each(account_, name_, 3, abs(position->open_quantity));
			}
			else if (position->open_quantity < 0) {
				put_order_each(account_, name_, 1, abs(position->open_quantity));
			}
		}
	}

	void SmOutSystem::put_order_each(std::shared_ptr<SmAccount> account, const std::string& signal_name, int order_kind, int order_amount)
	{
		if (!account || !symbol_) return;

		std::shared_ptr<OrderRequest> order_req = std::make_shared<OrderRequest>();
		order_req->request_id = OrderRequestManager::get_id();
		auto parent_account = mainApp.AcntMgr()->FindAccountById(account->parent_id());
		order_req->account_no = parent_account ? parent_account->No() : account->No();
		order_req->password = parent_account ? parent_account->Pwd() : account->Pwd();
		order_req->order_amount = order_amount * seung_su_;
		order_req->symbol_code = symbol_->SymbolCode();

		order_req->order_type = SmOrderType::New;
		if (account->Type() == "1") {
			order_req->request_type = OrderRequestType::Abroad;
			order_req->fill_condition = SmFilledCondition::Day;
		}
		else {
			order_req->request_type = OrderRequestType::Domestic;
			order_req->fill_condition = SmFilledCondition::Fas;
		}
		order_req->order_context.signal_name = signal_name;
		// 활성화된 시스템에 대하여 주문을 넣는다.
		switch (order_kind)
		{
		case 1: // Buy
			//msg.Format(_T("신호이름 : %s, 신호 종류 : %d, 주문 : 매수\n"), sigName.c_str(), orderKind);
			//TRACE(msg);
			LOGINFO(CMyLogger::getInstance(), _T("신호이름 : %s, 신호 종류 : %d, 주문 : 매수"), signal_name.c_str(), order_kind);
			order_req->position_type = SmPositionType::Buy;
			break;
		case 2: { // ExitLong -> Sell
			// 매도 진입 주문으로 처리
			LOGINFO(CMyLogger::getInstance(), _T("신호이름 : %s, 신호 종류 : %d, 주문 : 매도"), signal_name.c_str(), order_kind);
			order_req->position_type = SmPositionType::Sell;
		}
			  break;
		case 3: // Sell
			LOGINFO(CMyLogger::getInstance(), _T("신호이름 : %s, 신호 종류 : %d, 주문 : 매도"), signal_name.c_str(), order_kind);
			order_req->position_type = SmPositionType::Sell;
			//order_req->order_price = quote_p->close;
			break;
		case 4: { // ExitShort -> Buy
			LOGINFO(CMyLogger::getInstance(), _T("신호이름 : %s, 신호 종류 : %d, 주문 : 매수"), signal_name.c_str(), order_kind);
			order_req->position_type = SmPositionType::Buy;
			//order_req->order_price = quote_p->close;
		}
			  break;
		default:
			break;
		}
		order_req->price_type = SmOutSystemManager::price_type;
		SetOrderPrice(order_req);

		order_req->request_type = OrderRequestType::Domestic;
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

		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 시그널이름[%s]"), order_req->order_context.signal_name.c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 해외/국내[%s]"), std::to_string((int)order_req->request_type).c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 계좌번호[%s]"), order_req->account_no.c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 비밀번호[%s]"), order_req->password.c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 계좌타입[%s]"), account->Type().c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 종목코드[%s]"), order_req->symbol_code.c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 매수/매도[%s]"), std::to_string((int)order_req->position_type).c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문가격[%s]"), std::to_string((int)order_req->order_price).c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문수량[%s]"), std::to_string((int)order_req->order_amount).c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문타입[%s]"), std::to_string((int)order_req->order_type).c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 가격타입[%s]"), std::to_string((int)order_req->price_type).c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 선물/옵션[%s]"), std::to_string((int)order_req->future_or_option).c_str());
		LOGINFO(CMyLogger::getInstance(), _T("put_order_each:: 주문조건[%s]"), std::to_string((int)order_req->fill_condition).c_str());

		mainApp.order_request_manager()->add_order_request(order_req);
	}


	void SmOutSystem::SetOrderPrice(std::shared_ptr<OrderRequest> order_req)
	{

		int price = 0;
		auto quote_p = mainApp.QuoteMgr()->get_quote(symbol_->SymbolCode());

		if (!symbol_ || !quote_p || order_req->price_type == SmPriceType::None)
			order_req->order_price = 0;
		const int int_tick_size = static_cast<int>(symbol_->TickSize() * pow(10, symbol_->decimal()));
		if (order_req->price_type == SmPriceType::Market)
			order_req->order_price = 0;
		else if (order_req->price_type == SmPriceType::Price) {
			price = quote_p->close;
			if (order_req->position_type == SmPositionType::Buy || order_req->position_type == SmPositionType::ExitSell) {
				// 매수일 경우 목표가격보다 정해진 틱만큼 위로 주문을 낸다.
				price = price + int_tick_size * SmOutSystemManager::order_tick;
			}
			else if (order_req->position_type == SmPositionType::Sell || order_req->position_type == SmPositionType::ExitBuy) {
				// 매도일 경우 목표가격보다 정해진 틱만큼 아래로 주문을 낸다.
				price = price - int_tick_size * SmOutSystemManager::order_tick;
			}
		}
		order_req->order_price = price;
	}

}
