
#pragma once
#include <BCGCBProInc.h>
#include <memory>
#include <vector>
#include <string>
#include <set>
#include <map>

namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmOrder;
	class SmFund;
}
class SubAccountEditor;
class MainAccountGrid;
class SubAccountGrid : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(SubAccountGrid)

		// Construction
public:
	SubAccountGrid();


	// Attributes
protected:
	BOOL m_bExtendedPadding;

	// Operations
public:
	void ToggleExtendedPadding();
	BOOL IsExtendedPadding() const
	{
		return m_bExtendedPadding;
	}

	// Overrides
	virtual int GetTextMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetTextMargin();
	}

	virtual int GetTextVMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetTextVMargin();
	}

	virtual int GetImageMargin() const
	{
		return m_bExtendedPadding ? 8 : CBCGPGridCtrl::GetImageMargin();
	}

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicGridCtrl)
	//}}AFX_VIRTUAL

// Implementation
	void update_sub_account(std::shared_ptr<DarkHorse::SmAccount> sub_account);
public:
	MainAccountGrid* AccountGrid = nullptr;

	virtual ~SubAccountGrid();

	virtual void OnHeaderCheckBoxClick(int 	nColumn);

	virtual void OnRowCheckBoxClick(CBCGPGridRow* pRow);


	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val) { _Account = val; }

	void SetSubAccountEditor(SubAccountEditor* sub_account_editor) {
		_SubAccountEditor = sub_account_editor;
	}

	// Generated message map functions
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	SubAccountEditor* _SubAccountEditor = nullptr;
	bool _EnableOrderShow = false;
	bool _HeaderCheck = false;
	int _OldMaxRow = -1;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::set<int> _OldContentRowSet;
	void ClearOldContents(const int& last_index);
	// key : row, value : sub account
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _RowToSubAccountMap;
	int _MaxAccountRow{ 0 };
	int _SelectedIndex = -1;
public:
	void ClearOldContents();

	std::shared_ptr<DarkHorse::SmAccount> get_selected_account();
	void MoveSelectedAccounts();
	void MoveUp();
	void MoveDown();
	void MoveFirstAccount();
	void MoveAllAccounts();
	void ClearAll();
	void ClearSubAccountGrid(const int& row);
	void InitAccount(std::shared_ptr<DarkHorse::SmAccount> account);
	void InitAccount();
	void set_default_account();

	void StartTimer();
	void Update();
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

