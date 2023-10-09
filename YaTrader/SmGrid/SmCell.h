#pragma once
#include <string>
#include <memory>
#include <map>
#include <BCGCBProInc.h>
#include "SmCellType.h"
#include "SmGridResource.h"
class CBCGPGraphicsManager;
namespace DarkHorse {
	class SmOrder;
	struct SmOrderRequest;
	// 그리드 컨트롤의 그리드가 아닌 자체 경계선을 가진다.
	class SmCell
	{
	protected:
		int _LeftMargin = 0;
		int _RightMargin = 0;
		int _Row = -1;
		int _Col = -1;
		int _RowSpan = 1;
		int _ColSpan = 1;
		int _BorderColor = 0x0000000;
		int _BorderLineWidth = 1;
		int _BackGroundColor = 0xffffff;
		int _X = 0;
		int _Y = 0;
		int _Width = 0;
		int _Height = 0;
		bool _Visible = true;
		bool _ShowBorder = false;
		std::string _Text = "";
		// 0 : left, 1 : center, 2 : right.
		int _TextHAlign = 1;
		// 0 : top, 1 : center, 2 : bottom
		int _TextVAlign = 1;
		// 0 : Normal, 1 : Button, 2 : Header
		SmCellType _CellType = SmCellType::CT_NORMAL;
		// key : order request id, value : order request object
		std::map<int, std::shared_ptr<SmOrderRequest>> _OrderReqMap;
		// key : order unique id, value : order object
		std::map<int, std::shared_ptr<SmOrder>> _OrderMap;
		bool _ShowMark = false;
		// 0 : Normal, 1 : buy, 2 : sell, 3 : high / low.
		int _MarkType = 0;
	public:
		void draw(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res);
		void clear() {
			_Text.clear();
		}
		size_t GetOrderReqCount();
		size_t GetOrderCount();
		const std::map<int, std::shared_ptr<SmOrderRequest>>& GetOrderReqMap() {
			return _OrderReqMap;
		}
		const std::map<int, std::shared_ptr<SmOrder>>& GetOrderMap() {
			return _OrderMap;
		}
		void AddOrderReq(const std::shared_ptr<SmOrderRequest>& order_req);
		void AddOrder(const std::shared_ptr<SmOrder>& order);
		void ClearOrderReq();
		void ClearOrder();

		CBCGPRect GetCellRect();
		int Height() const { return _Height; }
		void Height(int val) { _Height = val; }
		int Width() const { return _Width; }
		void Width(int val) { _Width = val; }
		int Y() const { return _Y; }
		void Y(int val) { _Y = val; }
		int X() const { return _X; }
		void X(int val) { _X = val; }
		int BackGroundColor() const { return _BackGroundColor; }
		void BackGroundColor(int val) { _BackGroundColor = val; }
		int BorderLineWidth() const { return _BorderLineWidth; }
		void BorderLineWidth(int val) { _BorderLineWidth = val; }
		int BorderColor() const { return _BorderColor; }
		void BorderColor(int val) { _BorderColor = val; }
		int ColSpan() const { return _ColSpan; }
		void ColSpan(int val) { _ColSpan = val; }
		int RowSpan() const { return _RowSpan; }
		void RowSpan(int val) { _RowSpan = val; }
		int Col() const { return _Col; }
		void Col(int val) { _Col = val; }
		int Row() const { return _Row; }
		void Row(int val) { _Row = val; }
		bool Visible() const { return _Visible; }
		void Visible(bool val) { _Visible = val; }
		bool ShowBorder() const { return _ShowBorder; }
		void ShowBorder(bool val) { _ShowBorder = val; }
		std::string Text() const { return _Text; }
		void Text(std::string val) { _Text = val; }
		SmCellType CellType() const { return _CellType; }
		void CellType(SmCellType val) { _CellType = val; }
		int TextHAlign() const { return _TextHAlign; }
		void TextHAlign(int val) { _TextHAlign = val; }
		int LeftMargin() const { return _LeftMargin; }
		void LeftMargin(int val) { _LeftMargin = val; }
		int RightMargin() const { return _RightMargin; }
		void RightMargin(int val) { _RightMargin = val; }
		bool ShowMark() const { return _ShowMark; }
		void ShowMark(bool val) { _ShowMark = val; }
		int MarkType() const { return _MarkType; }
		void MarkType(int val) { _MarkType = val; }
		void set_show_position(const bool show_position);
		void set_show_quote_sign(const bool show_quote_sign);
		void set_show_move_rect(const bool show_moving_rect) {
			show_moving_rect_ = show_moving_rect;
		}
	private:
		void draw_arrow
		(
			CBCGPGraphicsManager* pGM,
			const SmOrderGridResource& res,
			const CBCGPPoint& start_point,
			const CBCGPPoint& end_point,
			const double& stroke_width,
			const int& head_width
		);
		// draw a circle by the position type.
		bool show_position_{false};
		// flag for displaying a text by the value type as following high, low, open, close, yesterday close.
		bool show_quote_sign_{ false };
		// display a rectangle over a cell when the mouse is hovering over the cell.
		bool show_moving_rect_{ false };
		void draw_option(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res);
		// draw a circle by the position type.
		void draw_position(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res);
		// display the quote sign by the value type as following high, low, open, close, yesterday close.
		void draw_quote_sign(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res);
		// draw the selected rectangles.
		void draw_moving_rect(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res);
		void draw_header(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res);
		void draw_check_header(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res);
		void draw_cell_by_type(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res);
		void draw_order_buy(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res);
		void draw_order_sell(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res);
	};
}

