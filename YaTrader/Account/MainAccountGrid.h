
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
class SubAccountGrid;
class MainAccountGrid : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(MainAccountGrid)

		// Construction
public:
	MainAccountGrid();

	// Attributes
protected:
	BOOL m_bExtendedPadding;

	// Operations
public:
	void ClearAll();
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
public:

	virtual ~MainAccountGrid();

	virtual void OnHeaderCheckBoxClick(int 	nColumn);

	virtual void OnRowCheckBoxClick(CBCGPGridRow* pRow);


	void SetSubAccountEditor(SubAccountEditor* sub_account_editor) {
		_SubAccountEditor = sub_account_editor;
	}
	SubAccountGrid* SubAccountGrid = nullptr;

	void AddAccount(std::shared_ptr<DarkHorse::SmAccount> account);
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
	std::set<int> _OldContentRowSet;
	void ClearOldContents();
	void ClearOldContents(const int& last_index);

	// key : row index, value : SmAccount object
	std::map<int, std::shared_ptr<DarkHorse::SmAccount>> _RowToAccountMap;
	int _MaxRow = 0;
public:
	void UpdateAccountInfo(std::shared_ptr<DarkHorse::SmAccount> account);
	std::shared_ptr<DarkHorse::SmAccount> GetFirstAccount();
	std::shared_ptr<DarkHorse::SmAccount> GetLastAccount();
	void ClearAccountGrid(const int& row);
	void SetAccountList();
	void SetAccountList(const std::vector<std::shared_ptr<DarkHorse::SmAccount>>& account_vector);
	void StartTimer();
	void Update();
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};