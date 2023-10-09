#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include "SmGridResource.h"
#include "SmCellType.h"
#include "SmGridConsts.h"
#include "SmButtonId.h"

#define SM_INPLACE_CONTROL   8                  // ID of inplace edit controls



namespace DarkHorse {
	
	class SmCell;
	class SmRow;
	class SmGrid
	{
	public:
		SmGrid(SmOrderGridResource& res, const int& row_count, const int& col_count)
			: m_pEditWnd(nullptr), _Res(res), _RowCount(row_count), _ColCount(col_count)
		{}
		~SmGrid() {}

		void SetOrderHeaderTitles();
		// 주어진 높이를 가지고 행 갯수를 다시 한번 정한다.
		int RecalRowCount(const int& height, bool change_close_row);
		void MakeColWidthMap();
		// 행 높이 맵을 만든다. 사용자가 정한 것은 따로 저장하고 
		// 사용자가 지정하지 않은 것은 기본 높이로 정한다.
		void MakeRowHeightMap();
		// 병합할 셀을 지정한다.
		void SetMergeCells();
		void AddMergeCell(const int& row, const int& col, const int& row_span, const int& col_span);
		int index_row() const { return index_row_; }
		void index_row(int val) { index_row_ = val; }
		DarkHorse::SmHeaderMode HeaderMode() const { return _HeaderMode; }
		void HeaderMode(DarkHorse::SmHeaderMode val) { _HeaderMode = val; }
		DarkHorse::SmHeaderPriority HeaderPriority() const { return _HeaderPriority; }
		void HeaderPriority(DarkHorse::SmHeaderPriority val) { _HeaderPriority = val; }

		int GridLineWidth() const { return _GridLineWidth; }
		void GridLineWidth(int val) { _GridLineWidth = val; }
		void Clear();
	private:
		int _GridLineWidth = 1;
		CWnd* m_pEditWnd;
		const SmOrderGridResource& _Res;
		std::vector<std::string> _RowHeaderTitles;
		std::vector<std::string> _ColHeaderTitles;
		// 그리드 높이
		int _Height = 0;
		// 그리드 너비
		int _Width = 0;
		// 버튼 맵. first : row, second : column
		std::set<std::pair<int, int>> _ButtonSet;
		// 기본 설정이 아닌 것만 저장함.
		std::map<int, int> _RowHeightMap;
		// 기본 설정이 아닌 것만 저장함.
		std::map<int, int> _ColWidthMap;
		// 행과 열 갯수를 참조하여 만든 셀 맵
		//std::map <std::pair<int, int>, std::shared_ptr<SmCell>> _CellMap;
		// Key : <row, col> pair, Value : <row_span, col_span> pair
		std::map<std::pair<int, int>, std::pair<int, int>> _MergedCellMap;

		std::map<int, std::shared_ptr<SmRow>> _RowMap;
		
		// 행 갯수
		int _RowCount = 0;
		// 열 갯수
		int _ColCount = 0;
		// 그리드 보이기/감추기
		bool _ShowGrid = true;
		// 행 그리드 선 굵기
		int _RowGridLineHeight = 1;
		// 열 그리드 선 굵기
		int _ColGridWidth = 1;
		int _GridHeight = 0;
		// 종가 행. 최초에 한번 결정되고 
		// 스크롤 할 때만 바뀐다.
		int index_row_{ 15 };
		int _HeaderRow = 0;
		int _HeaderCol = 0;
		int cell_height_ = DefaultRowHeight;
		SmHeaderMode _HeaderMode = SmHeaderMode::HeaderColOnly;
		SmHeaderPriority _HeaderPriority = SmHeaderPriority::ColFirst;
		int IsInMergedCells(const int& row, const int& col);
		void DrawHeader(CBCGPGraphicsManager* pGM,  std::shared_ptr<SmCell> cell);
		void DrawCheckHeader(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		//void DrawVerticalHeader(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		
		void DrawNormalCell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawButtonBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawButtonSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawButtonNormal(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawQuoteClose(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawQuotePreClose(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawQuoteOpen(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawQuoteHigh(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawQuoteLow(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawPositionBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawPositionSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawHogaSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawHogaBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawOrderBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawOrderSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawCheck(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawTickBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawTickSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawRemainSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawRemainBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		//void DrawHeader(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
	public:
		void SetAllRowHeight(const int& height);
		void OnEndEdit();
		bool Edit(CWnd* wnd, int  nRow , int nCol , CRect  rect, UINT nID , UINT  nChar );
		// 행 너비나 열 높이가 변경되었을 때 각 셀의 높이와 너비를 다시 계산해 준다.
		void RecalCells();
		//void RegisterOrderButtons();
		void RegisterOrderButtons(std::map<std::shared_ptr<DarkHorse::SmCell>, BUTTON_ID>& order_button_map);
		// 반드시 셀을 생성한 후에 호출해야 한다.
		void SetColCellType(const int& col, const SmCellType& cell_type);
		void SetRowHeaderTitles(const std::vector<std::string>& row_titles);
		void SetColHeaderTitles(const std::vector<std::string>& col_titles);
		void SetColHeaderTitles(const std::vector<std::string>& col_titles, const std::vector< SmCellType>& cell_types);
		//void ReleaseOrderButtons();
		void ReleaseOrderButtons(const std::map<std::shared_ptr<DarkHorse::SmCell>, BUTTON_ID>& order_button_map);
		
		//void DrawHorizontalHeader(CBCGPGraphicsManager* pGM, const std::vector<std::string>& header_titles, const int& header_row);
		//void DrawVerticalHeader(CBCGPGraphicsManager* pGM, const std::vector<std::string>& header_titles, const int& header_col, const int& start_col = 0);
		// x : x coordinate, y : y coordinate
		std::pair<int, int> FindRowCol(const int& x, const int& y);
		void DrawCell(std::shared_ptr<SmCell> cell, CBCGPGraphicsManager* pGM, 
			const CBCGPBrush& back_brush, 
			const CBCGPBrush& border_brush,
			const CBCGPBrush& text_brush,
			const CBCGPTextFormat& format, 
			const bool& fill,
			const bool& draw_border, const bool& draw_text );
		std::shared_ptr<SmCell> FindCellByPos(const int& x, const int& y);
		std::shared_ptr<SmCell> FindCell(const int& row, const int& col);
		void SetCellMark(const int& row, const int& col, const bool& show_mark);
		void SetCellType(const int& row, const int& col, const SmCellType& cell_type);
		void SetCellText(const int& row, const int& col, const std::string& text);
		void SetTextHAlign(const int& row, const int& col, const int h_align);
		void SetLeftMargin(const int& row, const int& col, const int& margin);
		void SetRightMargin(const int& row, const int& col, const int& margin);
		// 행과 열 갯수를 참조하여 필요한 셀을 만든다.
		void CreateGrids();
		//void CreatePositionGrids();
		void DrawGrid(CBCGPGraphicsManager* pGM, CRect& wnd_area);
		void DrawBorder(CBCGPGraphicsManager* pGM, CRect& wnd_area, const bool& selected = false);
		//void DrawOrderCells(CBCGPGraphicsManager* pGM, CRect& wnd_area);
		//void DrawCells(CBCGPGraphicsManager* pGM, CRect& wnd_area);
		void DrawCells(CBCGPGraphicsManager* pGM, CRect& wnd_area, const bool& use_hor_header = false, const bool& use_ver_header = false);
		void draw_cells(CBCGPGraphicsManager* pGM, CRect& wnd_area, const bool& use_hor_header = false, const bool& use_ver_header = false);
		void DrawSelectedCell(CBCGPGraphicsManager* pGM, const CBCGPBrush& fill_brush, const CBCGPBrush& select_brush, bool selected);
		void SetRowHeight(const int& row, const int& height);
		void SetColWidth(const int& col, const int& width);
		int RowCount() const { return _RowCount; }
		void RowCount(int val) { _RowCount = val; }
		int ColCount() const { return _ColCount; }
		void ColCount(int val) { _ColCount = val; }
		bool ShowGrid() const { return _ShowGrid; }
		void ShowGrid(bool val) { _ShowGrid = val; }
		int ColGridWidth() const { return _ColGridWidth; }
		void ColGridWidth(int val) { _ColGridWidth = val; }
		int Width() const { return _Width; }
		void Width(int val) { _Width = val; }
		int Height() const { return _Height; }
		void Height(int val) { _Height = val; }
		int RowGridHeight() const { return _RowGridLineHeight; }
		void RowGridHeight(int val) { _RowGridLineHeight = val; }
	};
}

