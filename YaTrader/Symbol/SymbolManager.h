#pragma once
#include <map>
#include <string>
#include "Symbol.h"
namespace DarkHorse {
class SymbolManager
{
public:
	static int create_id() { return ++_id; }
private:
	static int _id;
public:
	void read_domestic_masterfile();
	void add_symbol(Symbol&& symbol);
private:
	std::map<std::string, Symbol> symbol_map_;
};
}

