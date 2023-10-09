#pragma once
#include "BCGCBPro.h"
#include "../SmGrid/SmAcceptedArea.h"
#include "../SmGrid/SmFilledArea.h"
#include "../SmGrid/SmFavoriteArea.h"
#include "../SmGrid/SmAccountArea.h"
#include "SmCheckGrid.h"
#include "SmAcceptedGrid.h"
#include "SmPositionGrid.h"
#include "SmFavoriteGrid.h"
#include "../View/AccountOrderView.h"
#include "../View/AccountPositionView.h"
#include "../View/AccountProfitLossView.h"
#include "../View/FavoriteSymbolView.h"

// OrderLeftDialog dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}
class SmSymbolTableDialog;
class AbAccountOrderWindow;
class AbAccountOrderLeftWindow : public CBCGPDialog
{
	DECLARE_DYNAMIC(AbAccountOrderLeftWindow)

public:
	AbAccountOrderLeftWindow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~AbAccountOrderLeftWindow();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_LEFT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	AccountProfitLossView account_profit_loss_view_;
	AccountOrderView account_order_view_;
	AccountPositionView account_position_view_;
	FavoriteSymbolView favorite_symbol_view_;
	int order_window_id_ = 0;
public:
	void order_window_id(int id) { order_window_id_ = id; };
	void SetMainWnd(AbAccountOrderWindow* main_wnd);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnAddFav();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SetAccount(std::shared_ptr<DarkHorse::SmAccount> account);
	void SetFund(std::shared_ptr<DarkHorse::SmFund> fund);
	afx_msg LRESULT OnUmSymbolSelected(WPARAM wParam, LPARAM lParam);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	afx_msg void OnBnClickedBtnCancelSel();
	afx_msg void OnBnClickedBtnCancelAll();
	afx_msg void OnBnClickedBtnLiqSel();
	afx_msg void OnBnClickedBtnLiqAll();
	afx_msg void OnDestroy();
	CBCGPButton _BtnAddFav;
	CBCGPStatic _StaticFav;
};
