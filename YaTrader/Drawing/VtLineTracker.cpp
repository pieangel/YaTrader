
#include "stdafx.h"
#include "VtLineTracker.h"
#include "VtPointHandle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VtLineTracker::VtLineTracker()
{
}


VtLineTracker::~VtLineTracker()
{
}

void VtLineTracker::Setup()
{
	//NW
	_Handles.push_back(new VtPointHandle(this, RedimStatus::MovePoint, &_Parent->Location0));
	//SE
	_Handles.push_back(new VtPointHandle(this, RedimStatus::MovePoint, &_Parent->Location1));
}

void VtLineTracker::Setup(XYChart* chart)
{
	//NW
	_Handles.push_back(new VtPointHandle(chart, this, RedimStatus::MovePoint, &_Parent->Location0));
	//SE
	_Handles.push_back(new VtPointHandle(chart, this, RedimStatus::MovePoint, &_Parent->Location1));
}

void VtLineTracker::CreateHandlers()
{
	//NW
	_Handles.push_back(new VtPointHandle(this, RedimStatus::MovePoint, _Parent->GetStartValuePointer()));
	//SE
	_Handles.push_back(new VtPointHandle(this, RedimStatus::MovePoint, _Parent->GetEndValuePointer()));
}

void VtLineTracker::Draw(int startID, int zoomStartIndex, DrawArea * d, XYChart * chart, int dx, int dy)
{
	VtBaseTracker::Draw(startID, zoomStartIndex, d, chart, dx, dy);
}

void VtLineTracker::Draw(DoubleArray& timeStamp, XYChart* chart)
{
	VtBaseTracker::Draw(timeStamp, chart);
}

void VtLineTracker::MovePoint(int x, int y)
{
	for (auto i = _Handles.begin(); i != _Handles.end(); ++i)
	{
		if ((*i)->IsSelected())
		{
			// 핸들을 움직여 준다.
			(*i)->Move(x, y);
			// 핸들과 연결된 객체의 꼭지점을 움직여 준다.
			((VtPointHandle*)(*i))->LinkedPoint()->SetXY((*i)->GetX0(), (*i)->GetY0());
		}
	}
}

void VtLineTracker::MovePoint(XYChart* chart, int x, int y)
{
	for (auto i = _Handles.begin(); i != _Handles.end(); ++i)
	{
		if ((*i)->IsSelected())
		{
			(*i)->Move(chart, x, y);
			if (_Parent != nullptr)
			{
				_Parent->MovePoint(chart, x, y, ((VtPointHandle*)(*i))->LinkedPoint());
			}
		}
	}
}

