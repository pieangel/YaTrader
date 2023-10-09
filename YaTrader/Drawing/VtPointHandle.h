#pragma once
#include "VtHandle.h"
#include "VtDrawingConstants.h"
/// <summary>
/// 포인트 핸들 - 포인트를 잡고 도형을 변형할 때 사용된다.
/// </summary>
class VtPointHandle :
	public VtHandle
{
private:
	/// <summary>
	/// 핸들을 가지고 있는 부모 객체 - 도형
	/// </summary>
	VtElement* _Parent;
	/// <summary>
	/// 핸들이 참고하고 있는 포인트
	/// </summary>
	VtPoint*    _LinkedPoint;
	/// <summary>
	/// 핸들이 참조하고 있는 객체의 한 점에 연결된 포인트
	/// </summary>
	SmPoint* _SourcePoint;

public:
	VtPointHandle();
	virtual ~VtPointHandle();
	VtPointHandle(VtElement* e, RedimStatus o, VtPoint* pt);
	VtPointHandle(XYChart* chart, VtElement* e, RedimStatus o, VtPoint* pt);
	VtPointHandle(VtElement* e, RedimStatus o, SmPoint* pt);
	virtual void RePosition(VtElement* e);
	virtual void RePosition(XYChart* chart, VtElement* e);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);

	virtual void Draw(DoubleArray& timeStamps, XYChart* chart);
	/*
	virtual void SyncPoint()
	{
		if (_LinkedPoint != nullptr)
		{
			_LinkedPoint->x = Location0.x;
			_LinkedPoint->y = Location0.y;
		}
	}
	*/
	// 핸들을 옮겨 준다.
	virtual void Move(int x, int y);
	virtual void Move(XYChart* chart, int x, int y);
	VtPoint* LinkedPoint() const { return _LinkedPoint; }
	void LinkedPoint(VtPoint* val) { _LinkedPoint = val; }
	SmPoint* SourcePoint() const { return _SourcePoint; }
	void SourcePoint(SmPoint* val) { _SourcePoint = val; }
};

