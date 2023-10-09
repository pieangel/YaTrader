#include "stdafx.h"
#include "ProductControl.h"
#include "../Global/SmTotalManager.h"
#include "../Event/EventHub.h"
#include "../View/SymbolOrderView.h"
#include "../Symbol/SmSymbol.h"
#include <functional>

namespace DarkHorse {
	ProductControl::ProductControl()
		: id_(IdGenerator::get_id())
	{
		
	}

	ProductControl::~ProductControl()
	{
		
	}

	int ProductControl::get_next_up_value(const int value)
	{
		if (value_type_ == ValueType::None) return value;

		if (value_type_ == ValueType::KospiOption ||
			value_type_ == ValueType::KospiWeekly) {
			if (value >= 1000) return value + 5;
			else return value + 1;
		}
		else if (value_type_ == ValueType::MiniKospiOption) {
			if (value >= 300) return value + 2;
			else return value + 1;
		}
		else if (value_type_ == ValueType::Kosdaq) {
			return value + 10;
		}
		else {
			return value + product_.int_tick_size;
		}
	}

	int ProductControl::get_next_down_value(const int value)
	{
		if (value_type_ == ValueType::None) return value;

		if (value_type_ == ValueType::KospiOption ||
			value_type_ == ValueType::KospiWeekly) {
			if (value <= 1000) return value - 1;
			else return value - 5;
		}
		else if (value_type_ == ValueType::MiniKospiOption) {
			if (value <= 300) return value - 1;
			else return value - 2;
		}
		else if (value_type_ == ValueType::Kosdaq) {
			return value - 10;
		}
		else {
			return value - product_.int_tick_size;
		}
	}

	int ProductControl::get_row(const int source_value, const int base_row, const int base_value)
	{
		if (value_type_ == ValueType::None || base_value == 0) return -1;
		if (source_value == base_value) return base_row;

		CString msg;
		msg.Format("sour_value = %d, base_row = %d, base_value = %d, int_tick_size = %d\n", source_value, base_row, base_value, product_.int_tick_size);
		TRACE(msg);

		if (value_type_ == ValueType::KospiOption ||
			value_type_ == ValueType::KospiWeekly || 
			value_type_ == ValueType::MiniKospiOption) {
			return get_row_with_turnig_point(source_value, base_row, base_value);
		}
		else {
			if (source_value > base_value) {
				const int difference_value = source_value - base_value;
				return base_row - static_cast<int>(difference_value / product_.int_tick_size);
			}
			else {
				const int difference_value = base_value - source_value;
				return base_row + static_cast<int>(difference_value / product_.int_tick_size);
			}
		}
	}

	int ProductControl::get_value(const int source_row, const int base_row, const int base_value)
	{
		if (value_type_ == ValueType::None || base_value == 0) base_value;
		if (source_row == base_row) return base_value;

		if (value_type_ == ValueType::KospiOption ||
			value_type_ == ValueType::KospiWeekly ||
			value_type_ == ValueType::MiniKospiOption) {
			return get_value_with_turnig_point(source_row, base_row, base_value);
		}
		else {
			return  base_value + product_.int_tick_size * (base_row - source_row);
		}
	}

	int ProductControl::get_row_with_turnig_point(const int source_value, const int base_row, const int base_value)
	{
		if (base_value > product_.option_value_turning_point && 
			source_value > product_.option_value_turning_point) {
			if (source_value > base_value) {
				const int difference_value = source_value - base_value;
				return base_row - static_cast<int>(difference_value / product_.int_tick_size);
			}
			else {
				const int difference_value = base_value - source_value;
				return base_row + static_cast<int>(difference_value / product_.int_tick_size);
			}
		}
		else if (base_value < product_.option_value_turning_point && 
			source_value < product_.option_value_turning_point) {
			if (source_value > base_value) {
				const int difference_value = source_value - base_value;
				return base_row - difference_value;
			}
			else {
				const int difference_value = base_value - source_value;
				return base_row + difference_value;
			}
		}
		else {
			if (source_value > base_value) {
				const int difference_value_greater_than = source_value - product_.option_value_turning_point;
				const int difference_value_less = product_.option_value_turning_point - base_value;
				int difference_row = difference_value_less + static_cast<int>(difference_value_greater_than / 5);
				return base_row - difference_row;
			}
			else {
				const int difference_value_greater_than = base_value - product_.option_value_turning_point;
				const int difference_value_less = product_.option_value_turning_point - source_value;
				const int difference_row = difference_value_less + static_cast<int>(difference_value_greater_than / 5);
				return base_row + difference_row;
			}
		}
	}

	int ProductControl::get_value_with_turnig_point(const int source_row, const int base_row, const int base_value)
	{
		const int turning_point_row = get_row(product_.option_value_turning_point, base_row, base_value);
		if (base_row < turning_point_row &&
			source_row < turning_point_row) {
			if (source_row > base_row) {
				const int difference_row = source_row - base_row;
				return base_value - difference_row * product_.int_tick_size;
			}
			else {
				const int difference_row = base_row - source_row;
				return base_value + difference_row * product_.int_tick_size;
			}
		}
		else if (base_row > turning_point_row &&
			source_row > turning_point_row) {
			if (source_row > base_row) {
				const int difference_row = source_row - base_row;
				return base_value - difference_row;
			}
			else {
				const int difference_row = base_row - source_row;
				return base_value + difference_row;
			}
		}
		else {
			if (source_row > base_row) {
				const int difference_row_less = source_row - turning_point_row;
				const int difference_row_greater_than = turning_point_row - base_row;
				const int difference_value = difference_row_less + difference_row_greater_than * 5;
				return base_value - difference_value;
			}
			else {
				const int difference_row_less = base_row - turning_point_row;
				const int difference_row_greater_than = turning_point_row - source_row;
				const int difference_value = difference_row_less + difference_row_greater_than * 5;
				return base_value + difference_value;
			}
		}
	}

	void ProductControl::set_option_value_turning_point()
	{
		if (value_type_ == ValueType::KospiOption ||
			value_type_ == ValueType::KospiWeekly)
			product_.option_value_turning_point = kospi_option_turning_point;
		else if (value_type_ == ValueType::MiniKospiOption)
			product_.option_value_turning_point = mini_kospi_option_turning_point;
	}

	void ProductControl::set_value_type(const std::string& symbol_code)
	{
		if (symbol_code.empty()) return;

		if (symbol_code.find("201") != std::string::npos ||
			symbol_code.find("301") != std::string::npos) 
			value_type_ = ValueType::KospiOption;
		else if (
			symbol_code.find("209") != std::string::npos ||
			symbol_code.find("309") != std::string::npos || 
			symbol_code.find("2AF") != std::string::npos ||
			symbol_code.find("3AF") != std::string::npos)
			value_type_ = ValueType::KospiWeekly;
		else if (symbol_code.find("205") != std::string::npos ||
			symbol_code.find("305") != std::string::npos)
			value_type_ = ValueType::MiniKospiOption;
		else if (symbol_code.find("206") != std::string::npos ||
			symbol_code.find("306") != std::string::npos)
			value_type_ = ValueType::Kosdaq;
		else
			value_type_ = ValueType::Future;
	}

	void ProductControl::subscribe_product_control()
	{
		
	}
	void ProductControl::update_product(std::shared_ptr<SmSymbol> symbol)
	{
		if (!symbol) return;

		product_.decimal = symbol->decimal();
		product_.tick_size = symbol->TickSize();
		product_.int_tick_size = static_cast<int>(symbol->TickSize() * pow(10, symbol->decimal()));
		product_.seung_su = symbol->seung_su();
		product_.tick_value = static_cast<int>(symbol->TickValue());
		set_value_type(symbol->SymbolCode());
		set_option_value_turning_point();
	}
}