#pragma once
#include <string>
namespace DarkHorse {
	struct SmWndEvent {
		int cmd = 0;
		int SymbolId = 0;
		int AccountId = 0;
		std::string OrderNo;
	};
}