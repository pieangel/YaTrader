#pragma once
#include "../ViewModel/VmProduct.h"
#include "../Util/IdGenerator.h"
#include "../Symbol/MarketDefine.h"
#include <memory>
class SymbolOrderView;
namespace DarkHorse {
	class SmSymbol;
	class ProductControl
	{
	public:
		ProductControl();
		~ProductControl();
		void update_product(std::shared_ptr<SmSymbol> symbol);
		const VmProduct& get_product()
		{
			return product_;
		}
		const int get_id()
		{
			return id_;
		}
		void symbol_order_view(SymbolOrderView* symbol_order_view_p) {
			symbol_order_view_ = symbol_order_view_p;
		}
		int get_next_up_value(const int value);
		int get_next_down_value(const int value);
		int get_row(const int source_value, const int base_row, const int base_value);
		int get_value(const int source_row, const int base_row, const int base_value);
		ValueType get_value_type() const {
			return value_type_;
		}
	private:
		int get_row_with_turnig_point(const int source_value, const int base_row, const int base_value);
		int get_value_with_turnig_point(const int source_row, const int base_row, const int base_value);
		void set_option_value_turning_point();
		void set_value_type(const std::string& symbol_code);
		void subscribe_product_control();
		VmProduct product_;
		int id_;
		ValueType value_type_ = ValueType::None;
		SymbolOrderView* symbol_order_view_{ nullptr };
	};
}
