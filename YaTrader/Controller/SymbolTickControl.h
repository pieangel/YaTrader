#pragma once
#include "../Quote/SmQuote.h"
#include <vector>
#include <functional>
class SymbolTickView;
namespace DarkHorse {
	class SymbolTickControl
	{
	public:
		SymbolTickControl();
		~SymbolTickControl();
		void update_tick(SmTick tick);
		
		const int get_id()
		{
			return id_;
		}
		void set_symbol_id(const int symbol_id) {
			symbol_id_ = symbol_id;
		}
		void set_symbol_decimal(const int symbol_decimal) {
			symbol_decimal_ = symbol_decimal;
		}
		const std::vector<SmTick>& get_tick_vec() {
			return tick_vec_;
		}
		int get_symbol_decimal() {
			return symbol_decimal_;
		}
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
	private:
		int symbol_decimal_{ 0 };
		std::vector<SmTick> tick_vec_;
		int symbol_id_{ 0 };
		void subscribe_tick_control();
		int id_;
		std::function<void()> event_handler_;
	};
}
