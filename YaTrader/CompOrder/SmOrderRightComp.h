#pragma once
// SmOrderRightComp dialog
#include "../SmGrid/SmAssetArea.h"
#include "../SmGrid/SmSymbolArea.h"
#include "../Order/SmOrderConst.h"
#include "SmOrderButton.h"
#include <memory>

// OrderRightDialog dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
}

class SmOrderCompMainDialog;
class SmOrderRightComp : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmOrderRightComp)

public:
	SmOrderRightComp(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmOrderRightComp();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_ORDER_RIGHT_COMP};
#endif


	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	SmAssetArea _AssetArea;
	SmSymbolArea _SymbolArea;

	SmOrderCompMainDialog* _MainWnd = nullptr;
public:
	SmOrderCompMainDialog* MainWnd() const { return _MainWnd; }
	void MainWnd(SmOrderCompMainDialog* val) { _MainWnd = val; }

	void SetAccount(const std::shared_ptr<DarkHorse::SmAccount>& account);
	void SetSymbol(const std::shared_ptr < DarkHorse::SmSymbol>& symbol);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
private:
	int _Slip = 0;
	DarkHorse::SmPriceType _PriceType = DarkHorse::SmPriceType::Price;
public:
	void DoOrder();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	afx_msg void OnBnClickedBtnOrderAll();

	SmOrderButton _OrderButton;
	CBCGPButton _RadioPrice;
	CBCGPButton _RadioMarket;
	CBCGPEdit _EditSlip;
	afx_msg void OnBnClickedRadioMarket();
	afx_msg void OnBnClickedRadioPrice();
	CBCGPSpinButtonCtrl _SpinSlip;
	afx_msg void OnEnChangeEditSlip();
};
