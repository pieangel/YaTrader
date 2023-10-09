#pragma once
#include "../ViewModel/VmQuote.h"
#include "../Util/IdGenerator.h"
#include "../Symbol/SymbolConst.h"
#include <memory>
#include <functional>

namespace DarkHorse {
	struct SmQuote;
class QuoteControl
{
public:
	QuoteControl();
	~QuoteControl();
	void update_quote(std::shared_ptr<SmQuote> quote);
	const VmQuote& get_quote()
	{
		return quote_;
	}
	const int get_id()
	{
		return id_;
	}
	void set_symbol_id(const int symbol_id) {
		symbol_id_ = symbol_id;
	}
	void set_event_handler(std::function<void()> event_handler) {
		event_handler_= event_handler;
	}
	DarkHorse::SymbolType symbol_type() const { return symbol_type_; }
	void symbol_type(DarkHorse::SymbolType val) { symbol_type_ = val; }
private:
	DarkHorse::SymbolType symbol_type_{ DarkHorse::SymbolType::None };
	int id_;
	int symbol_id_{ 0 };
	VmQuote quote_;
	void subscribe_quote_control();
	std::function<void()> event_handler_;
};
}

