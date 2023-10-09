#pragma once
#include <string>
namespace DarkHorse 
{
struct SmHoga
{
	struct	SmHogaItem
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
	int symbol_id{ 0 };
	std::string symbol_code;
};
}

