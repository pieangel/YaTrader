#include "stdafx.h"
#include "SymbolTickControl.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../View/SymbolOrderView.h"
#include "../Quote/SmQuote.h"
#include "../Util/IdGenerator.h"
#include "../View/SymbolTickView.h"

namespace DarkHorse {
	SymbolTickControl::SymbolTickControl()
		: id_(IdGenerator::get_id())
	{
		subscribe_tick_control();
		tick_vec_.resize(26);
	}

	SymbolTickControl::~SymbolTickControl()
	{
		mainApp.event_hub()->unsubscribe_tick_event_handler( id_ );
	}

	void SymbolTickControl::update_tick(SmTick tick)
	{
		if (symbol_id_ !=  0 && tick.symbol_id != symbol_id_) return;

		std::rotate(tick_vec_.rbegin(), tick_vec_.rbegin() + 1, tick_vec_.rend());
		tick_vec_[0] = std::move(tick);

		if (event_handler_) event_handler_();
	}

	void SymbolTickControl::subscribe_tick_control()
	{
		mainApp.event_hub()->subscribe_tick_event_handler
		(
			id_,
			std::bind(&SymbolTickControl::update_tick, this, std::placeholders::_1)
		);
	}

}