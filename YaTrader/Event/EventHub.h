#pragma once
/*
#include <functional>
using namespace std::placeholders;

void function1(std::function<void(int, int)> fun)
{
	fun(1, 1);
}

int main (int argc, const char * argv[])
{
   ...

   aClass a;
   auto fp = std::bind(&aClass::test, a, _1, _2);
   function1(fp);

   return 0;
}
*/
#include "eventpp/callbacklist.h"
#include "eventpp/eventdispatcher.h"
#include <functional>
#include <map>
#include <string>
#include <vector>
#include "../Order/OrderUi/ControlId.h"
#include "../Json/json.hpp"
#include "../Quote/SmQuote.h"
#include "../Order/SmOrderConst.h"
#include "EventHubArg.h"

namespace DarkHorse {
	class AccountOrderControl;
	class AccountPositionControl;
	class AccountProfitLossControl;
	class AssetControl;
	class BuyHogaControl;
	class BuyOrderControl;
	class BuyStopOrderControl;
	class FavoriteSymbolControl;
	class FutureCloseControl;
	class FuturePositionControl;
	class FutureExpectedControl;
	class OptionCloseControl;
	class OptionPositionControl;
	class OptionExpectedControl;
	class PriceRangeControl;
	class SellHogaControl;
	class SellOrderControl;
	class SellStopOrderControl;
	class SymbolInfoControl;
	class SymbolPositionControl;
	class SymbolProfitLossControl;
	class SymbolTickControl;
	class TotalHogaInfoControl;
	struct SmHoga;
	struct SmQuote;
	struct SmTick;
	struct Order;
	class SmSymbol;
	struct Position;
	class SmAccount;
	class SmFund;


	using SymbolMasterCBL = eventpp::CallbackList<void(std::shared_ptr<SmSymbol> quote)>;
	using SymbolMasterCBH = eventpp::CallbackList<void(std::shared_ptr<SmSymbol> quote)>::Handle;

	using TickCBL = eventpp::CallbackList<void(SmTick tick)>;
	using TickCBH = eventpp::CallbackList<void(SmTick tick)>::Handle;

	using HogaCBL = eventpp::CallbackList<void(std::shared_ptr<SmHoga> hoga)>;
	using HogaCBH = eventpp::CallbackList<void(std::shared_ptr<SmHoga> hoga)>::Handle;

	using QuoteCBL = eventpp::CallbackList<void(std::shared_ptr<SmQuote> quote)>;
	using QuoteCBH = eventpp::CallbackList<void(std::shared_ptr<SmQuote> quote)>::Handle;

	using ExpectedCBL = eventpp::CallbackList<void(std::shared_ptr<SmQuote> quote)>;
	using ExpectedCBH = eventpp::CallbackList<void(std::shared_ptr<SmQuote> quote)>::Handle;

	using PositionCBL = eventpp::CallbackList<void(std::shared_ptr<Position> position)>;
	using PositionCBH = eventpp::CallbackList<void(std::shared_ptr<Position> position)>::Handle;

	using SymbolCBL = eventpp::CallbackList<void(const int order_window_id, std::shared_ptr<SmSymbol> symbol)>;
	using SymbolCBH = eventpp::CallbackList<void(const int order_window_id, std::shared_ptr<SmSymbol> symbol)>::Handle;

	using OrderCBL = eventpp::CallbackList<void(std::shared_ptr<Order> order, OrderEvent order_event)>;
	using OrderCBH = eventpp::CallbackList<void(std::shared_ptr<Order> order, OrderEvent order_event)>::Handle;

	using StopOrderCBL = eventpp::CallbackList<void(std::shared_ptr<Order> order)>;
	using StopOrderCBH = eventpp::CallbackList<void(std::shared_ptr<Order> order)>::Handle;

	using AccountProfitLossCBL = eventpp::CallbackList<void()>;
	using AccountProfitLossCBH = eventpp::CallbackList<void()>::Handle;


	using AbSymbolCBL = eventpp::CallbackList<void(std::shared_ptr<SmSymbol> symbol)>;
	using AbSymbolCBH = eventpp::CallbackList<void(std::shared_ptr<SmSymbol> symbol)>::Handle;

	using SymbolOrderViewCBL = eventpp::CallbackList<void(const std::string& account_type, const int center_window_id, std::shared_ptr<SmSymbol> symbol)>;
	using SymbolOrderViewCBH = eventpp::CallbackList<void(const std::string& account_type, const int center_window_id, std::shared_ptr<SmSymbol> symbol)>::Handle;


	using AccountCBL = eventpp::CallbackList<void(const int source_window_id, std::shared_ptr<SmAccount> account)>;
	using AccountCBH = eventpp::CallbackList<void(const int source_window_id, std::shared_ptr<SmAccount> account)>::Handle;

	using FundCBL = eventpp::CallbackList<void(const int source_window_id, std::shared_ptr<SmFund> fund)>;
	using FundCBH = eventpp::CallbackList<void(const int source_window_id, std::shared_ptr<SmFund> fund)>::Handle;

class EventHub
{
public:

	void subscribe_tick_event_handler(const int tick_control_id, std::function<void(SmTick tick)>&& handler)
	{
		TickCBH handle = tick_cb_list_.append(handler);
		tick_cb_handle_map_[tick_control_id] = handle;
	}
	void unsubscribe_tick_event_handler(const int tick_control_id)
	{
		auto found = tick_cb_handle_map_.find(tick_control_id);
		if (found == tick_cb_handle_map_.end()) return;
		tick_cb_list_.remove(found->second);
	}
	void process_tick_event(SmTick tick)
	{
		tick_cb_list_(tick);
	}

	void subscribe_hoga_event_handler(const int hoga_control_id, std::function<void(std::shared_ptr<SmHoga> hoga)>&& handler)
	{
		HogaCBH handle = hoga_cb_list_.append(handler);
		hoga_cb_handle_map_[hoga_control_id] = handle;
	}
	void unsubscribe_hoga_event_handler(const int hoga_control_id)
	{
		auto found = hoga_cb_handle_map_.find(hoga_control_id);
		if (found == hoga_cb_handle_map_.end()) return;
		hoga_cb_list_.remove(found->second);
	}
	void process_hoga_event(std::shared_ptr<SmHoga> hoga)
	{
		hoga_cb_list_(hoga);
	}

	void subscribe_quote_event_handler(const int quote_control_id, std::function<void(std::shared_ptr<SmQuote> quote)>&& handler)
	{
		QuoteCBH handle = quote_cb_list_.append(handler);
		quote_cb_handle_map_[quote_control_id] = handle;
	}
	void unsubscribe_quote_event_handler(const int quote_control_id)
	{
		auto found = quote_cb_handle_map_.find(quote_control_id);
		if (found == quote_cb_handle_map_.end()) return;
		quote_cb_list_.remove(found->second);
	}
	void process_quote_event(std::shared_ptr<SmQuote> quote)
	{
		quote_cb_list_(quote);
	}

	void subscribe_expected_event_handler(const int expected_control_id, std::function<void(std::shared_ptr<SmQuote> quote)>&& handler)
	{
		ExpectedCBH handle = expected_cb_list_.append(handler);
		expected_cb_handle_map_[expected_control_id] = handle;
	}
	void unsubscribe_expected_event_handler(const int expected_control_id)
	{
		auto found = expected_cb_handle_map_.find(expected_control_id);
		if (found == expected_cb_handle_map_.end()) return;
		expected_cb_list_.remove(found->second);
	}

	void process_expected_event(std::shared_ptr<SmQuote> quote)
	{
		expected_cb_list_(quote);
	}

	void subscribe_symbol_event_handler(const int symbol_control_id, std::function<void(const int order_window_id, std::shared_ptr<SmSymbol> symbol)>&& handler)
	{
		SymbolCBH handle = symbol_cb_list_.append(handler);
		symbol_cb_handle_map_[symbol_control_id] = handle;
	}

	void unsubscribe_symbol_event_handler(const int symbol_control_id)
	{
		auto found = symbol_cb_handle_map_.find(symbol_control_id);
		if (found == symbol_cb_handle_map_.end()) return;
		symbol_cb_list_.remove(found->second);
	}

	void process_symbol_event(const int order_window_id, std::shared_ptr<SmSymbol> symbol)
	{
		symbol_cb_list_(order_window_id, symbol);
	}


	void add_ab_symbol_event_handler(const int symbol_control_id, std::function<void(std::shared_ptr<SmSymbol> symbol)>&& handler)
	{
		AbSymbolCBH handle = ab_symbol_cb_list_.append(handler);
		ab_symbol_cb_handle_map_[symbol_control_id] = handle;
	}

	void unsubscribe_ab_symbol_event_handler(const int symbol_control_id)
	{
		auto found = ab_symbol_cb_handle_map_.find(symbol_control_id);
		if (found == ab_symbol_cb_handle_map_.end()) return;
		ab_symbol_cb_list_.remove(found->second);
	}

	void process_ab_symbol_event(std::shared_ptr<SmSymbol> symbol)
	{
		ab_symbol_cb_list_(symbol);
	}


	void subscribe_symbol_master_event_handler(const int symbol_master_control_id, std::function<void(std::shared_ptr<SmSymbol> symbol)>&& handler)
	{
		SymbolMasterCBH handle = symbol_master_cb_list_.append(handler);
		symbol_master_cb_handle_map_[symbol_master_control_id] = handle;
	}
	void unsubscribe_symbol_master_event_handler(const int symbol_master_control_id)
	{
		auto found = symbol_master_cb_handle_map_.find(symbol_master_control_id);
		if (found == symbol_master_cb_handle_map_.end()) return;
		symbol_master_cb_list_.remove(found->second);
	}

	void process_symbol_master_event(std::shared_ptr<SmSymbol> symbol)
	{
		symbol_master_cb_list_(symbol);
	}
	/*
	void subscribe_position_event_handler(const int position_control_id, std::function<void(std::shared_ptr<Position> position)>&& handler)
	{
		PositionCBH handle = position_cb_list_.append(handler);
		position_cb_handle_map_[position_control_id] = handle;
	}
	void unsubscribe_position_event_handler(const int position_control_id)
	{
		auto found = position_cb_handle_map_.find(position_control_id);
		if (found == position_cb_handle_map_.end()) return;
		position_cb_list_.remove(found->second);
	}

	void process_position_event(std::shared_ptr<Position> position)
	{
		position_cb_list_(position);
	}
	*/

	void subscribe_order_event_handler(const int order_control_id, std::function<void(std::shared_ptr<Order> order, OrderEvent order_event)>&& handler)
	{
		OrderCBH handle = order_cb_list_.append(handler);
		order_cb_handle_map_[order_control_id] = handle;
	}
	void unsubscribe_order_event_handler(const int order_control_id)
	{
		auto found = order_cb_handle_map_.find(order_control_id);
		if (found == order_cb_handle_map_.end()) return;
		order_cb_list_.remove(found->second);
	}

	void process_order_event(std::shared_ptr<Order> order, OrderEvent order_event)
	{
		order_cb_list_(order, order_event);
	}


	void subscribe_stop_order_event_handler(const int order_control_id, std::function<void(std::shared_ptr<Order> order)>&& handler)
	{
		StopOrderCBH handle = stop_order_cb_list_.append(handler);
		stop_order_cb_handle_map_[order_control_id] = handle;
	}
	void unsubscribe_stop_order_event_handler(const int order_control_id)
	{
		auto found = stop_order_cb_handle_map_.find(order_control_id);
		if (found == stop_order_cb_handle_map_.end()) return;
		stop_order_cb_list_.remove(found->second);
	}

	void process_stop_order_event(std::shared_ptr<Order> order)
	{
		stop_order_cb_list_(order);
	}
	// add a window resize event to the event listener.
	void add_window_resize_event(const int window_id, std::function<void()>&& handler)
	{
		window_resize_event_.appendListener(window_id, handler);
	}
	void trigger_window_resize_event(const int window_id) {
		window_resize_event_.dispatch(window_id);
	}
	
	void add_parameter_event(const int window_id, std::function<void(const DarkHorse::OrderSetEvent& event, const std::string&, const bool)>&& handler)
	{
		parameter_dispatcher.appendListener(window_id, handler);
	}
	void trigger_parameter_event(const int window_id, const DarkHorse::OrderSetEvent& event, const std::string& s, const bool b)
	{
		parameter_dispatcher.dispatch(window_id, event, s, b);
	}
	
	void subscribe_account_profit_loss_event_handler(const int account_profit_loss_control_id, std::function<void()>&& handler)
	{
		AccountProfitLossCBH handle = account_profit_loss_cb_list_.append(handler);
		account_profit_loss_cb_handle_map_[account_profit_loss_control_id] = handle;
	}
	void unsubscribe_account_profit_loss_event_handler(const int account_profit_loss_control_id)
	{
		auto found = account_profit_loss_cb_handle_map_.find(account_profit_loss_control_id);
		if (found == account_profit_loss_cb_handle_map_.end()) return;
		account_profit_loss_cb_list_.remove(found->second);
	}
	void process_account_profit_loss_event()
	{
		account_profit_loss_cb_list_();
	}


	void add_ab_symbol_event(const int window_id, std::function<void(std::shared_ptr<SmSymbol> symbol)>&& handler)
	{
		ab_symbol_dispatcher.appendListener(window_id, handler);
	}
	void trigger_ab_symbol_event(const int window_id, std::shared_ptr<SmSymbol> symbol)
	{
		ab_symbol_dispatcher.dispatch(window_id, symbol);
	}

	void add_symbol_order_view_event(const int event_id, std::function<void(const int center_window_id, std::shared_ptr<SmSymbol> symbol)>&& handler)
	{
		symbol_order_view_event_dispatcher.appendListener(event_id, handler);
	}
	void trigger_symbol_order_view_event(const int event_id, const int center_window_id, std::shared_ptr<SmSymbol> symbol)
	{
		symbol_order_view_event_dispatcher.dispatch(event_id, center_window_id, symbol);
	}


	void subscribe_symbol_order_view_event_handler(const int control_id, std::function<void(const std::string& account_type, const int center_window_id, std::shared_ptr<SmSymbol> symbol)>&& handler)
	{
		SymbolOrderViewCBH handle = symbol_order_view_cb_list_.append(handler);
		symbol_order_view_cb_handle_map_[control_id] = handle;
	}
	void unsubscribe_symbol_order_view_event_handler(const int control_id)
	{
		auto found = symbol_order_view_cb_handle_map_.find(control_id);
		if (found == symbol_order_view_cb_handle_map_.end()) return;
		symbol_order_view_cb_list_.remove(found->second);
	}
	void process_symbol_order_view_event(const std::string& account_type, const int center_window_id, std::shared_ptr<SmSymbol> symbol)
	{
		symbol_order_view_cb_list_(account_type, center_window_id, symbol);
	}


	void subscribe_account_event_handler(const int control_id, std::function<void(const int source_window_id, std::shared_ptr<SmAccount> account)>&& handler)
	{
		AccountCBH handle = account_cb_list_.append(handler);
		account_cb_handle_map_[control_id] = handle;
	}

	void unsubscribe_account_event_handler(const int control_id)
	{
		auto found = account_cb_handle_map_.find(control_id);
		if (found == account_cb_handle_map_.end()) return;
		account_cb_list_.remove(found->second);
	}

	void process_account_event(const int source_window_id, std::shared_ptr<SmAccount> account)
	{
		account_cb_list_(source_window_id, account);
	}

	void subscribe_fund_event_handler(const int control_id, std::function<void(const int source_window_id, std::shared_ptr<SmFund> fund)>&& handler)
	{
		FundCBH handle = fund_cb_list_.append(handler);
		fund_cb_handle_map_[control_id] = handle;
	}

	void unsubscribe_fund_event_handler(const int control_id)
	{
		auto found = fund_cb_handle_map_.find(control_id);
		if (found == fund_cb_handle_map_.end()) return;
		fund_cb_list_.remove(found->second);
	}

	void process_fund_event(const int source_window_id, std::shared_ptr<SmFund> fund)
	{
		fund_cb_list_(source_window_id, fund);
	}

private:
	eventpp::EventDispatcher<int, void(const DarkHorse::OrderSetEvent& event, const std::string&, const bool)> parameter_dispatcher;

	eventpp::EventDispatcher<int, void(std::shared_ptr<SmSymbol> symbol)> ab_symbol_dispatcher;

	// event id, window id, symbol object.
	eventpp::EventDispatcher<int, void(const int center_window_id, std::shared_ptr<SmSymbol> symbol)> symbol_order_view_event_dispatcher;

	// window id, event handler
	eventpp::EventDispatcher<int, void()> window_resize_event_;

	OrderCBL order_cb_list_;
	std::map<int, OrderCBH> order_cb_handle_map_;

	HogaCBL hoga_cb_list_;
	std::map<int, HogaCBH> hoga_cb_handle_map_;

	QuoteCBL quote_cb_list_;
	std::map<int, QuoteCBH> quote_cb_handle_map_;

	TickCBL tick_cb_list_;
	std::map<int, TickCBH> tick_cb_handle_map_;
	// 맵을 만들어야 하고 해제 해야 한다. 
	SymbolCBL symbol_cb_list_;
	std::map<int, SymbolCBH> symbol_cb_handle_map_;

	ExpectedCBL expected_cb_list_;
	std::map<int, ExpectedCBH> expected_cb_handle_map_;

	SymbolMasterCBL symbol_master_cb_list_;
	std::map<int, SymbolMasterCBH> symbol_master_cb_handle_map_;

	PositionCBL position_cb_list_;
	std::map<int, PositionCBH> position_cb_handle_map_;

	StopOrderCBL stop_order_cb_list_;
	std::map<int, StopOrderCBH> stop_order_cb_handle_map_;

	AccountProfitLossCBL account_profit_loss_cb_list_;
	std::map<int, AccountProfitLossCBH> account_profit_loss_cb_handle_map_;

	AbSymbolCBL ab_symbol_cb_list_;
	std::map<int, AbSymbolCBH> ab_symbol_cb_handle_map_;

	SymbolOrderViewCBL symbol_order_view_cb_list_;
	std::map<int, SymbolOrderViewCBH> symbol_order_view_cb_handle_map_;

	AccountCBL account_cb_list_;
	std::map<int, AccountCBH> account_cb_handle_map_;

	FundCBL fund_cb_list_;
	std::map<int, FundCBH> fund_cb_handle_map_;
};
}

