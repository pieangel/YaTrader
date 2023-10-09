#pragma once
#include <map>
namespace DarkHorse {
	class SmCenterValueMap
	{
		/// <summary>
		/// Key : Quote Value in integer. value : row index
		/// </summary>
		std::map<int, int> _QuoteToRowIndexMap;
		/// <summary>
		/// Key : row index, value : quote value.
		/// </summary>
		std::map<int, int> _RowIndexToPriceMap;
	};
}

