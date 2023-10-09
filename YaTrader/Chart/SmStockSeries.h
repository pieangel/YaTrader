#pragma once
#include "SmExternalStorage.h"
#include <BCGCBProInc.h>
class SmStockSeries : public CBCGPChartStockSeries
{
protected:
	SmStockSeries()
	{
		m_pStorage = NULL;
	}

public:
	SmStockSeries(CBCGPChartVisualObject* pChartCtrl, CBCGPBaseChartStockSeries::StockSeriesType seriesType) :
		CBCGPChartStockSeries(pChartCtrl, seriesType)
	{
		m_pStorage = NULL;
	}

	void SetExternalStorage(SmStockStorage* pStorage)
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
	SmStockStorage* m_pStorage;
};
