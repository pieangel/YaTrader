#pragma once
#include "../resource.h"
enum CmdMode {
	CM_REFRESH,
	CM_HIDE_LEFT,
	CM_SHOW_LEFT,
	CM_ADD_CENTER,
	CM_DEL_CENTER,
	CM_HIDE_TICK,
	CM_SHOW_TICK,
	CM_HIDE_RIGHT,
	CM_SHOW_RIGHT
};

static UINT BASED_CODE order_wnd_indicators[] =
{
	ID_INDICATOR_NISH,
	ID_INDICATOR_TIME
};
