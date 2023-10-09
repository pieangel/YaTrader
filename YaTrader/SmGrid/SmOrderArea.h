#pragma once
#include <memory>
#include <map>
#include <set>
#include <vector>
#include "../Order/SmOrderGridConst.h"
#include "SmGridResource.h"
#include "../Order/SmOrderConst.h"
#include "SmButtonId.h"
#include "../Order/SmOrderSettings.h"


namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmStopOrderManager;
	class SmOrderViewer;
	struct SmOrderRequest;
}

class AbAccountOrderWindow;
class SmFundOrderDialog;
class SmOrderArea : public CBCGPStatic
{
public:
	void SetMainDialog(AbAccountOrderWindow* main_dialog) {
		_MainDialog = main_dialog;
	}
	void SetFundDialog(SmFundOrderDialog* fund_dialog) {
		_FundDialog = fund_dialog;
	}
	void Clear();
	SmOrderArea();
	~SmOrderArea();
	std::shared_ptr<DarkHorse::SmGrid> Grid() const { return _Grid; }
	void Grid(std::shared_ptr<DarkHorse::SmGrid> val) { _Grid = val; }
	void SetQuote(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void SetHoga(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void SetPosition();
	void ClearOldHoga();
	void ClearOldQuote();
	void ClearStopOrders();
	void ClearOrders();
	void SetStopOrder();
	void SetOrder();
	void ClearOldHoga(DarkHorse::Hoga_Type hoga_type) const noexcept;
	int FindRow(const int& value) const noexcept;
	int FindRowFromCenterValue(std::shared_ptr<DarkHorse::SmSymbol> symbol, const int& value);
	void SetQuoteColor(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	int FindValue(const int& row) const noexcept;
	void Refresh();
	void DrawHogaLine(const CRect& rect);
	void DrawFixedSelectedCell();
	void DrawMovingOrder();
	void SetUp();
	//int RecalRowCount();
	int RecalRowCount(const int& height);
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val) { _Account = val; }
	void UpdateOrder(const std::string& symbol_code);
	void ConnectOrderUpdateSignal();
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
	void SetCenterValues(std::shared_ptr<DarkHorse::SmSymbol> symbol, const bool& make_row_map = true);
	bool Selected() const { return _Selected; }
	void Selected(bool val) { _Selected = val; }
	void PutOrderBySpaceBar();
	void ChangeOrderByKey(const int up_down);
	int OrderAmount() const { return _OrderAmount; }
	void OrderAmount(int val) { _OrderAmount = val; }
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	int CutMode() const { return _CutMode; }
	void CutMode(int val) { _CutMode = val; }
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void OnQuoteEvent(const std::string& symbol_code);
	void OnHogaEvent(const std::string& symbol_code);
	void OnSymbolMasterEvent(const std::string& symbol_code);
	void UpdateOrderSettings(DarkHorse::SmOrderSettings settings);
	void SetAllRowHeight(const int& row_height);
	bool FixedMode() const { return _FixedMode; }
	void FixedMode(bool val);
	
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val);
private:
	void DrawStopOrder();
	std::vector<std::pair<CBCGPRect, CBCGPRect>> _StopRectVector;
	void DrawArrow(const CBCGPPoint& start_point, const CBCGPPoint& end_point, const double& stroke_width, const int& head_width);
	void ProcessFixedMode();

	bool _FixedMode = false;
	void SetProfitLossCut(std::shared_ptr<DarkHorse::SmOrderRequest> order_req);
	void SetStopOrderCut(std::shared_ptr<DarkHorse::SmOrderRequest> order_req);
	DarkHorse::SmOrderSettings _OrderSettings;
	bool _EnableOrderShow = false;
	bool _EnableQuoteShow = false;
	bool _EnableHogaShow = false;
	bool _EnableStopShow = false;
	int _CutMode = 0;
	bool _MovingOrder = false;
	void PutStopOrder(const DarkHorse::SmPositionType& type, const int& price);
	void PutOrder(const DarkHorse::SmPositionType& type, const int& price, const DarkHorse::SmPriceType& price_type = DarkHorse::SmPriceType::Price);
	AbAccountOrderWindow* _MainDialog = nullptr;
	SmFundOrderDialog* _FundDialog = nullptr;
	void ProcesButtonClick(const std::shared_ptr<DarkHorse::SmCell>& cell);
	bool ProcesButtonClickByPos(const CPoint& point);
	std::pair<int, int> GetOrderCount(const std::shared_ptr<DarkHorse::SmCell>& cell);
	bool _Editing = false;
	// 원래 헤더 너비는 정해져 있지만 빈틈을 없애기 위하여 헤더 너비를 다시 설정한다.
	void ResetHeaderWidth(const int& wnd_width);
	
	SmOrderGridResource _Resource;
	void CreateResource();


	std::vector<std::shared_ptr<DarkHorse::SmCell>> _SelectedCells;

	std::shared_ptr<DarkHorse::SmCell> _FixedSelectedCell{ nullptr };
	std::pair<int, int> _SelectedValue{ 0, 0 };

	/// <summary>
	/// Key : Quote Value in integer. value : row index
	/// </summary>
	std::map<int, int> _QuoteToRowIndexMap;
	/// <summary>
	/// Key : row index, value : quote value.
	/// </summary>
	std::map<int, int> _RowIndexToPriceMap;

	// Key : cell object, value : button id
	std::map<std::shared_ptr<DarkHorse::SmCell>, BUTTON_ID> _ButtonMap;

	std::shared_ptr<DarkHorse::SmGrid> _Grid{ nullptr };

	CBCGPGraphicsManager* m_pGM{ nullptr };
	DarkHorse::MouseMode _MouseMode{ DarkHorse::MouseMode::Normal };
	int _CloseRow{ 13 };
	int _ValueStartRow{ 1 };
	std::set<int> _OldHogaBuyRowIndex;
	std::set<int> _OldHogaSellRowIndex;
	std::set<int> _OldOrderBuyRowIndex;
	std::set<int> _OldOrderSellRowIndex;
	std::set<int> _OldStopBuyRowIndex;
	std::set<int> _OldStopSellRowIndex;
	std::set<std::pair<int, int>> _TotalHogaMap;
	// key : row, value : column
	std::set<std::pair<int, int>> _QuoteValueMap;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol{ nullptr };

	std::shared_ptr<DarkHorse::SmAccount> _Account{ nullptr };

	std::shared_ptr<DarkHorse::SmStopOrderManager> _SellStopOrderMgr{ nullptr };
	std::shared_ptr<DarkHorse::SmStopOrderManager> _BuyStopOrderMgr{ nullptr };
	std::vector<std::string> _OrderTableHeader;


	int _OrderAmount{ 1 };

	int _X{ 0 };
	int _Y{ 0 };

	int _StartX{ 0 };
	int _StartY{ 0 };
	int _EndX{ 0 };
	int _EndY{ 0 };

	bool _Selected{ false };
	
	CBCGPStrokeStyle _OrderStroke;
	bool _Hover{ false };
	int _OrderStartCol{ -1 };
	int _OrderStartRow{ -1 };
	int _OrderStartPrice{ -1 };
	bool _DraggingOrder{ false };
	bool m_bTrackMouse{ false };
	bool _CenterValued{ false };
	void IncreaseCloseRow(const int& delta);
	void ClearOldSelectedCells();
	void DrawMovingRect();

	void CancelOrder(const std::shared_ptr<DarkHorse::SmCell>& src_cell);
	void ChangeOrder(const std::shared_ptr<DarkHorse::SmCell>& src_cell, const int& tgt_price);

	void ChangeStop(const std::shared_ptr<DarkHorse::SmCell>& src_cell, const std::shared_ptr<DarkHorse::SmCell>& tgt_cell, const int& src_price, const int& tgt_price);
	void CancelStop(const std::shared_ptr<DarkHorse::SmCell>& src_cell);
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

