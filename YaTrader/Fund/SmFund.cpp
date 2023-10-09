#include "stdafx.h"
#include "SmFund.h"
#include "../Account/SmAccount.h"
#include "../Global/SmTotalManager.h"
#include "../Account/SmAccountManager.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/Position.h"
#include "../Order/SmSymbolOrderManager.h"

namespace DarkHorse {

	SmFund::SmFund()
	{
		position_ = std::make_shared<Position>();
	}

	std::shared_ptr<SmAccount> SmFund::FindAccount(const std::string& account_no)
	{
		for (auto it = _AccountVector.begin(); it != _AccountVector.end(); it++) {
			if ((*it)->No().compare(account_no) == 0) {
				return *it;
			}
		}

		return nullptr;
	}

	void SmFund::AddAccount(const std::shared_ptr<SmAccount>& account)
	{
		if (!account) return;

		if (FindAccount(account->No())) return;
		account->UsedForFund(true);
		account->fund_name(_Name);
		fund_type_ = account->Type();
		_AccountVector.push_back(account);
	}

	void SmFund::RemoveAccount(const std::string& account_no)
	{
		for (auto it = _AccountVector.begin(); it != _AccountVector.end(); it++) {
			if ((*it)->No().compare(account_no) == 0) {
				(*it)->UsedForFund(false);
				(*it)->fund_name("");
				_AccountVector.erase(it);
				return;
			}
		}
	}

	void SmFund::RemoveAllAccounts()
	{
		for (auto it = _AccountVector.begin(); it != _AccountVector.end(); it++) {
			(*it)->UsedForFund(false);
			(*it)->fund_name("");
		}

		_AccountVector.clear();
	}

	int SmFund::GetRatioPercent(const std::string account_no)
	{
		auto account = FindAccount(account_no);
		if (!account) return 0;
		if (GetTotalSeungSu() == 0) return 0;

		double seungsu_ratio = (double)(account->SeungSu()) / (double)(GetTotalSeungSu());
		int seungsu_percent = static_cast<int>(seungsu_ratio * 100);
		return seungsu_percent;
	}

	int SmFund::GetTotalSeungSu()
	{
		if (_AccountVector.empty()) return 0;

		int sum = 0;

		for (auto it = _AccountVector.begin(); it != _AccountVector.end(); it++) {
			sum += (*it)->SeungSu();
		}

		return sum;
	}

	void SmFund::MoveUp(const int& target_index)
	{
		int size = _AccountVector.size();
		if (target_index < 1 || target_index >= size)
			return;

		std::swap(_AccountVector[target_index], _AccountVector[target_index - 1]);
	}

	void SmFund::MoveDown(const int& target_index)
	{
		int size = _AccountVector.size();
		if (target_index < 0 || target_index >= size - 1)
			return;

		std::swap(_AccountVector[target_index], _AccountVector[target_index + 1]);
	}

	int SmFund::GetPositionCount(const std::string& symbol_code)
	{
		int fund_open_qty = 0;

		const std::vector<std::shared_ptr<SmAccount>>& account_vec = GetAccountVector();
		if (account_vec.empty()) return fund_open_qty;

// 		for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
// 			auto account = (*it);
// 			std::shared_ptr<Position> position = mainApp.total_position_manager()->get_account_position_manager(account->No(), symbol_code);
// 			fund_open_qty += position->OpenQty;
// 		}

		return fund_open_qty;
	}

	int SmFund::GetFilledOrderCount(const std::string& symbol_code)
	{
		int fund_filled_qty = 0;

		const std::vector<std::shared_ptr<SmAccount>>& account_vec = GetAccountVector();
		if (account_vec.empty()) return fund_filled_qty;

		for (auto it = account_vec.begin(); it != account_vec.end(); ++it) {
			auto account = (*it);
			auto symbol_order_mgr = mainApp.TotalOrderMgr()->FindAddSymbolOrderManager(account->No(), symbol_code);
			if (!symbol_order_mgr) continue;

			int filled_count = symbol_order_mgr->GetUnsettledCount();
			fund_filled_qty += filled_count;
		}

		return fund_filled_qty;
	}

	void SmFund::SaveToXml(pugi::xml_node& node_fund)
	{
		pugi::xml_node fund_child = node_fund.append_child("fund_name");
		fund_child.append_child(pugi::node_pcdata).set_value(_Name.c_str());
		fund_child = node_fund.append_child("fund_account_list");
		if (_AccountVector.empty()) return;
		for (auto it = _AccountVector.begin(); it != _AccountVector.end(); ++it) {
			std::shared_ptr<DarkHorse::SmAccount> fund_account = *it;
			pugi::xml_node node_fund_account = fund_child.append_child("fund_account");
			node_fund_account.append_child(pugi::node_pcdata).set_value(fund_account->No().c_str());
		}

	}

	void SmFund::LoadFromXml(pugi::xml_node& node_fund)
	{
		//_Name = node_fund.child_value("fund_name");
		pugi::xml_node fund_account_list_node = node_fund.child("fund_account_list");
		if (fund_account_list_node) {
			for (pugi::xml_node fund_account_node = fund_account_list_node.child("fund_account"); fund_account_node; fund_account_node = fund_account_node.next_sibling("fund_account")) {
				std::string fund_account_no = fund_account_node.child_value();
				auto fund_account = mainApp.AcntMgr()->FindAccount(fund_account_no);
				if (fund_account) {
					fund_account->UsedForFund(true);
					_AccountVector.push_back(fund_account);
				}
			}
		}
	}

	bool SmFund::is_account_exist(const int& account_id)
	{
		auto it = account_id_set_.find(account_id);
		if (it == account_id_set_.end()) return false;
		return true;
	}

	std::shared_ptr<DarkHorse::SmAccount> SmFund::get_first_account()
	{
		if (_AccountVector.empty()) return nullptr;

		if (_AccountVector[0]->is_subaccount()) {
			return _AccountVector[0]->parent_account().lock() ? _AccountVector[0]->parent_account().lock() : nullptr;
		}
		else {
			return _AccountVector[0];
		}
	}

} // namespace DarkHorse
