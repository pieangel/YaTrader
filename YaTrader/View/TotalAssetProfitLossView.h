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
	class AccountAssetControl;
	class AccountProfitLossControl;
}

class TotalAssetProfitLossView : public CBCGPStatic
{
public:
	TotalAssetProfitLossView();
	~TotalAssetProfitLossView();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return fund_; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
public:
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return account_; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	void SetAssetInfo();
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void on_update_account_profit_loss();
private:
	void update_account_profit_loss();
	void SetAccountAssetInfo();
	void SetFundAssetInfo();
	// 0 : account, 1 : fund
	int _Mode = 0;
	bool _EnableOrderShow = false;
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
	std::shared_ptr<DarkHorse::AccountAssetControl> asset_control_;

	bool enable_account_profit_loss_show_ = false;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

