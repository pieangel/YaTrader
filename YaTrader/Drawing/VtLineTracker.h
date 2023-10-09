#pragma once
#include "VtBaseTracker.h"
/// <summary>
/// Ʈ��Ŀ�� �׸��� ��ü�� �����ϴ� ���� ǥ���Ѵ�.
/// �ٸ� �� �ڵ�� ��ü�� ǥ���Ѵ�.
/// Ʈ��Ŀ�� ����� �� �� �� ��ü�� ���� �������� ����ŭ �ڵ��� ����� ����.
/// </summary>
class VtLineTracker :
	public VtBaseTracker
{
public:
	VtLineTracker(VtElement* e)
		: VtBaseTracker(e)
	{
		Setup();
	}

	VtLineTracker(XYChart* chart, VtElement* e)
		: VtBaseTracker(chart,e)
	{
		Setup(chart);
	}
	/// <summary>
	/// ����Ʈ��Ŀ�� �����. 
	/// </summary>
	/// <param name="chart"></param>
	/// <param name="timestamps"></param>
	/// <param name="e"></param>
	VtLineTracker(XYChart* chart, DoubleArray& timestamps, VtElement* e)
		: VtBaseTracker(chart, timestamps, e)
	{
		CreateHandlers();
	}

	VtLineTracker();
	virtual ~VtLineTracker();
	void Setup();
	void Setup(XYChart* chart);
	void CreateHandlers();
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	virtual void Draw(DoubleArray& timeStamp, XYChart* chart);
	virtual void MovePoint(int x, int y);
	virtual void MovePoint(XYChart* chart, int x, int y);
};

