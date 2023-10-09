
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "../SmGrid/SmGridResource.h"
#include "../Order/OrderUi/DmDefine.h"
#include "../ViewModel/VmFuture.h"
#include "../ViewModel/VmQuote.h"
#include "../ViewModel/VmFuture.h"
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
}
using order_p = std::shared_ptr<DarkHorse::Order>;
class DmFutureView : public CBCGPStatic
{
public:
	DmFutureView();
	~DmFutureView();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	void update_expected(std::shared_ptr<DarkHorse::SmQuote> quote);
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return _Fund; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	void init_dm_future();
	void update_quote();
	void set_view_mode(ViewMode view_mode);
	void on_update_position_vm_future(const VmPosition& position);
public:
	void update_order(order_p order, DarkHorse::OrderEvent order_event);
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	void UpdateAssetInfo();
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	int order_window_id() const { return order_window_id_; }
	void order_window_id(int val) { order_window_id_ = val; }
private:
	std::string get_position_text(const DarkHorse::VmFuture& future_info);
	void get_future_info(DarkHorse::VmFuture& future_info);
	void get_init_accepted_order_count(DarkHorse::VmFuture& future_info);
	DarkHorse::OrderType order_type_{ DarkHorse::OrderType::None };
	int order_window_id_ = 0;
	void set_position(DarkHorse::VmFuture& future_info);
	void update_position();
	void on_update_position();
	void on_update_quote();
	void update_close(const DarkHorse::VmQuote& quote);
	void show_values();
	void show_value(const int row, const int col, const DarkHorse::VmFuture& future_info);
	void set_background_color(std::shared_ptr<DarkHorse::SmCell> cell, const DarkHorse::VmFuture& future_info);
	ViewMode view_mode_{ ViewMode::VM_Close };
	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	std::shared_ptr<DarkHorse::SymbolPositionControl> position_control_;
	void register_symbol_to_server(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void UpdateAccountAssetInfo();
	void UpdateFundAssetInfo();
	void register_symbols();
	bool registered_{ false };
	// 0 : account, 1 : fund
	int _Mode = 0;
	bool enable_show_{ true };
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;
	
	std::vector<DarkHorse::VmFuture> symbol_vec_;
	// key : symbol code, value : row index.
	std::map<std::string, int> symbol_row_index_map_;
	int id_{ 0 };
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};






