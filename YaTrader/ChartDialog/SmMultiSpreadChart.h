#pragma once
#include <BCGCBProInc.h>
#include "SmChartStorage.h"
#include <memory>
#include <map>
#include "../Chart/SmChartConsts.h"
#include "../TimeData/SmCompData.h"
#include "../Chart/SmExternalStorage.h"
#include "../DataFrame/DataFrame.h"
#include "../Chart/ChartViewer.h"

// SmSpreadChart dialog
namespace DarkHorse {
	class SmChartData;
	class SmSymbol;
	class SmTimeData;
	class SmCompData;
}
#define MAX_SERIES 7

class SmSetSpread;
class CVirtualSeries : public CBCGPChartLineSeries
{
public:
	CVirtualSeries(CBCGPChartVisualObject* pChart) : CBCGPChartLineSeries(pChart)
	{

	}

	void Clear() {
		m_arXValues.RemoveAll();
		m_arYValues.RemoveAll();
		m_arScreenPoints.RemoveAll();
	}


	/// Sample data creation
	void CreateData(int nDataPointCount)
	{
		m_arXValues.SetSize(nDataPointCount);
		m_arYValues.SetSize(nDataPointCount);
		m_arScreenPoints.SetSize(nDataPointCount);

		COleDateTime dtCurrTime = COleDateTime::GetCurrentTime();
		COleDateTime dtStart(dtCurrTime.GetYear(), 1, 1, 0, 0, 0);
		COleDateTimeSpan dtInterval(0, 0, 1, 0);

		double dblVal = 0;
		for (int i = 0; i < nDataPointCount; i++)
		{
			double dblDelta = 100;
			dblVal += dblDelta;

			m_arXValues[i] = dtStart.m_dt;
			m_arYValues[i] = dblVal;

			dtStart += dtInterval;
		}
	}

	COleDateTime GetFirstDate() const
	{
		return m_arXValues[0];
	}

	COleDateTime GetLastDate() const
	{
		return m_arXValues[m_arXValues.GetSize() - 1];
	}

	virtual int AddDataPoint(double dblY, double dblX, BCGPChartFormatSeries* pDataPointFormat = NULL, DWORD_PTR dwUserData = 0) {
		m_arYValues.Add(dblY);
		m_arXValues.Add(dblX);
		m_arScreenPoints.SetSize(m_arXValues.GetSize());
		return m_arXValues.GetSize();
	}

	virtual BOOL SetDataPointValue(int nDataPointIndex, double dblValue, CBCGPChartData::ComponentIndex ci = CBCGPChartData::CI_Y) {
		if (m_arXValues.GetSize() == 0) return TRUE;

		if (ci == CBCGPChartData::CI_X) {
			m_arXValues[nDataPointIndex] = dblValue;
		}
		if (ci == CBCGPChartData::CI_Y) {
			m_arYValues[nDataPointIndex] = dblValue;
		}

		return TRUE;
	}

	/// Data point management - called by Chart
	virtual int GetDataPointCount() const
	{
		if (m_arXValues.GetSize() == 0) return 0;
		return (int)m_arXValues.GetSize();
	}

	virtual const CBCGPChartDataPoint* GetDataPointAt(int nIndex) const
	{
		// m_virtualPoint is defined in CBCGPChartSeries and can be used to return wrapped data,
		// but it has to be unconsted
		CBCGPChartDataPoint* pDP = (CBCGPChartDataPoint*)&m_virtualPoint;
		if (m_arXValues.GetSize() == 0) return pDP;

		pDP->SetComponentValue(m_arYValues[nIndex]);
		pDP->SetComponentValue(m_arXValues[nIndex], CBCGPChartData::CI_X);

		return &m_virtualPoint;
	}

	virtual CBCGPChartValue GetDataPointValue(int nDataPointIndex, CBCGPChartData::ComponentIndex ci) const
	{
		if (ci == CBCGPChartData::CI_Y)
		{
			return m_arYValues[nDataPointIndex];
		}

		if (ci == CBCGPChartData::CI_X)
		{
			return m_arXValues[nDataPointIndex];
		}

		return CBCGPChartValue();
	}
	/// End Data point 

	/// Screen position management - called by Chart
	/// nScreenPointIndex is usually required for stacked charts
	virtual void SetDataPointScreenPoint(int nDataPointIndex, int /*nScreenPointIndex*/, CBCGPPoint pt)
	{
		if (m_arScreenPoints.GetSize() == 0) return;

		m_arScreenPoints[nDataPointIndex] = pt;
	}

	virtual CBCGPPoint GetDataPointScreenPoint(int nDataPointIndex, int /*nScreenPointIndex*/) const
	{
		if (m_arScreenPoints.GetSize() == 0) return CBCGPPoint();
		return m_arScreenPoints[nDataPointIndex];
	}

	virtual BOOL IsDataPointScreenPointsEmpty(int nDataPointIndex) const
	{
		if (m_arScreenPoints.GetSize() == 0) return FALSE;
		return (m_arScreenPoints.GetSize() == 0 || nDataPointIndex >= m_arScreenPoints.GetSize());
	}
	/// End screen position management

protected:
	CArray<double, double>	m_arYValues;
	CArray<double, double>	m_arXValues;
	CBCGPPointsArray		m_arScreenPoints;
};

class SmSymbolTableDialog;
// SmMultiSpreadChart dialog
class SmMultiSpreadChart : public CBCGPDialog
{
	DECLARE_DYNAMIC(SmMultiSpreadChart)

public:
	SmMultiSpreadChart(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmMultiSpreadChart();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MULTI_SPREAD_CHART };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void ShowSeries(const int& series);
	virtual BOOL OnInitDialog();

	afx_msg LRESULT OnCompDataAdded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCompDataUpdated(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnSet();
	afx_msg void OnBnClickedBtnFind();
	afx_msg void OnCbnSelchangeComboStyle();
	afx_msg void OnClear();
	afx_msg LRESULT OnChartAxisZoomed(WPARAM wp, LPARAM lp);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnUmSymbolSelected(WPARAM wParam, LPARAM lParam);
	void UpdateChartColorTheme(int nTheme, BOOL bIsDarkTheme);
	void SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& window_id, const int& series_index);
	void SetChartDataForMain(std::shared_ptr<DarkHorse::SmChartData> chart_data);
	void SetChartDataForSecond(std::shared_ptr<DarkHorse::SmChartData> chart_data);
	void SetChartDataForOthers(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& series_index);
	// action :: 1 : update, 2 : add
	void OnChartEvent(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action);
private:
	std::shared_ptr< SmSetSpread> _SpreadSet = nullptr;
	CBCGPChartAxisY* m_pAxisY1;
	CBCGPChartAxisY* m_pAxisY2;

	CBCGPBrush			m_brLine1;
	CBCGPBrush			m_brLine2;
	CBCGPBrush			m_brLine3;
	CBCGPBrush			m_brLine4;
	CBCGPBrush			m_brLine5;
	CBCGPBrush			m_brDeltaOutline;
	CBCGPStrokeStyle	m_strokeStyle;

	CBCGPBrush			m_brFill1;
	CBCGPBrush			m_brFill2;
	CBCGPBrush			m_brFill3;
	CBCGPBrush			m_brFill4;
	CBCGPBrush			m_brFill5;
	CBCGPBrush			m_brDeltaText;

	CBCGPBrush			m_brAxisMarkFill;

	CBCGPBrush			m_brText1;
	CBCGPBrush			m_brText2;
	CBCGPBrush			m_brText3;
	CBCGPBrush			m_brText4;
	CBCGPBrush			m_brText5;

	CBCGPBrush			m_brUp;
	CBCGPBrush			m_brDown;

	CBCGPBrush			m_brFillText1;
	CBCGPBrush			m_brFillText2;
	CBCGPBrush			m_brFillText3;
	CBCGPBrush			m_brFillText4;
	CBCGPBrush			m_brFillText5;
	CBCGPBrush			m_brDeltaFill;

	CBCGPBrush			m_brZeroLine;

	CBCGPBrush m_brText;
	CBCGPBrush m_brFill;
	CBCGPBrush m_brBorder;


	CBCGPChartTextObject* m_pMainLabel;
	CBCGPChartTextObject* m_pCompLabel;
	CBCGPChartTextObject* m_pVirLabel1;
	CBCGPChartTextObject* m_pVirLabel2;
	CBCGPChartTextObject* m_pVirLabel3;
	CBCGPChartLineObject* m_pLine1;
	CBCGPChartLineObject* m_pLine2;

	CBCGPChartTextObject* m_pDeltaLabel;
	CBCGPChartTextObject* m_pDeltaLabel2;

	CBCGPChartTextObject* m_pCloseLabel;

	CBCGPChartTextObject* m_pCloseLabel1;
	CBCGPChartTextObject* m_pCloseLabel2;
	CBCGPChartTextObject* m_pCloseLabel3;
	CBCGPChartTextObject* m_pCloseLabel4;
	CBCGPChartTextObject* m_pCloseLabel5;
	CBCGPChartTextObject* m_pCloseLabel6;

	std::vector< CBCGPChartTextObject*> _OpenTextVec;
	std::vector< CBCGPChartTextObject*> _CloseTextVec;

	DarkHorse::SmCompData _ChartData;
	const CBCGPChartTheme& GetCustomTheme(int nTheme)
	{
		return nTheme == -2 ? m_CustomColorTheme : m_CustomTexturedTheme;
	}

	CBCGPChartTheme		m_CustomColorTheme;
	CBCGPChartTheme		m_CustomTexturedTheme;
	CBCGPToolBarImages	m_Thumbnails;

	BOOL m_bIsDarkTheme;
	int m_nColorTheme = -2;
	void InitTheme();
	std::shared_ptr< SmSymbolTableDialog> _SymbolTableDlg = nullptr;
	bool _NeedUpdate = false;
	bool _NeedAdd = false;
	bool _NeedRedraw = false;
	int _UniqueId = -1;
	void InitSymbolCombo();
	// key : row index, value : smsymbol object
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToMainSymbolMap;
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToCompSymbolMap;
	virtual void OnChangeBackgroundDarkness() {}
	BOOL					m_bIsDarkBackground;
	BOOL					m_bIsTexturedTheme;
	BOOL					m_bIsFlatTheme;
	UINT					m_nThumbnailFlags;

	int		m_nChartType;
	int		m_nLineType;
	int		m_nOverlayLine;
	int		m_nOverlayBand;

	BOOL	m_bAutoScroll;
	BOOL	m_bDataTable;

	int _Multiply = 1;

	void CreateChart();
	
	void LoadChartData();
	void RequestChartData();
	bool _bDataReady = false;
	CVirtualSeries* m_arSeries[MAX_SERIES];

	CBCGPChartCtrl	m_wndChart;
	void SetDefaultLineWidth();
	void ProcessAddData();
	void ProcessUpdateData();
	void InitMulCombo();
public:
	CBCGPComboBox _ComboTheme;
	afx_msg void OnCbnSelchangeComboTheme();
	CBCGPButton _CheckComp;
	CBCGPButton _CheckMain;
	CBCGPButton _CheckVirtual;
	CBCGPComboBox _ComboSymbolComp;
	CBCGPComboBox _ComboSymbolMain;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnBnClickedCheckMain();
	afx_msg void OnBnClickedCheckComp();
	afx_msg void OnBnClickedCheckVirtual();
	afx_msg void OnCbnSelchangeComboSymbolMain();
	CBCGPComboBox _ComboMul;

	afx_msg void OnCbnSelchangeComboMul();
	afx_msg void OnBnClickedBtnZoomOut();
	afx_msg void OnBnClickedBtnZoomIn();
	afx_msg void OnBnClickedBtnRecal();
	afx_msg void OnEnChangeEditNq();
	afx_msg void OnEnChangeEditNk();
	afx_msg void OnEnChangeEditHs();
	CBCGPEdit _EditHs;
	CBCGPEdit _EditNk;
	CBCGPEdit _EditNq;
	CBCGPEdit _EditAFactor;
	CBCGPEdit _EditBFactor;
	afx_msg void OnEnChangeEditAfactor();
	afx_msg void OnEnChangeEditBfactor();
};
