#include "stdafx.h"
#include "SmFundManager.h"
#include "SmFund.h"
#include "../Util/IdGenerator.h"

using namespace DarkHorse;

int SmFundManager::_Id = 0;

DarkHorse::SmFundManager::SmFundManager()
{
	Init();
}

std::shared_ptr<DarkHorse::SmFund> SmFundManager::FindAddFund(const std::string& fund_name, const std::string fund_type)
{
	auto found = _FundMap.find(fund_name);
	if (found != _FundMap.end())
		return found->second;
	else {
		std::shared_ptr<SmFund> fund = std::make_shared<SmFund>(IdGenerator::get_id(), fund_name);
		fund->fund_type(fund_type);
		_FundMap[fund_name] = fund;
		_FundIdMap[fund->Id()] = fund;
		return fund;
	}
}

void DarkHorse::SmFundManager::RemoveFund(const std::string& fund_name)
{
	auto found = _FundMap.find(fund_name);
	if (found == _FundMap.end()) return;

	found->second->RemoveAllAccounts();

	_FundMap.erase(found);
}

void DarkHorse::SmFundManager::Init()
{
	FindAddFund("Fund1", "9");
	FindAddFund("Fund2", "9");
	FindAddFund("Fund3", "9");
	FindAddFund("Fund4", "9");
}

std::shared_ptr<DarkHorse::SmFund> DarkHorse::SmFundManager::FindFundById(const int& fund_id)
{
	auto found = _FundIdMap.find(fund_id);

	if (found != _FundIdMap.end()) return found->second;

	return nullptr;
}

std::shared_ptr<DarkHorse::SmFund> DarkHorse::SmFundManager::FindFund(const std::string& fund_name)
{
	auto found = _FundMap.find(fund_name);
	if (found != _FundMap.end())
		return found->second;
	return nullptr;
}

void DarkHorse::SmFundManager::SaveToXml(pugi::xml_node& node_fund_list)
{
	for (auto it = _FundMap.begin(); it != _FundMap.end(); ++it) {
		std::shared_ptr<DarkHorse::SmFund> fund = it->second;
		pugi::xml_node node_fund = node_fund_list.append_child("fund");
		fund->SaveToXml(node_fund);
	}
}

void DarkHorse::SmFundManager::LoadFromXml(pugi::xml_node& node_fund_list)
{
	for (pugi::xml_node fund_node = node_fund_list.child("fund"); fund_node; fund_node = fund_node.next_sibling("fund")) {
		std::string fund_name = fund_node.child_value("fund_name");
		std::string fund_type = fund_node.child_value("fund_type");
		std::shared_ptr<DarkHorse::SmFund> fund = FindAddFund(fund_name, fund_type);
		fund->LoadFromXml(fund_node);
		_FundMap[fund->Name()] = fund;
	}
}
