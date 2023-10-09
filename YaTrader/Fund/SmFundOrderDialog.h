#pragma once
#include <BCGCBProInc.h>



#pragma once
#include <memory>
#include <map>
#include <unordered_map>
#include "../Order/SmLineGrid.h"
#include <vector>
#include "../Order/OrderWndConst.h"
#include "../Util/SmButton.h"
namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	struct SmPosition;
	class SmFund;

}


// SmMainOrderDialog dialog
class SmFundOrderCenterWnd;
class SmFundOrderLeft;
class SmFundOrderRight;
class SmFundOrderDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmFundOrderDialog)

public:
	// Static Members
	static int _Id;
	static int GetId() { return _Id++; }

	void SetAccount();
	void SetFund();
	SmFundOrderDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmFundOrderDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_FUND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void OnSymbolClicked(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void OnSymbolClicked(const std::string& symbol_code);
	virtual BOOL OnInitDialog();
	const std::map<int, std::shared_ptr<SmFundOrderCenterWnd>>& GetCenterWndMap() {
		return _CenterWndMap;
	}
	void OnQuoteAreaShowHide();
	void RecalcChildren(CmdMode mode);
	void RecalcChildren2(CmdMode mode);
private:
	std::shared_ptr<SmFundOrderLeft> _LeftWnd = nullptr;
	std::shared_ptr<SmFundOrderRight> _RightWnd = nullptr;
	std::map<int, std::shared_ptr<SmFundOrderCenterWnd>> _CenterWndMap;
	bool _ShowLeft = true;
	bool _ShowRight = true;
	// key : combo index, value : account object.
	//std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _ComboAccountMap;
	std::map<int, std::shared_ptr<DarkHorse::SmFund>> _ComboFundMap;
	int _LineGap = 4;
	bool _Init = false;
	void SetFundForOrderWnd();
	CRect moveRect;
	int _CurrentFundIndex{ 0 };
	void SetAccountInfo(std::shared_ptr<DarkHorse::SmAccount> account);
	void SetFundInfo(std::shared_ptr<DarkHorse::SmFund> fund);
	CRect _rcMain;
	CRect _rcLeft;
	CRect _rcRight;
	//std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;
	void ResetFund();
public:
	afx_msg LRESULT OnUmFundChanged(WPARAM wParam, LPARAM lParam);
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
	std::vector<SmLineGrid*> _LineVector;
	afx_msg void OnStnClickedStaticAccountName();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCbnSelchangeComboFund();
	afx_msg LRESULT OnUmOrderUpdate(WPARAM wParam, LPARAM lParam);
	void ChangedSymbol(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void ChangedCenterWindow(const int& center_wnd_id);
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnLiqAll();
	afx_msg LRESULT OnUmServerMsg(WPARAM wParam, LPARAM lParam);
	SmButton _StaticMsg;
	afx_msg void OnBnClickedBtnFundSet();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual void PostNcDestroy();
};
