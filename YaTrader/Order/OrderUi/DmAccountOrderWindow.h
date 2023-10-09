#pragma once
#include <memory>
#include <map>
#include <unordered_map>
#include "../SmLineGrid.h"
#include <vector>
#include "../OrderWndConst.h"
#include "../../Util/SmButton.h"
#include "../../Json/json.hpp"

using json = nlohmann::json;

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	struct SmPosition;

}


// SmMainOrderDialog dialog
class DmAccountOrderCenterWindow;
class DmAccountOrderLeftWindow;
class DmAccountOrderRightWindow;
class DmAccountOrderWindow : public CBCGPDialog
{
	DECLARE_DYNAMIC(DmAccountOrderWindow)

public:
	// Static Members
	static int _Id;
	static int GetId() { return _Id++; }

	void SetAccount();
	DmAccountOrderWindow(CWnd* pParent = nullptr);   // standard constructor
	DmAccountOrderWindow(CWnd* pParent, const size_t center_window_count, std::string& account_no);
	DmAccountOrderWindow(CWnd* pParent, const size_t center_window_count, std::string& account_no, const nlohmann::json center_wnd_prop);
	virtual ~DmAccountOrderWindow();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DM_ACNT_ORDDER_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	const std::map<int, std::shared_ptr<DmAccountOrderCenterWindow>>& get_center_window_map() {
		return center_window_map_;
	}
	void OnQuoteAreaShowHide();
	void RecalcChildren(CmdMode mode);
	void RecalcChildren2(CmdMode mode);
private:
	CBCGPButton _BtnLeft;
	CBCGPButton _BtnRight;
	nlohmann::json center_wnd_prop_;
	size_t center_window_count_{ 0 };
	std::string account_no_;
	int id_{0};
	bool destroyed_{ false };
	std::shared_ptr<DmAccountOrderLeftWindow> _LeftWnd = nullptr;
	std::shared_ptr<DmAccountOrderRightWindow> _RightWnd = nullptr;
	// key : window id, value : window object.
	std::map<int, std::shared_ptr<DmAccountOrderCenterWindow>> center_window_map_;
	bool _ShowLeft = true;
	bool _ShowRight = true;
	// key : combo index, value : account object.
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _ComboAccountMap;
	// key : account no, value : combo index.
	std::map<std::string, int> _AccountComboMap;
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
public:
	std::shared_ptr<DarkHorse::SmAccount> get_account() { return _Account; }
	std::string get_account_no();
	size_t get_center_window_count() { return center_window_map_.size(); }
	void on_symbol_view_event(const std::string& account_type, int center_window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void on_symbol_view_clicked(const int center_window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol);
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
	void saveToJson(json& j) const;
	void loadFromJson(const json& j);
};


