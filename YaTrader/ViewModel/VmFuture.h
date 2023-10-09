#pragma once
#include <memory>
#include <string>
namespace DarkHorse {
	class SmSymbol;
	struct VmFuture {
		std::string account_no;
		std::string fund_name;
		std::string symbol_code;
		std::shared_ptr<SmSymbol> symbol_p;
		int symbol_id{ 0 };
		int close{ 0 };
		int expected{ 0 };
		int position{ 0 };
		bool ordered{ false };
		int decimal{ 0 };
		int accepted_count{ 0 };
	};
}