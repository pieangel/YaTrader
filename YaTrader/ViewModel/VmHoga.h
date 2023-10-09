#pragma once
#include <string>
namespace DarkHorse
{
	struct VmHoga
	{
		struct	VmHogaItem
		{
			int	BuyCnt;
			int	BuyPrice;
			int	BuyQty;
			int	SellCnt;
			int	SellPrice;
			int	SellQty;
		}	Ary[5];

		int	TotSellQty;
		int	TotBuyQty;
		int	TotSellCnt;
		int	TotBuyCnt;
		std::string HogaTime;
	};
}
