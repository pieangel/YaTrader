

#pragma once
#include "BCGCBPro.h"
#include "../SmGrid/SmAcceptedArea.h"
#include "../SmGrid/SmFilledArea.h"
#include "../SmGrid/SmFavoriteArea.h"
#include "../SmGrid/SmAccountArea.h"
#include "../Order/SmCheckGrid.h"
#include "../Order/SmAcceptedGrid.h"
#include "../Order/SmPositionGrid.h"
#include "../Order/SmFavoriteGrid.h"

// OrderLeftDialog dialog

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}
class SmSymbolTableDialog;
class SmFundOrderDialog;
class SmFundOrderLeft : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmFundOrderLeft)

public:
	SmFundOrderLeft(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmFundOrderLeft();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_LEFT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	SmAccountArea _AccountArea;

	SmAcceptedGrid _AcceptedGrid;
	SmPositionGrid _PositionGrid;
	SmFavoriteGrid _FavoriteGrid;

	SmFilledArea _FilledArea;

	
public:
	CBCGPButton _BtnAddFav;
	CBCGPStatic _StaticFav;
	void SetFundOrderWnd(SmFundOrderDialog* fund_order_wnd);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnAddFav();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SetAccount(std::shared_ptr<DarkHorse::SmAccount> account);
	void SetFund(std::shared_ptr < DarkHorse::SmFund> fund);
	afx_msg LRESULT OnUmSymbolSelected(WPARAM wParam, LPARAM lParam);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	afx_msg void OnBnClickedBtnCancelSel();
	afx_msg void OnBnClickedBtnCancelAll();
	afx_msg void OnBnClickedBtnLiqSel();
	afx_msg void OnBnClickedBtnLiqAll();
};
