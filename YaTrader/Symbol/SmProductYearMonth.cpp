#include "stdafx.h"
#include "SmProductYearMonth.h"
#include "SmSymbol.h"
#include <algorithm>
namespace DarkHorse {

void SmProductYearMonth::sort_symbol_vector()
{
	if (_SymbolVec.size() == 0) return;

	//std::sort(_SymbolVec.begin(), _SymbolVec.end(), DarkHorse::strike_less_than_key());

	std::sort(_SymbolVec.begin(), _SymbolVec.end(), [](const std::shared_ptr<SmSymbol>& lhs, const std::shared_ptr<SmSymbol>& rhs)
	{
		return lhs->Strike() < rhs->Strike();
	});
}
}
