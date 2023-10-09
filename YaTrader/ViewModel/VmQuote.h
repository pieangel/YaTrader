#pragma once
#include <string>
namespace DarkHorse {
	struct VmQuote
	{
		int high{ 0 };
		int low{ 0 };
		int close{ 0 };
		int open{ 0 };
		int pre_day_close{ 0 };
		std::string symbol_code;
		int symbol_id{ 0 };
	};
}
