#pragma once
#include <BCGCBProInc.h>
#include <map>
#include <memory>	
namespace DarkHorse
{
	class SmSymbol;
}
const int grid_row_count = 100;
class SymbolFutureView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(SymbolFutureView)
public:
	void set_source_window_id(const int window_id) { source_window_id_ = window_id; }
	SymbolFutureView();
	virtual ~SymbolFutureView();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void init_symbol(const int market_index);
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> row_to_symbol_;
	COLORREF _DefaultBackColor;
	void ClearGrid();
	bool init_ = false;
	int source_window_id_ = 0;
};

