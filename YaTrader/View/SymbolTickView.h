/*
#pragma once
class SymbolTickView
{
};
*/
#pragma once
#include <memory>
#include <vector>
#include <string>

#include "../SmGrid/SmGridResource.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SymbolTickControl;
}

class SymbolTickView : public CBCGPStatic
{
public:
	SymbolTickView();
	~SymbolTickView();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	int get_id() {
		return id_;
	}
	void Clear();
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return symbol_; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val);
	void OnQuoteEvent(const std::string& symbol_code);
	void set_parent(CWnd* parent) {
		parent_ = parent;
	}
	void set_center_window_id(const int center_window_id) {
		center_window_id_ = center_window_id;
	}
private:
	int center_window_id_{0};
	int id_{0};
	CWnd* parent_{ nullptr };
	void draw_tick(const int row, const int col, const std::string& value, const int up_down);
	void on_update_tick();
	void update_tick();
	bool _EnableQuoteShow = false;
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;
	std::shared_ptr<DarkHorse::SymbolTickControl> tick_control_;

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> symbol_ = nullptr;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};



