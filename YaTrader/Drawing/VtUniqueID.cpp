#include "stdafx.h"
#include "VtUniqueID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int VtUniqueID::_current_id = 1;

VtUniqueID::VtUniqueID()
	: _id(_current_id++)
{
}


VtUniqueID::~VtUniqueID()
{
}


