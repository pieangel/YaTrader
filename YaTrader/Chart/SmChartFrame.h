#pragma once
#include <BCGCBProInc.h>
#include "SmExternalStorage.h"

class SmChartFrame : public CBCGPChartVisualObject
{
	DECLARE_DYNCREATE(SmChartFrame)
public:
	SmChartFrame();
	int Id() const { return _Id; }
	void Id(int val) { _Id = val; }
	bool Selected() const { return _Selected; }
	void Selected(bool val) { _Selected = val; }
	void Init();
private:
	SmStockStorage _Storage;
	int _Id{ 0 };
	bool _Selected = false;
};
