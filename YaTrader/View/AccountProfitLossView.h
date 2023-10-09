/*
#pragma once
class AccountProfitLossView
{
};
*/

#pragma once

#include <memory>
#include <vector>
#include <string>

#include "../SmGrid/SmGridResource.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmFund;
	class AccountProfitLossControl;
}

class AccountProfitLossView : public CBCGPStatic
{
public:
	AccountProfitLossView();
	~AccountProfitLossView();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return fund_; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
public:
	void on_update_account_profit_loss();
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return account_; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	int order_window_id() const { return order_window_id_; }
	void order_window_id(int val) { order_window_id_ = val; }
	void update_account_profit_loss();
private:
	bool updating_ = false;
	int order_window_id_ = 0;
	std::string get_format_price(const double& value);
	void UpdateFundAssetInfo();
	bool enable_account_profit_loss_show_ = false;
	bool _EnableQuoteShow = false;
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> account_ = nullptr;
	std::shared_ptr<DarkHorse::SmFund> fund_ = nullptr;
	std::shared_ptr<DarkHorse::AccountProfitLossControl> account_profit_loss_control_;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};



