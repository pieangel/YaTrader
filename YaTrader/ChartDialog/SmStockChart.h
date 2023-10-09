#pragma once
#include "afxdialogex.h"

#include <BCGCBProInc.h>
#include "../Chart/SmChartConsts.h"
#include "../TimeData/SmCompData.h"
#include <memory>
#include <map>
#include "chartdir.h"
#include "../Chart/ChartViewer.h"
#include "FinanceChart.h"
#include "../Drawing/VtPoint.h"
#include "../Drawing/VtDrawingConstants.h"

// SmMultiChart dialog
namespace DarkHorse {
	class SmChartData;
	class SmSymbol;
	class SmSystem;
	struct PnfItem;
}

// SmStockChart 대화 상자

class SmStockChart : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmStockChart)

public:
	SmStockChart(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~SmStockChart();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSTEM_CHART };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();

private:
	/// <summary>
	/// 차트를 그린다.
	/// </summary>
	/// <param name="viewer"></param>
	void drawChart(CChartViewer* viewer);
	/// <summary>
	/// 차트의 마우스 움직임을 추적한다.
	/// </summary>
	/// <param name="c"></param>
	/// <param name="mouseX"></param>
	void trackFinance(MultiChart* c, int mouseX);
	/// <summary>
	/// 뷰포트 컨트롤에 차트 전체를 그린다.
	/// </summary>
	/// <param name="vpc"></param>
	void drawFullChart(CViewPortControl* vpc);

	int CalcChartHeight();

	bool _ShowIndicator = false;
	int _IndicatorHeight1 = 0;	
	
	int _IndicatorHeight2 = 0;
	int _ViewPortHeight = 25;
	int _ToolBarHeight = 25;
	DarkHorse::SmChartStyle _MainChartType = DarkHorse::SmChartStyle::OHLC;
	int _MainDataType = 0;
	int _ChartWidth = 1800;
	int _ChartHeight = 800;
	int _PlotAreaGapHeight = 7;
	int _YAxisGap = 40;
	int _LeftMargin = 105;
	int _RightMargin = 0;
	int _XAxisLabelHeight = 14;
	/// <summary>
	/// 차트 뷰어 객체
	/// </summary>
	CChartViewer m_ChartViewer;
	/// <summary>
	/// 뷰포트 컨트롤 객체
	/// </summary>
	CViewPortControl m_ViewPortControl;

	std::shared_ptr<DarkHorse::SmChartData> _ChartData = nullptr;

	std::shared_ptr<DarkHorse::SmSystem> _System = nullptr;

	/// <summary>
	/// 현재 마우스 위치 X, Y 좌표
	/// </summary>
	int _MouseX = 0;
	int _MouseY = 0;

	/// <summary>
	/// 초기화 되었음을 설정한다.
	/// </summary>
	bool _InitChart = false;

	bool _InitDlg = false;

	bool _Enable = true;

	/// <summary>
	/// 편집 모드 :
	/// </summary>
	EditStatus _EditMode = EditStatus::None;

	/// <summary>
	/// 편집 모드에서 객체의 현재 위치 X, Y 좌표 - 픽셀
	/// </summary>
	int _MovingX = 0;
	int _MovingY = 0;

	/// <summary>
	/// 기본 선 색 - 객체를 그릴때 기본적으로 선택된다.
	/// </summary>
	VtColor _PenColor;
	/// <summary>
	/// 기본적인 채우기 색 - 객체를 그릴때 기본적으로 채워지는 색
	/// </summary>
	VtColor _FillColor;

	static const int initialFullRange = 0;

	void InitChartDataCombo();
	void RecalLayout();

	std::map<int, std::shared_ptr<DarkHorse::SmChartData>> _ChartDataMap;
	std::map<int, std::shared_ptr<DarkHorse::SmSystem>> _SystemMap;
	void SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data);
	void ChangeChartData(std::shared_ptr<DarkHorse::SmChartData> new_chart_data);

	void DrawPnf(const int NbPoints, const std::map<double, std::shared_ptr<DarkHorse::PnfItem>>& wave_map, const DoubleArray& timeStamp, XYChart* const chart);
	void DrawTitleValue(FinanceChart* m);
	void DrawCurrentValue(FinanceChart* m);
	void OnChartUpdateTimer();

	void InitChart(std::shared_ptr<DarkHorse::SmChartData> chart_data);
	void InitSystemCombo();
	void InitTypeCombo();
public:
	CBCGPComboBox _ComboSymbol;
	CBCGPComboBox _ComboCycle;
	CBCGPComboBox _ComboTick;

	afx_msg void OnViewPortChanged();
	afx_msg void OnMouseMovePlotArea();
	afx_msg void OnMouseMoveOverChart();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeComboSymbol();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CBCGPComboBox _ComboStyle;
	afx_msg void OnCbnSelchangeComboStyle();
	CBCGPComboBox _ComboSystem;
	CBCGPComboBox _ComboData;
	afx_msg void OnCbnSelchangeComboData();
	CBCGPComboBox _ComboJoin;
	afx_msg void OnBnClickedBtnJoin();
	afx_msg void OnCbnSelchangeComboJoinIndex();
};
