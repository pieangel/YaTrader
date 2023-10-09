#include "stdafx.h"
#include "QuoteControl.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../Quote/SmQuote.h"
#include "../Log/MyLogger.h"

namespace DarkHorse {
	QuoteControl::QuoteControl()
		: id_(IdGenerator::get_id())
	{
		subscribe_quote_control();
	}

	QuoteControl::~QuoteControl()
	{
		mainApp.event_hub()->unsubscribe_quote_event_handler( id_ );
	}

	void QuoteControl::update_quote(std::shared_ptr<SmQuote> quote)
	{
		try {
			if (!quote) return;
			if (symbol_id_ != 0 && quote->symbol_id != symbol_id_) return;
			quote_.symbol_code = quote->symbol_code;
			quote_.symbol_id = quote->symbol_id;
			quote_.high = quote->high;
			quote_.low = quote->low;
			quote_.open = quote->open;
			quote_.close = quote->close;
			quote_.pre_day_close = quote->pre_day_close;

			if (event_handler_) event_handler_();
		}
		catch (const std::exception& e) {
			const std::string error = e.what();
			LOGINFO(CMyLogger::getInstance(), "error = %s", error.c_str());
		}
	}

	void QuoteControl::subscribe_quote_control()
	{
		mainApp.event_hub()->subscribe_quote_event_handler
		(
			id_,
			std::bind(&QuoteControl::update_quote, this, std::placeholders::_1)
		);
	}
	
}