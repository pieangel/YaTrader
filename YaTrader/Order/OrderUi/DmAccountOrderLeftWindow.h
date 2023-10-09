//#pragma once
/*
class DmAccountOrderLeftWindow
{
};
*/

#pragma once
#include "BCGCBPro.h"
#include "../../SmGrid/SmAcceptedArea.h"
#include "../../SmGrid/SmFilledArea.h"
#include "../../SmGrid/SmFavoriteArea.h"
#include "../../SmGrid/SmAccountArea.h"
#include "../SmCheckGrid.h"
#include "../SmAcceptedGrid.h"
#include "../SmPositionGrid.h"
#include "../SmFavoriteGrid.h"
#include "../../View/AccountOrderView.h"
#include "../../View/AccountPositionView.h"
#include "../../View/AccountProfitLossView.h"
#include "../../View/DmOptionView.h"
#include "../../View/DmFutureView.h"
#include "../../View/AccountAssetView.h"
#include <map>
#include <string>

// DmAccountOrderLeftWindow dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}
class SmSymbolTableDialog;
class DmAccountOrderWindow;
class DmAccountOrderLeftWindow : public CBCGPDialog
{
	DECLARE_DYNAMIC(DmAccountOrderLeftWindow)

public:
	DmAccountOrderLeftWindow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DmAccountOrderLeftWindow();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DM_ACNT_ORDER_LEFT};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	void set_option_view();
	// key : option year-month combo index, value : year-month name
	std::map<int, std::string> option_yearmonth_index_map;
	int year_month_index{ 0 };
	int option_market_index{ 0 };
	void init_option_market();
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	AccountProfitLossView account_profit_loss_view_;
	AccountAssetView asset_view_;
	DmOptionView option_view_;
	DmFutureView future_view_;
	int order_window_id_{ 0 };
public:
	int order_window_id() const { return order_window_id_; }
	void order_window_id(int val) { order_window_id_ = val; }
	void SetMainWnd(DmAccountOrderWindow* main_wnd);
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
	CBCGPButton _BtnAddFav;
	CBCGPStatic _StaticFav;
	CBCGPComboBox combo_option_market_;
	CBCGPComboBox combo_option_month_;
	afx_msg void OnCbnSelchangeComboOptionMarket();
	afx_msg void OnCbnSelchangeComboOptionMonth();
	afx_msg void OnBnClickedRadioClose();
	afx_msg void OnBnClickedRadioPosition();
	afx_msg void OnBnClickedRadioExpected();
	afx_msg void OnDestroy();
	CBCGPButton radio_close_;
	CBCGPButton radio_position_;
	CBCGPButton ratio_expected_;
};


