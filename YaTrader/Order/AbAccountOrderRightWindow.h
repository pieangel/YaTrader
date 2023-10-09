#pragma once

#include "../SmGrid/SmAssetArea.h"
#include "../SmGrid/SmSymbolArea.h"
#include "../View/AccountAssetView.h"
#include "../View/SymbolInfoView.h"
#include <memory>

// OrderRightDialog dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}


class AbAccountOrderRightWindow : public CBCGPDialog
{
	DECLARE_DYNAMIC(AbAccountOrderRightWindow)

public:
	AbAccountOrderRightWindow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~AbAccountOrderRightWindow();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_RIGHT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	AccountAssetView asset_view_;
	SymbolInfoView symbol_info_view_;
	int order_window_id_{ 0 };
public:
	int order_window_id() const { return order_window_id_; }
	void order_window_id(int val) { order_window_id_ = val; }
	void SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account);
	void SetFund(std::shared_ptr<DarkHorse::SmFund> fund);
	void SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	void OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
