#include "stdafx.h"
#include "SmAccountPositionManager.h"
#include "SmPosition.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
using namespace DarkHorse;

std::shared_ptr<DarkHorse::SmPosition> DarkHorse::SmAccountPositionManager::FindAddPosition(const std::string& symbol_code)
{
	auto it = _PositionMap.find(symbol_code);
	if (it != _PositionMap.end()) return it->second;

	auto position = std::make_shared<DarkHorse::SmPosition>();
	position->SymbolCode = symbol_code;
	position->AccountNo = _AccountNo;
	_PositionMap[symbol_code] = position;

	return position;
}

std::shared_ptr<DarkHorse::SmPosition> DarkHorse::SmAccountPositionManager::FindPosition(const std::string& symbol_code)
{
	auto found = _PositionMap.find(symbol_code);
	if (found == _PositionMap.end()) return nullptr;
	return found->second;
}

void DarkHorse::SmAccountPositionManager::UpdatePosition(const std::string& symbol_code)
{
	auto found = _PositionMap.find(symbol_code);
	if (found == _PositionMap.end()) return;

	

	auto symbol = mainApp.SymMgr()->FindSymbol(symbol_code);
	if (!symbol) return;

	auto position = found->second;
	if (position->OpenQty == 0) { SumAccountPL(); return; }

	const double open_pl = position->OpenQty * (symbol->Qoute.close - position->AvgPrice) * symbol->seung_su();

	const double open_pl_value = open_pl / pow(10, symbol->decimal());

	position->OpenPL = open_pl_value;

	SumAccountPL();
}

double DarkHorse::SmAccountPositionManager::SumAccountPL()
{
	auto account = mainApp.AcntMgr()->FindAccount(_AccountNo);
	if (!account) return 0;

	double open_pl = 0.0;
	double trade_pl = 0.0;
	double fee = 0.0;
	for (auto it = _PositionMap.begin(); it != _PositionMap.end(); ++it) {
		auto position = it->second;
		trade_pl += position->TradePL;
		open_pl += position->OpenPL;
		fee += position->TradeFee + position->UnsettledFee;
	}

	account->Asset.OpenProfitLoss = open_pl;
	account->Asset.TradeProfitLoss = trade_pl;
	account->Asset.Fee = fee;

	return open_pl;
}
