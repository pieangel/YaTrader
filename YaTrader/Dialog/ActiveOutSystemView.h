#pragma once

#include <BCGCBProInc.h>
#include <map>
#include <memory>	
#include <vector>

namespace DarkHorse
{
	class SmSymbol;
	class SmOutSystem;
	struct Position;
	struct SmQuote;
}
const int active_out_system_row = 300;
using position_p = std::shared_ptr<DarkHorse::Position>;
using quote_p = std::shared_ptr<DarkHorse::SmQuote>;
using symbol_p = std::shared_ptr<DarkHorse::SmSymbol>;
using out_system_p = std::shared_ptr<DarkHorse::SmOutSystem>;

class ActiveOutSystemView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(ActiveOutSystemView)
public:
	ActiveOutSystemView();
	virtual ~ActiveOutSystemView();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void add_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	void remove_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	void on_update_quote();
	void on_update_position();
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool need_update = false;
	bool need_update_position_ = false;
	bool nedd_update_quote_ = false;
	void update_position(const position_p& position);
	void update_quute(const quote_p& quote);
	void init_grid();
	void clear_old_contents(const int& last_index);
	// key : out system id, value : row, 
	std::map<int, int> row_to_out_system_;
	// key : out system id, value : out system
	std::map<int, std::shared_ptr<DarkHorse::SmOutSystem>> out_system_map_;
	std::vector<std::shared_ptr<DarkHorse::SmOutSystem>> out_systems_;
	void remap_row_to_out_system();
	COLORREF _DefaultBackColor;
	void ClearGrid();
	bool init_ = false;
	int max_index_ = 0;
};
