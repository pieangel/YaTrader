#pragma once


// SmFundCompLeftDialog dialog

#include "BCGCBPro.h"
#include "../SmGrid/SmAcceptedArea.h"
#include "../SmGrid/SmFilledArea.h"
#include "../SmGrid/SmFavoriteArea.h"
#include "../SmGrid/SmAccountArea.h"
#include "../Order/SmCheckGrid.h"
#include "../Order/SmAcceptedGrid.h"
#include "../Order/SmPositionGrid.h"
#include "../Order/SmFavoriteGrid.h"


namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
}
class SmSymbolTableDialog;
class SmFundCompMainDialog;
class SmFundCompLeftDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmFundCompLeftDialog)

public:
	SmFundCompLeftDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmFundCompLeftDialog();

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


	//SmFilledArea _FilledArea;
	//SmFavoriteArea _FavoriteArea;
	//SmAcceptedArea _AcceptedArea;
	//CBCGPScrollBar _VScrollBarAcpt;
	//CBCGPScrollBar _VScrollBarPosi;
	//CBCGPScrollBar _VScrollBarFav;
public:
	void SetMainWnd(SmFundCompMainDialog* main_wnd);
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
	CBCGPButton _BtnAddFav;
	CBCGPStatic _StaticFav;
	SmFavoriteGrid _FavoriteGrid;
};


