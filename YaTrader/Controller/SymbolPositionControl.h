#pragma once
#include "../ViewModel/VmPosition.h"
#include "../Util/IdGenerator.h"
#include "../Position/PositionConst.h"
#include "../Order/SmOrderConst.h"
#include "../Symbol/SmSymbol.h"
#include "../ViewModel/VmPosition.h"
#include <memory>
#include <functional>
#include <map>
#include <string>
namespace DarkHorse {
	struct Position;
	struct SmQuote;
	class SmAccount;
	class SmFund;
	class SmSymbol;
	class SymbolPositionControl
	{
	public:
		SymbolPositionControl();
		~SymbolPositionControl();
		void update_profit_loss(std::shared_ptr<SmQuote> quote);
		void update_position(std::shared_ptr<Position> position);
		const VmPosition& get_position()
		{
			return position_;
		}
		const int get_id()
		{
			return id_;
		}
		void set_symbol(std::shared_ptr<SmSymbol> symbol);
		//void set_account_id(const int account_id);
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
		OrderType Position_type() const { return position_type_; }
		void Position_type(OrderType val) { position_type_ = val; }
		void set_account(std::shared_ptr<SmAccount> account);
		void set_fund(std::shared_ptr<SmFund> fund);
		void set_out_system_event_handler(std::function<void(const int)> out_system_event_handler) {
			out_system_event_handler_ = out_system_event_handler;
		}
		void set_out_system_id(const int out_system_id) {
			out_system_id_ = out_system_id;
		};

		void set_fund_option_event_handler(std::function<void(std::shared_ptr<Position> position)> fund_option_event_handler) {
			fund_option_event_handler_ = fund_option_event_handler;
		}
		void set_vm_option_event_handler(std::function<void(const VmPosition& position)> vm_option_event_handler) {
			vm_option_event_handler_ = vm_option_event_handler;
		}
		void set_vm_fund_event_handler(std::function<void(const VmPosition& position)> vm_fund_event_handler) {
			vm_fund_event_handler_ = vm_fund_event_handler;
		}
		void reset_position();
	private:
		bool is_account_exist(const std::shared_ptr<Position>& position);
		OrderType position_type_{ OrderType::None };
		// key : account no, value : position object.
		std::map<std::string, std::shared_ptr<Position>> position_map_;
		// key : account no, value : account object
		std::map<std::string, std::shared_ptr<SmAccount>> account_map_;
		int symbol_seung_su_{ 1 };
		int symbol_decimal_{ 1 };
		int id_{ 0 };
		int symbol_id_{ 0 };
		int account_id_{ 0 };
		int fund_id_{ 0 };
		int out_system_id_{ 0 };
		std::shared_ptr<SmAccount> account_{ nullptr };
		std::shared_ptr<SmFund> fund_{ nullptr };
		std::shared_ptr<SmSymbol> symbol_{ nullptr };
		VmPosition position_;
		void subscribe_position_control();
		void clear_position();
		std::function<void()> event_handler_;
		std::function<void(const int)> out_system_event_handler_;
		std::function<void(std::shared_ptr<Position> position)> fund_option_event_handler_;
		std::function<void(const VmPosition& position)> vm_option_event_handler_;
		std::function<void(const VmPosition& position)> vm_fund_event_handler_;
	};
}
