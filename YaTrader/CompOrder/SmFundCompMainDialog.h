
// SmFundCompMainDialog dialog


#pragma once
#include <BCGCBProInc.h>

#include <memory>
#include <map>
#include <unordered_map>
#include "../Order/SmLineGrid.h"
#include <vector>
#include "../Order/OrderWndConst.h"
#include "../Util/SmButton.h"
#include "../Order/SmOrderConst.h"
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	struct SmPosition;
	class SmFund;
}


// SmMainOrderDialog dialog
class SmCompOrderDialog;
class SmFundCompLeftDialog;
class SmFundCompRightDialog;
class SmFundCompMainDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmFundCompMainDialog)

public:
	// Static Members
	static int _Id;
	static int GetId() { return _Id++; }

	void SetAccount();
	void SetFund();
	SmFundCompMainDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmFundCompMainDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	const std::map<int, std::shared_ptr<SmCompOrderDialog>>& GetCenterWndMap() {
		return _CenterWndMap;
	}
	void OnQuoteAreaShowHide();
	void RecalcChildren(CmdMode mode);

	void OnDoOrder(const DarkHorse::SmPriceType price_type, const int slip);
private:

	// key : combo index, value : account object.

	std::map<int, std::shared_ptr<DarkHorse::SmFund>> _ComboFundMap;

	int _CurrentFundIndex{ 0 };
	std::shared_ptr<SmFundCompLeftDialog> _LeftWnd = nullptr;
	std::shared_ptr<SmFundCompRightDialog> _RightWnd = nullptr;
	std::map<int, std::shared_ptr<SmCompOrderDialog>> _CenterWndMap;
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
	void SetFundInfo(std::shared_ptr<DarkHorse::SmFund> fund);
	CRect _rcMain;
	CRect _rcLeft;
	CRect _rcRight;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;
	// 0 : for account, 1 : for fund
	int _Mode = 0;
	void SetFundForOrderWnd();
public:
	void OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void OnSymbolClicked(const std::string& symbol_code);
	// 주문창을 추가한다.
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnRemove();
	afx_msg void OnBnClickedBtnLeft();
	afx_msg void OnBnClickedBtnRight();
	afx_msg LRESULT OnEnterSizeMove(WPARAM, LPARAM);
	afx_msg LRESULT OnExitSizeMove(WPARAM, LPARAM);
	CBCGPComboBox _ComboFund;
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
	SmLineGrid _Line11;
	SmLineGrid _Line12;
	SmLineGrid _Line13;
	SmLineGrid _Line14;
	SmLineGrid _Line15;
	SmLineGrid _Line16;
	SmLineGrid _Line17;
	SmLineGrid _Line18;
	SmLineGrid _Line19;
	SmLineGrid _Line20;
	SmLineGrid _Line21;
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
};


