#pragma once
#include <string>
namespace DarkHorse {
	struct SmQuote
	{
		std::string symbol_code;
		int symbol_id{ 0 };
		int high{ 0 };
		int low{ 0 };
		int close{ 0 };
		int open{ 0 };
		int pre_day_close{ 0 };
		int expected{ 0 };
		double preday_cmp{ 0.0 };
	};

	struct SmTick 
	{
		int symbol_id{ 0 };
		std::string time;
		int close{ 0 };
		int qty{ 0 };
		int updown{ 0 };
	};
}

