#pragma once
#include "VtBaseTracker.h"
/// <summary>
/// 트래커는 그리기 객체를 선택하는 것을 표현한다.
/// 다만 그 핸들로 객체를 표현한다.
/// 트래커가 만들어 질 때 그 객체가 가진 꼭지점의 수만큼 핸들이 만들어 진다.
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
	/// 라인트래커를 만든다. 
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

