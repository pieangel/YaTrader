#pragma once
#include <memory>
#include <string>
namespace DarkHorse {
	class SmSymbol;
	struct VmOption {
		std::string account_no;
		std::string fund_name;
		std::string symbol_code;
		std::shared_ptr<SmSymbol> symbol_p{ nullptr };
		// 0 : none, 1 : call, 2 : put
		int call_put{ 0 };
		int symbol_id{ 0 };
		int close{ 0 };
		int expected{ 0 };
		int position{ 0 };
		int atm{ 0 };
		bool ordered{ false };
		std::string strike;
		int decimal{ 0 };
		int accepted_count{ 0 };
	};
}