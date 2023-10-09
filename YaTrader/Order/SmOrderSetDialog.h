#pragma once
#include <BCGPDialog.h>
#include "../Event/EventHubArg.h"
// SmOrderSetDialog dialog
class AbAccountOrderCenterWindow;
class SmFundOrderCenterWnd;
class DmAccountOrderCenterWindow;
class SmOrderSetDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmOrderSetDialog)

public:
	SmOrderSetDialog(CWnd* pParent = nullptr);   // standard constructor
	SmOrderSetDialog(CWnd* pParent, const int& window_id_from, const DarkHorse::OrderSetEvent& setttings);   // standard constructor
	virtual ~SmOrderSetDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_SET };
#endif

	void OrderWnd(AbAccountOrderCenterWindow* val) { _OrderWnd = val; }
	void FundWnd(SmFundOrderCenterWnd* val) { _FundWnd = val; }
	void SetDmAccountWnd(DmAccountOrderCenterWindow* val) { _DmAccountOrderCenterWindow = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckSetWide();
	int get_id() {
		return id_;
	}
private:
	DarkHorse::OrderSetEvent order_set_event_;
	int id_{0};
	int window_id_from_{ 0 };
	AbAccountOrderCenterWindow* _OrderWnd = nullptr;
	SmFundOrderCenterWnd* _FundWnd = nullptr;
	void apply_change();
	DmAccountOrderCenterWindow* _DmAccountOrderCenterWindow = nullptr;
public:
	CBCGPButton check_show_symbol_tick_;
	CBCGPButton check_align_by_alt_;
	CBCGPButton check_show_bar_color_;
	CBCGPButton check_cancel_by_right_click_;
	CBCGPButton check_order_by_space_;
	CBCGPButton check_stop_by_real_;
	CBCGPButton check_show_order_column_;
	CBCGPButton check_show_stop_column_;
	CBCGPButton check_show_count_column_;
	CBCGPEdit edit_row_height_;
	CBCGPEdit edit_stop_width_;
	CBCGPEdit edit_order_width_;
	CBCGPEdit edit_count_width_;
	CBCGPEdit edit_qty_width_;
	CBCGPEdit edit_quote_width_;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckBarColor();
	afx_msg void OnBnClickedCheckAlignByAlt();
	afx_msg void OnBnClickedCheckOrderBySpace();
	afx_msg void OnBnClickedCheckCancelByRightClick();
	afx_msg void OnBnClickedCheckStopToReal();
	afx_msg void OnBnClickedCheckShowOrderCol();
	afx_msg void OnBnClickedCheckShowStopCol();
	afx_msg void OnBnClickedCheckShowCountCol();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedCheckShowSymbolTick();
};
