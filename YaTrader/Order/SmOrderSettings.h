#pragma once
#include "SmOrderConst.h"
namespace DarkHorse {
struct SmOrderSettings {
	bool LossCut;
	int LossCutTick;
	bool ProfitCut;
	int ProfitCutTick;
	int SlipTick;
	SmPriceType PriceType = SmPriceType::Price;
};
}