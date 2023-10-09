#pragma once
#include <BCGCBProInc.h>
#include <map>
#include <memory>	

namespace DarkHorse
{
	class SmSymbol;
	class SmOutSystem;
	class SmAccount;
	class SmFund;
}

class HdSymbolSelecter;
class SmAccountFundSelector;
class SmAutoSignalManagerDialog;
class OutSystemView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(OutSystemView)
public:
	OutSystemView();
	virtual ~OutSystemView();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnItemChanged(CBCGPGridItem* pItem, int nRow, int nColumn);
	void add_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	void remove_out_system();
	std::shared_ptr<DarkHorse::SmOutSystem> get_out_system(const int row);
	std::shared_ptr<DarkHorse::SmOutSystem> get_selected_out_system();
	SmAutoSignalManagerDialog* parent_dlg() const { return parent_dlg_; }
	void parent_dlg(SmAutoSignalManagerDialog* val) { parent_dlg_ = val; }
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void init_grid();
	void clear_old_contents(const int& last_index);
	void remap_row_to_out_system();
	// key : row, value : out system
	std::map<int, std::shared_ptr<DarkHorse::SmOutSystem>> row_to_out_system_;
	COLORREF _DefaultBackColor;
	void ClearGrid();
	int out_system_row_count = 300;
	int selected_row_ = -1;
	void create_out_system_cells(CBCGPGridRow* pRow, std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	CBCGPGridRow* create_or_get_row(const int row_index);
	SmAutoSignalManagerDialog* parent_dlg_{ nullptr };
	int max_index_{ 0 };
};

class  CSymbolItem : public CBCGPGridItem
{
	// Construction
public:
	CSymbolItem(const CString& strValue, OutSystemView& pOutSystemVeiw);

	// Overrides
	virtual void OnClickButton(CPoint point);
private:
	OutSystemView& pOutSystemVeiw_;

	int id_{ 0 };
	void set_symbol_from_out(const int window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol);
};


class  CAccountItem : public CBCGPGridItem
{
	// Construction
public:
	CAccountItem(const CString& strValue, OutSystemView& pOutSystemVeiw);

	// Overrides
	virtual void OnClickButton(CPoint point);

private:
	OutSystemView& pOutSystemVeiw_;

	void set_account_from_out(const int window_id, std::shared_ptr<DarkHorse::SmAccount> account);
	void set_fund_from_out(const int window_id, std::shared_ptr<DarkHorse::SmFund> fund);
	int id_{ 0 };
};

class  COutDefItem : public CBCGPGridItem
{
	// Construction
public:
	COutDefItem(const CString& strValue, OutSystemView& pOutSystemVeiw);

	void SetItemChanged() override;
private:
	OutSystemView& pOutSystemVeiw_;
	int id_{ 0 };
};

