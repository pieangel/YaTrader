#include "stdafx.h"
#include "SmEditItem.h"
#include "../Account/SmAccount.h"
#include "SmFundAccountGrid.h"

SmEditItem::SmEditItem(const long& init)
	: CBCGPGridItem(init)
{
}

BOOL SmEditItem::OnEndEdit()
{
	return CBCGPGridItem::OnEndEdit();
}

void SmEditItem::OnBeginInplaceEdit()
{
	CBCGPGridItem::OnBeginInplaceEdit();
}

void SmEditItem::OnEndInplaceEdit()
{
	int value = GetValue();

	if (_Account) _Account->SeungSu(value);
	if (_FundAccountGrid) _FundAccountGrid->InitFund();
	
	CBCGPGridItem::OnEndInplaceEdit();
}
