#include "stdafx.h"
#include "SmTotalPositionManager.h"
#include "SmAccountPositionManager.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountAsset.h"
#include "../Account/SmAccountManager.h"
using namespace DarkHorse;

std::shared_ptr<DarkHorse::SmAccountPositionManager> DarkHorse::SmTotalPositionManager::FindAddAccountPositionManager(std::string account_no)
{
	const auto it = _AccountPositionMap.find(account_no);
	if (it != _AccountPositionMap.end()) return it->second;

	auto account_position_mgr = std::make_shared<SmAccountPositionManager>(account_no);

	account_position_mgr->AccountNo(account_no);

	_AccountPositionMap[account_no] = account_position_mgr;

	return account_position_mgr;
}

std::shared_ptr<DarkHorse::SmPosition> DarkHorse::SmTotalPositionManager::FindAddPosition(const std::string& account_no, const std::string& symbol_code)
{
	std::shared_ptr<DarkHorse::SmAccountPositionManager> acnt_position_mgr = FindAddAccountPositionManager(account_no);
	return acnt_position_mgr->FindAddPosition(symbol_code);
	//return nullptr;
}

std::shared_ptr<DarkHorse::SmPosition> DarkHorse::SmTotalPositionManager::FindPosition(const std::string& account_no, const std::string& symbol_code)
{
	auto found = _AccountPositionMap.find(account_no);
	if (found == _AccountPositionMap.end()) return nullptr;

	return found->second->FindPosition(symbol_code);
}

void DarkHorse::SmTotalPositionManager::UpdatePosition(const std::string& symbol_code)
{
	for (auto it = _AccountPositionMap.begin(); it != _AccountPositionMap.end(); ++it) {
		it->second->UpdatePosition(symbol_code);
	}
}
