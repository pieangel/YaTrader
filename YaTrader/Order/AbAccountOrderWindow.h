#pragma once
#include <memory>
#include <map>
#include <unordered_map>
#include "SmLineGrid.h"
#include <vector>
#include "OrderWndConst.h"
#include "../Util/SmButton.h"
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	struct SmPosition;

}


// SmMainOrderDialog dialog
class AbAccountOrderCenterWindow;
class AbAccountOrderLeftWindow;
class AbAccountOrderRightWindow;
class AbAccountOrderWindow : public CBCGPDialog
{
	DECLARE_DYNAMIC(AbAccountOrderWindow)

public:
	void SetAccount();
	AbAccountOrderWindow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~AbAccountOrderWindow();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	const std::map<int, std::shared_ptr<AbAccountOrderCenterWindow>>& GetCenterWndMap() {
		return center_window_map_;
	}
	void OnQuoteAreaShowHide();
	void RecalcChildren(CmdMode mode);
	void RecalcChildren2(CmdMode mode);
private:
	bool destroyed_{ false };
	std::shared_ptr<AbAccountOrderLeftWindow> _LeftWnd = nullptr;
	std::shared_ptr<AbAccountOrderRightWindow> _RightWnd = nullptr;
	std::map<int, std::shared_ptr<AbAccountOrderCenterWindow>> center_window_map_;
	bool _ShowLeft = true;
	bool _ShowRight = true;
	// key : combo index, value : account object.
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _ComboAccountMap;
	int _LineGap = 4;
	bool _Init = false;
	void SetAccountForOrderWnd();
	CRect moveRect;
	int _CurrentAccountIndex{ 0 };
	void SetAccountInfo(std::shared_ptr<DarkHorse::SmAccount> account);
	CRect _rcMain;
	CRect _rcLeft;
	CRect _rcRight;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	int id_{ 0 };
public:
	int get_id() const { return id_; }
	void on_symbol_view_event(const std::string& account_type, int center_window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void on_symbol_view_clicked(const int center_window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void OnSymbolClickedFromOut(const int order_window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void OnSymbolClicked(const std::string& symbol_code);
	// 주문창을 추가한다.
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnRemove();
	afx_msg void OnBnClickedBtnLeft();
	afx_msg void OnBnClickedBtnRight();
	afx_msg LRESULT OnEnterSizeMove(WPARAM, LPARAM);
	afx_msg LRESULT OnExitSizeMove(WPARAM, LPARAM);
	CBCGPComboBox _ComboAccount;
	CStatic _StaticAccountName;
	SmLineGrid _Line1;
	SmLineGrid _Line2;
	SmLineGrid _Line3;
	SmLineGrid _Line4;
	SmLineGrid _Line5;
	SmLineGrid _Line6;
	SmLineGrid _Line7;
	SmLineGrid _Line8;
	SmLineGrid _Line9;
	SmLineGrid _Line10;
	std::vector<SmLineGrid*> _LineVector;
	afx_msg void OnStnClickedStaticAccountName();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCbnSelchangeComboAccount();
	afx_msg LRESULT OnUmOrderUpdate(WPARAM wParam, LPARAM lParam);
	void ChangedSymbol(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void ChangedCenterWindow(const int& center_wnd_id);
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnLiqAll();
	afx_msg LRESULT OnUmServerMsg(WPARAM wParam, LPARAM lParam);
	SmButton _StaticMsg;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	virtual void PostNcDestroy();
};
