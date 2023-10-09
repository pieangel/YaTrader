
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "../SmGrid/SmGridResource.h"
#include "../ViewModel/VmOption.h"
#include "../Order/OrderUi/DmDefine.h"
#include "../ViewModel/VmQuote.h"
#include "../Order/SmOrderConst.h"
#include "../ViewModel/VmPosition.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmFund;
	class QuoteControl;
	struct SmQuote;
	class SymbolPositionControl;
	struct Order;
	struct Position;
}

using order_p = std::shared_ptr<DarkHorse::Order>;

class DmOptionView : public CBCGPStatic
{
public:
	DmOptionView();
	~DmOptionView();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	void set_option_view(
		const int option_market_index,
		const std::string& year_month_name
	);
public:
	void update_order(order_p order, DarkHorse::OrderEvent order_event);
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return _Fund; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	void UpdateAssetInfo();
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void set_view_mode(ViewMode view_mode);
	int order_window_id() const { return order_window_id_; }
	void order_window_id(int val) { order_window_id_ = val; }
private:
	std::string get_position_text(const DarkHorse::VmOption& option_info);
	void get_option_info(DarkHorse::VmOption& option_info);
	void get_init_accepted_order_count(DarkHorse::VmOption& option_info);
	void set_call_put_area();
	int order_window_id_ = 0;
	void update_expected(std::shared_ptr<DarkHorse::SmQuote> quote);
	void update_quote();
	void update_close(const DarkHorse::VmQuote& quote);
	void update_position();
	void on_update_position_vm(const VmPosition& position);
	void update_value_cell(const int symbol_id, const DarkHorse::VmOption& option_info);
	void on_update_quote();
	void register_symbols(const int option_market_index);
	void register_symbol(const std::string symbol_code);
	void show_value(const int row, const int col, const DarkHorse::VmOption& option_info);
	void set_background_color(std::shared_ptr<DarkHorse::SmCell> cell, const DarkHorse::VmOption& option_info);
	void show_strike(const int row, const int col, const DarkHorse::VmOption& option_info);
	void show_values();
	ViewMode view_mode_{ ViewMode::VM_Close };
	void set_option_info(
		const int option_market_index,
		const std::string& year_month_name
	);
	void make_symbol_vec(bool call_side = true);
	void init_strike_index();

	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	std::shared_ptr<DarkHorse::SymbolPositionControl> position_control_;
	void register_symbols_to_server();
	void set_option_view();
	void set_strike_start_index(const int distance);
	void set_strike();
	void UpdateAccountAssetInfo();
	void UpdateFundAssetInfo();
	DarkHorse::OrderType order_type_{ DarkHorse::OrderType::None };
	int atm_index_{ 0 };
	// 0 : account, 1 : fund
	int _Mode = 0;
	int option_market_index_ = 0;
	std::string year_month_name_;
	int strike_start_index_ = 1;
	bool enable_show_{ true };
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;

	std::vector<DarkHorse::VmOption> call_symbol_vector_;
	std::vector<DarkHorse::VmOption> put_symbol_vector_;

	// key : (row, col), value : VmOption object
	std::map<std::pair<int, int>, std::shared_ptr<DarkHorse::SmSymbol>> symbol_map_;
	// key : symbol id, value : (row, col)
	std::map<int, std::pair<int, int>> row_col_map_;
	// key : symbol code, value : symbol vector index.
	std::map<std::string, int> symbol_vector_index_map_;

	std::map<int, int> registered_map_;

	int id_{ 0 };

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};





