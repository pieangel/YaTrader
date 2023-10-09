#pragma once
#include <string>
#include <memory>
#include <map>
#include "../Order/SmOrderConst.h"
namespace DarkHorse {
	class SmAccount;
	class SmSymbol;
	class SmFund;
	class SymbolPositionControl;
	class QuoteControl;
	struct OrderRequest;
	class SmOutSystem
	{
	public:
		explicit SmOutSystem(const std::string& name);
		~SmOutSystem();
		std::string name() const { return name_; }
		void name(std::string val) { name_ = val; }
		std::shared_ptr<SmAccount> account() const { return account_; }
		void account(std::shared_ptr<SmAccount> val);
		int id() const { return id_; }
		void id(int val) { id_ = val; }
		DarkHorse::OrderType order_type() const { return order_type_; }
		void order_type(DarkHorse::OrderType val) { order_type_ = val; }

		std::string desc() const { return desc_; }
		void desc(std::string val) { desc_ = val; }
		std::shared_ptr<DarkHorse::SmSymbol> symbol() const { return symbol_; }
		void symbol(std::shared_ptr<DarkHorse::SmSymbol> val);
		std::shared_ptr<DarkHorse::SmFund> fund() const { return fund_; }
		void fund(std::shared_ptr<DarkHorse::SmFund> val);
		int seung_su() const { return seung_su_; }
		void seung_su(int val) { seung_su_ = val; }
		int signal_id() const { return signal_id_; }
		void signal_id(int val) { signal_id_ = val; }
		void put_order(const std::string& signal_name, int order_kind, int order_amount);
		std::shared_ptr<QuoteControl> quote_control() const { return quote_control_; }
		std::shared_ptr<SymbolPositionControl> position_control() const { return position_control_; }
		bool enable() const { return enable_; }
		void enable(bool val) { enable_ = val; }
		void liq_all();
	private:
		std::shared_ptr<QuoteControl> quote_control_;
		std::shared_ptr<SymbolPositionControl> position_control_;
		void put_order_each(std::shared_ptr<SmAccount> account, const std::string& signal_name, int order_kind, int order_amount);
		void SetOrderPrice(std::shared_ptr<OrderRequest> order_req);
		OrderType order_type_{ OrderType::None };
		int id_{ 0 };
		// Signal Name. "T1", "T2" for yes.
		std::string name_;
		std::shared_ptr<SmAccount> account_{ nullptr };
		std::shared_ptr<SmSymbol> symbol_{ nullptr };
		std::shared_ptr<SmFund> fund_{ nullptr };
		std::string desc_;
		int seung_su_{ 0 };
		int signal_id_{ 0 };
		bool enable_{ false };
	};
	// key : system id, value : SmOUtSystem object.
	typedef std::map<int, std::shared_ptr<SmOutSystem>> SmOutSystemMap;
}

