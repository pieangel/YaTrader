/*
#pragma once
class SymbolOrderView
{
};
*/

#pragma once
#include <memory>
#include <map>
#include <set>
#include <vector>
#include "../Order/SmOrderGridConst.h"
#include "../SmGrid/SmGridResource.h"
#include "../Order/SmOrderConst.h"
#include "../SmGrid/SmButtonId.h"
#include "../Order/SmOrderSettings.h"
#include "../SmGrid/SmCellType.h"
#include "../Order/OrderRequest/OrderRequest.h"
#include "../Event/EventHubArg.h"
#include "../Symbol/SymbolConst.h"
#include "../Position/PositionConst.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmStopOrderManager;
	class SmOrderViewer;
	struct SmOrderRequest;
	struct OrderRequest;
	class HogaControl;
	class QuoteControl;
	class ProductControl;
	class OrderControl;
	class SymbolPositionControl;
	struct SmHoga;
	struct SmQuote;
	class SubOrderControl;
	class PriceOrderMap;
	class StopOrderControl;
	class PriceOrderRequestMap;
	class SmFund;
    struct Order;
}

using order_request_p = std::shared_ptr<DarkHorse::OrderRequest>;
using price_order_request_map_p = std::shared_ptr<DarkHorse::PriceOrderRequestMap>;

class DmAccountOrderWindow;
class DmFundOrderWindow;
class SymbolOrderView : public CBCGPStatic
{
public:
	void set_stop_as_real_order(bool enable);
	int get_id() {
		return id_;
	}
	void SetMainDialog(DmAccountOrderWindow* main_dialog) {
		_MainDialog = main_dialog;
	}
	void SetFundDialog(DmFundOrderWindow* fund_dialog) {
		_FundDialog = fund_dialog;
	}
	void Clear();
	SymbolOrderView();
	~SymbolOrderView();
	std::shared_ptr<DarkHorse::SmGrid> Grid() const { return _Grid; }
	void Grid(std::shared_ptr<DarkHorse::SmGrid> val) { _Grid = val; }
	
	void update_hoga();
	void update_quote();

	//void SetPosition();
	void ClearOldHoga();
	void ClearOldQuote();
	void ClearStopOrders();
	void ClearOrders();
	//void SetStopOrder();
	//void SetOrder();
	void ClearOldHoga(DarkHorse::Hoga_Type hoga_type) const noexcept;
	int FindRow(const int& value) const noexcept;
	int FindRowFromCenterValue(const int& value);
	void set_quote_color();
	int FindValue(const int& row) const noexcept;
	void Refresh();
	void DrawHogaLine(const CRect& rect);
	void DrawFixedSelectedCell();
	void DrawMovingOrder();
	void SetUp();
	//int RecalRowCount();
	int RecalRowCount(const int& height);
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return account_; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmFund> fund() const { return fund_; }
	void fund(std::shared_ptr<DarkHorse::SmFund> val);
	void ArrangeCenterValue();
	void BuyByMarketPrice();
	void SellByMarketPrice();
	void CancelSellStop();
	void CancelSellOrder();
	void CancelAllOrder();
	void CancelBuyStop();
	void CancelBuyOrder();
	// 중앙 값을 정한다. 중앙 값은 최초에 심볼이 결정될 때 한번, 
	// 그리고 전체 크기가 늘어나거나 줄어들때(대화상자를 늘이거나 줄일때) 다시 설정해 준다.
	void SetCenterValues(const bool& make_row_map = true);
	void set_center_values(const bool make_row_map = true);
	int get_start_value();
	bool Selected() const { return _Selected; }
	void Selected(bool val) { _Selected = val; }
	void PutOrderBySpaceBar();
	//void PutOrderBySpaceBar(std::shared_ptr<DarkHorse::SmAccount> account);
	void ChangeOrderByKey(const int up_down);
	int OrderAmount() const { return _OrderAmount; }
	void OrderAmount(int val) { _OrderAmount = val; }
	//void OnOrderChanged(const int& account_id, const int& symbol_id);
	int CutMode() const { return _CutMode; }
	void CutMode(int val) { _CutMode = val; }
	//void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	//void OnQuoteEvent(const std::string& symbol_code);
	//void OnHogaEvent(const std::string& symbol_code);
	//void OnSymbolMasterEvent(const std::string& symbol_code);
	void UpdateOrderSettings(DarkHorse::SmOrderSettings settings);
	void SetAllRowHeight(const int& row_height);
	void reset_col_widths(const DarkHorse::OrderSetEvent& order_set_event);
	int get_entire_width();
	// 원래 헤더 너비는 정해져 있지만 빈틈을 없애기 위하여 헤더 너비를 다시 설정한다.
	void ResetHeaderWidth(const int& wnd_width);
	bool FixedMode() const { return _FixedMode; }
	void FixedMode(bool val);

	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val);
	//void insert_decimal(std::string& value, const int decimal);
	void on_update_quote();
	void on_update_hoga();
	void on_update_symbol_master(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void on_update_order();
	void on_update_position();
	void set_order_request_type(DarkHorse::OrderRequestType order_req_type);
	void set_fill_condition(DarkHorse::SmFilledCondition fill_condition) {
		fill_condition_ = fill_condition;
	}
	void set_price_type(DarkHorse::SmPriceType price_type) {
		price_type_ = price_type;
	}
	void set_parent(CWnd* wnd) {
		parent_ = wnd;
	}
	void on_paramter_event(const DarkHorse::OrderSetEvent& event, const std::string& event_message, const bool enable);
	/*
	void put_order
	(
		const std::string& symbol_code,
		const DarkHorse::SmPositionType& type,
		const int price,
		const int amount,
		const DarkHorse::SmPriceType price_type
	);
	*/

	void put_order
	(
		std::shared_ptr<DarkHorse::SmAccount> account,
		const std::string& symbol_code,
		const DarkHorse::SmPositionType& type,
		const int price,
		const int amount,
		const DarkHorse::SmPriceType price_type
	);

	void put_order
	(
		const DarkHorse::SmPositionType& type,
		const int& price,
		const DarkHorse::SmPriceType& price_type = DarkHorse::SmPriceType::Price
	);
	void set_center_window_id(const int center_window_id) {
		center_window_id_ = center_window_id;
	}
	void set_order_window_id(const int order_window_id) {
		order_window_id_ = order_window_id;
	}
	DarkHorse::SymbolType symbol_type() const { return symbol_type_; }
	void symbol_type(DarkHorse::SymbolType val) { symbol_type_ = val; }
	DarkHorse::PositionType position_type() const { return position_type_; }
	void position_type(DarkHorse::PositionType val) { position_type_ = val; }
	DarkHorse::OrderType order_type() const { return order_type_; }
	void order_type(DarkHorse::OrderType val) { order_type_ = val; }
	void reset_window_size();
	const std::vector<DarkHorse::OrderGridHeaderInfo>& grid_header_vector() const { return grid_header_vector_; }
	void trigger_resize_event();
	DarkHorse::OrderSetEvent order_set() const { return order_set_; }
	void order_set(DarkHorse::OrderSetEvent val) { order_set_ = val; }
private:
	DarkHorse::OrderSetEvent order_set_;
	void set_order_area();
	DarkHorse::OrderType order_type_{ DarkHorse::OrderType::None };

	DarkHorse::SymbolType symbol_type_{ DarkHorse::SymbolType::None };
	DarkHorse::PositionType position_type_{ DarkHorse::PositionType::None };
	void CancelSellOrder(std::shared_ptr<DarkHorse::SmAccount> account);
	void CancelBuyOrder(std::shared_ptr<DarkHorse::SmAccount> account);
	int order_window_id_{0};
	int center_window_id_{ 0 };
	void set_fixed_selected_cell();
	void clear_fixed_selected_cell();
	void on_order_set_event(const DarkHorse::OrderSetEvent& event, const bool flag);
	CWnd* parent_{ nullptr };
	void update_position();
	void update_buy_stop_order();
	void update_sell_stop_order();
	void on_update_buy_stop_order();
	void on_update_sell_stop_order();

	void draw_stop_order_cell(const DarkHorse::SmPositionType position, price_order_request_map_p price_order_req_map);
	void draw_stop_order_line
	(
		std::shared_ptr<DarkHorse::SmCell> cell,
		const DarkHorse::SmPositionType position,
		const std::map<int, order_request_p>& order_req_map
	);

	void draw_stop_order_by_price
	(
		const std::map<int, price_order_request_map_p>& order_req_map,
		const DarkHorse::SmPositionType position
	);
	void draw_total_stop_order
	(
		const int count,
		const DarkHorse::SmPositionType position
	);

	void draw_buy_stop_order();
	void draw_sell_stop_order();

	void clear_buy_stop_order();
	void clear_sell_stop_order();

	bool enable_buy_stop_order_show_{ false };
	bool enable_sell_stop_order_show_{ false };

	std::set<int> old_stop_buy_order_index_;
	std::set<int> old_stop_sell_order_index_;

	std::shared_ptr<DarkHorse::StopOrderControl> buy_stop_order_control_{ nullptr };
	std::shared_ptr<DarkHorse::StopOrderControl> sell_stop_order_control_{ nullptr };

	std::vector<std::pair<CBCGPRect, CBCGPRect>> buy_stop_order_rect_vector_;
	std::vector<std::pair<CBCGPRect, CBCGPRect>> sell_stop_order_rect_vector_;

	void set_position();
	void set_order();
	void set_filled_condition(DarkHorse::OrderRequestType order_req_type);
	std::shared_ptr<DarkHorse::SmCell> draw_cell(const int row, const int col, const int value);
	void draw_order_cell(DarkHorse::SmPositionType position, const int price, const int count);
	void draw_order();
	void draw_order_by_price
	(
		DarkHorse::SubOrderControl& sub_order_control,
		DarkHorse::SmPositionType position
	);
	void draw_total_order
	(
		const DarkHorse::SubOrderControl& sub_order_control,
		DarkHorse::SmPositionType position
	);
	int find_close_row_from_end_row();
	int find_zero_value_row();
	int find_row(const int target_value);
	int find_start_value();
	int find_row2(const int target_value);
	int find_value(const int target_row);
	int get_center_row();
	void set_quote_value(const int value, const DarkHorse::SmCellType cell_type);
	void set_quote_cell(const int row, const bool show_mark, const DarkHorse::SmMarkType mark_type);
	
	void init_quote_control(const std::string& symbol_code);
	void init_hoga_control(const std::string& symbol_code);
	std::shared_ptr<DarkHorse::OrderControl> order_control_;
	std::shared_ptr<DarkHorse::HogaControl> hoga_control_;
	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	std::shared_ptr<DarkHorse::ProductControl> product_control_;
	std::shared_ptr<DarkHorse::SymbolPositionControl> position_control_;
	void DrawStopOrder();
	std::vector<std::pair<CBCGPRect, CBCGPRect>> _StopRectVector;
	void DrawArrow
	(
		const CBCGPPoint& start_point, 
		const CBCGPPoint& end_point, 
		const double& stroke_width, 
		const int& head_width
	);
	void ProcessFixedMode();

	bool _FixedMode = false;
	void SetProfitLossCut(std::shared_ptr<DarkHorse::SmOrderRequest> order_req);
	void SetProfitLossCut(std::shared_ptr<DarkHorse::OrderRequest> order_req);
	void SetStopOrderCut(std::shared_ptr<DarkHorse::SmOrderRequest> order_req);
	void set_order_close(std::shared_ptr<DarkHorse::OrderRequest> order_req);
	void set_virtual_filled_value(std::shared_ptr<DarkHorse::OrderRequest> order_req);
	DarkHorse::SmOrderSettings _OrderSettings;
	bool _EnableOrderShow = false;
	bool _EnableQuoteShow = false;
	bool _EnableHogaShow = false;
	bool _EnableStopShow = false;
	bool enable_position_show_ = false;
	int _CutMode = 0;
	bool _MovingOrder = false;
	void put_stop_order(const DarkHorse::SmPositionType& type, const int& price);
	
	DmAccountOrderWindow* _MainDialog = nullptr;
	DmFundOrderWindow* _FundDialog = nullptr;
	void ProcesButtonClick(const std::shared_ptr<DarkHorse::SmCell>& cell);
	bool ProcesButtonClickByPos(const CPoint& point);
	std::pair<int, int> GetOrderCount(const std::shared_ptr<DarkHorse::SmCell>& cell);
	std::pair<int, int> get_order_count(const std::shared_ptr<DarkHorse::SmCell>& cell);
	bool _Editing = false;
	SmOrderGridResource _Resource;
	void CreateResource();

	DarkHorse::OrderRequestType order_request_type_{ DarkHorse::OrderRequestType::None };

	std::vector<std::shared_ptr<DarkHorse::SmCell>> _SelectedCells;

	std::shared_ptr<DarkHorse::SmCell> selected_cell_{ nullptr };
	std::pair<int, int> _SelectedValue{ 0, 0 };

	/// <summary>
	/// Key : Quote Value in integer. value : row index
	/// </summary>
	std::map<int, int> price_to_row_;
	/// <summary>
	/// Key : row index, value : quote value.
	/// </summary>
	std::map<int, int> row_to_price_;

	// Key : cell object, value : button id
	std::map<std::shared_ptr<DarkHorse::SmCell>, BUTTON_ID> _ButtonMap;

	std::shared_ptr<DarkHorse::SmGrid> _Grid{ nullptr };

	CBCGPGraphicsManager* m_pGM{ nullptr };
	DarkHorse::MouseMode _MouseMode{ DarkHorse::MouseMode::Normal };
	// 가격 표시 기준점. 이 기준점을 기준으로 현재가를 기준으로 가격을 만든다.
	// 현재가가 변해도 이 값은 변화가 없고 마우스 휠을 사용하여 이 값이 변할 때
	// 다시 현재가를 기준으로 다시 만든다. 마우스 휠을 움직일 때 이 행은 항상 현재가를 가르키지만
	// 마우스 휠이 멈췄을 때는 더이상 현재가가 있는 행을 가르키지 않는다. 
	int index_row_{ 13 };
	std::set<int> _OldHogaBuyRowIndex;
	std::set<int> _OldHogaSellRowIndex;
	std::set<int> _OldOrderBuyRowIndex;
	std::set<int> _OldOrderSellRowIndex;
	std::set<int> _OldStopBuyRowIndex;
	std::set<int> _OldStopSellRowIndex;
	std::set<std::pair<int, int>> _TotalHogaMap;
	// key : (row, column) value : price
	std::set<std::pair<int, int>> quote_priece_set;
	std::shared_ptr<DarkHorse::SmSymbol> symbol_{ nullptr };

	std::shared_ptr<DarkHorse::SmAccount> account_{ nullptr };
	std::shared_ptr<DarkHorse::SmFund> fund_{ nullptr };

	std::shared_ptr<DarkHorse::SmStopOrderManager> _SellStopOrderMgr{ nullptr };
	std::shared_ptr<DarkHorse::SmStopOrderManager> _BuyStopOrderMgr{ nullptr };
	std::vector<std::string> _OrderTableHeader;

	std::vector<DarkHorse::OrderGridHeaderInfo> grid_header_vector_;

	int id_{ 0 };
	int _OrderAmount{ 1 };
	DarkHorse::SmFilledCondition fill_condition_{DarkHorse::SmFilledCondition::Fas};
	DarkHorse::SmPriceType price_type_{ DarkHorse::SmPriceType::Price };
	int _X{ 0 };
	int _Y{ 0 };

	int _StartX{ 0 };
	int _StartY{ 0 };
	int _EndX{ 0 };
	int _EndY{ 0 };

	bool _Selected{ false };
	bool center_valued_{ false };

	int price_start_row_{ 2 };
	int price_end_row_{ 0 };
	CBCGPStrokeStyle _OrderStroke;
	bool _Hover{ false };
	int _OrderStartCol{ -1 };
	int _OrderStartRow{ -1 };
	int _OrderStartPrice{ -1 };
	bool _DraggingOrder{ false };
	bool m_bTrackMouse{ false };
	bool _CenterValued{ false };
	void increase_close_row(const int& delta);
	void set_close_row();
	void ClearOldSelectedCells();
	void DrawMovingRect();
	void set_moving_rect();
	void reset_moving_rect();
	void add_moving_rect(const std::shared_ptr<DarkHorse::SmCell>& src_cell);
	void CancelOrder(const std::shared_ptr<DarkHorse::SmCell>& src_cell);
	void cancel_order(const std::shared_ptr<DarkHorse::SmCell>& src_cell);
	void cancel_order(const std::vector<std::shared_ptr<DarkHorse::Order>> order_vec);
	void ChangeOrder(const std::shared_ptr<DarkHorse::SmCell>& src_cell, const int& tgt_price);
	void change_order(const std::shared_ptr<DarkHorse::SmCell>& src_cell, const int& target_price);
	void change_order(const std::vector<std::shared_ptr<DarkHorse::Order>>& order_vec, const int& target_price);
	void change_stop
	(
		const std::shared_ptr<DarkHorse::SmCell>& src_cell, 
		const std::shared_ptr<DarkHorse::SmCell>& tgt_cell, 
		const int& src_price, 
		const int& tgt_price
	);
	void change_stop
	(
		price_order_request_map_p order_req_map, 
		const int target_price
	);
	void cancel_stop(const std::shared_ptr<DarkHorse::SmCell>& src_cell);
	void ProcessButtonMsg(const BUTTON_ID& id);
protected:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnWmSymbolMasterReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWmQuoteReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWmHogaReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmOrderChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};



