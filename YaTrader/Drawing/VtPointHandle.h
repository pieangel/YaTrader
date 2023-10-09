#pragma once
#include "VtHandle.h"
#include "VtDrawingConstants.h"
/// <summary>
/// ����Ʈ �ڵ� - ����Ʈ�� ��� ������ ������ �� ���ȴ�.
/// </summary>
class VtPointHandle :
	public VtHandle
{
private:
	/// <summary>
	/// �ڵ��� ������ �ִ� �θ� ��ü - ����
	/// </summary>
	VtElement* _Parent;
	/// <summary>
	/// �ڵ��� �����ϰ� �ִ� ����Ʈ
	/// </summary>
	VtPoint*    _LinkedPoint;
	/// <summary>
	/// �ڵ��� �����ϰ� �ִ� ��ü�� �� ���� ����� ����Ʈ
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
	// �ڵ��� �Ű� �ش�.
	virtual void Move(int x, int y);
	virtual void Move(XYChart* chart, int x, int y);
	VtPoint* LinkedPoint() const { return _LinkedPoint; }
	void LinkedPoint(VtPoint* val) { _LinkedPoint = val; }
	SmPoint* SourcePoint() const { return _SourcePoint; }
	void SourcePoint(SmPoint* val) { _SourcePoint = val; }
};

