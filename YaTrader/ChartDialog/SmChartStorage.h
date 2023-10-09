#pragma once
#include <BCGCBProInc.h>
#include <map>
#include <string>

class CExternalStorage
{
public:
	void AddData(double dblOpen, double dblHigh, double dblLow, double dblClose, const COleDateTime& time)
	{
		CString strTime = time.Format("%Y-%m-%d %I:%M:%S");
		CBCGPChartStockData data(dblOpen, dblHigh, dblLow, dblClose, time);
		m_stockData.Add(data);
		m_Index[(const char*)strTime] = time;
	}

	CBCGPChartStockData GetData(int nIndex) const
	{
		return m_stockData[nIndex];
	}

	int GetIndex(const COleDateTime& date_time) {
		CString strTime = date_time.Format("%Y-%m-%d %I:%M:%S");
		auto found = m_Index.find((const char*)strTime);
		if (found == m_Index.end()) return -1;


	}

	void Clear()
	{
		m_stockData.RemoveAll();
	}

	int GetCount() const { return (int)m_stockData.GetSize(); }

	void ShiftData(const CBCGPChartStockData& newValue)
	{
		memmove(m_stockData.GetData(), m_stockData.GetData() + 1, sizeof(CBCGPChartStockData) * (GetCount() - 1));
		m_stockData.GetData()[GetCount() - 1] = newValue;
	}

	void UpdateData(const double& dblOpen, const double& dblHigh, const double& dblLow, const double& dblClose) {
		m_stockData[GetCount() - 1].m_dblOpen = dblOpen;
		m_stockData[GetCount() - 1].m_dblHigh = dblHigh;
		m_stockData[GetCount() - 1].m_dblLow = dblLow;
		m_stockData[GetCount() - 1].m_dblClose = dblClose;
	}

	void UpdateData(const double& dblClose) {
		if (dblClose > m_stockData[GetCount() - 1].m_dblHigh)
			m_stockData[GetCount() - 1].m_dblHigh = dblClose;
		if (dblClose < m_stockData[GetCount() - 1].m_dblLow)
			m_stockData[GetCount() - 1].m_dblLow = dblClose;
		m_stockData[GetCount() - 1].m_dblClose = dblClose;
	}

protected:
	typedef CArray<CBCGPChartStockData, CBCGPChartStockData&> CBCGPStockDataArray;
	CBCGPStockDataArray m_stockData;
	std::map<std::string, COleDateTime> m_Index;
};

class CCustomStockSeries : public CBCGPChartStockSeries
{
protected:
	CCustomStockSeries()
	{
		m_pStorage = NULL;
	}

public:
	CCustomStockSeries(CBCGPChartVisualObject* pChartCtrl, CBCGPBaseChartStockSeries::StockSeriesType seriesType) :
		CBCGPChartStockSeries(pChartCtrl, seriesType)
	{
		m_pStorage = NULL;
	}

	void SetExternalStorage(CExternalStorage* pStorage)
	{
		m_pStorage = pStorage;
	}

	virtual BOOL IsOptimizedLongDataMode() const { return TRUE; }
	virtual int GetDataPointCount() const
	{
		if (m_pStorage == NULL)
		{
			return 0;
		}

		return m_pStorage->GetCount();
	}

	virtual CBCGPChartStockData GetStockDataAt(int nDataPointIndex) const
	{
		int nDPCount = GetDataPointCount();
		if (nDPCount == 0 || nDataPointIndex >= nDPCount)
		{
			return CBCGPChartStockData();
		}

		return m_pStorage->GetData(nDataPointIndex);
	}

	virtual CBCGPChartValue GetCustomStockValue(int nDataPointIndex) const
	{
		CBCGPChartStockData data = GetStockDataAt(nDataPointIndex);
		return CBCGPChartValue((data.m_dblHigh + data.m_dblLow) / 2);
	}

protected:
	CExternalStorage* m_pStorage;
};