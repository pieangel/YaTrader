
#pragma once

#include "../SmGrid/SmAssetArea.h"
#include "../SmGrid/SmSymbolArea.h"
#include <memory>

// OrderRightDialog dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}


class SmFundOrderRight : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmFundOrderRight)

public:
	SmFundOrderRight(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmFundOrderRight();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_RIGHT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	SmAssetArea _AssetArea;
	SmSymbolArea _SymbolArea;
public:
	void SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account);
	void SetFund(const std::shared_ptr<DarkHorse::SmFund>& fund);
	void SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	void OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
