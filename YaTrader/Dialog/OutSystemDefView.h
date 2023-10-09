#pragma once


#include <BCGCBProInc.h>
#include <map>
#include <memory>	

namespace DarkHorse
{
	class SmSymbol;
}
const int grid_row_count2 = 100;
class OutSystemDefView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(OutSystemDefView)
public:
	OutSystemDefView();
	virtual ~OutSystemDefView();
	void init_grid();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
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
};