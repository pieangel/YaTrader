#pragma once
#include <BCGCBProInc.h>
#include "SmChartStorage.h"
#include <memory>
#include <map>
#include "../Chart/SmChartConsts.h"
#include "../TimeData/SmCompData.h"
// SmSpreadChart dialog
namespace DarkHorse {
	class SmChartData;
	class SmSymbol;
}
class SmSymbolTableDialog;

// GaSpreadChart dialog

class GaSpreadChart : public CBCGPDialog
{
	DECLARE_DYNAMIC(GaSpreadChart)

public:
	GaSpreadChart(CWnd* pParent = nullptr);   // standard constructor
	virtual ~GaSpreadChart();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GA_MULTI_SPREAD_CHART };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	// action :: 1 : update, 2 : add
	void OnChartEvent(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action);
private:
	void InitChart();
	void InitSymbolCombo();
	void InitChartData();
private:
	DarkHorse::SmCompData _CompData;
	// key : row index, value : smsymbol object
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToMainSymbolMap;
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> _RowToCompSymbolMap;
	CBCGPComboBox _ComboSymbolComp;
	CBCGPComboBox _ComboSymbolMain;
	HWND m_hPE;
	int _UniqueId = -1;
};
