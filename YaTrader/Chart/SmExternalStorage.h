#pragma once
#include <BCGCBProInc.h>

class SmStockStorage
{
public:
	void AddData(double dblOpen, double dblHigh, double dblLow, double dblClose, const COleDateTime& time)
	{
		CBCGPChartStockData data(dblOpen, dblHigh, dblLow, dblClose, time);
		m_stockData.Add(data);
	}

	CBCGPChartStockData GetData(int nIndex) const
	{
		return m_stockData[nIndex];
	}

	void Clear()
	{
		m_stockData.RemoveAll();
	}

	int GetCount() const { return (int)m_stockData.GetSize(); }

	void ShiftData(CBCGPChartStockData* data, const int& len, const CBCGPChartStockData& newValue)
	{
		memmove(data, data + 1, sizeof(*data) * (len - 1));
		data[len - 1] = newValue;
	}
	

protected:
	CArray<CBCGPChartStockData, CBCGPChartStockData&> m_stockData;
};



class SmLineStorage
{
public:
	void AddData(double dblClose, const COleDateTime& time)
	{
		m_arYValues.Add(dblClose);
		m_arXValues.Add(time.m_dt);
	}

	double GetData(int nIndex) const
	{
		return m_arYValues[nIndex];
	}

	double GetIndexValue(int nIndex) const {
		return m_arXValues[nIndex];
	}

	void Clear()
	{
		m_arYValues.RemoveAll();
		m_arXValues.RemoveAll();
		m_arScreenPoints.RemoveAll();
	}

	int GetCount() const { return (int)m_arXValues.GetSize(); }

	void ShiftData(double* data, const int& len, const double& newValue)
	{
		memmove(data, data + 1, sizeof(*data) * (len - 1));
		data[len - 1] = newValue;
	}

	void ShiftData(const COleDateTime& time, const double& newValue) {
		memmove(m_arYValues.GetData(), m_arYValues.GetData() + 1, sizeof(double) * (m_arYValues.GetSize() - 1));
		m_arYValues[m_arYValues.GetSize() - 1] = newValue;
		memmove(m_arXValues.GetData(), m_arXValues.GetData() + 1, sizeof(double) * (m_arXValues.GetSize() - 1));
		m_arXValues[m_arXValues.GetSize() - 1] = time.m_dt;
	}


public:
	CArray<double, double>	m_arYValues;
	CArray<double, double>	m_arXValues;
	CBCGPPointsArray		m_arScreenPoints;
};


