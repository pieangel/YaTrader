#include "stdafx.h"
#include "SmAccountItem.h"
#include "../Account/SmAccount.h"
#include "SmFundAccountGrid.h"

SmAccountItem::SmAccountItem(const long& init)
	: CBCGPGridItem(init)
{
}

BOOL SmAccountItem::OnEndEdit()
{
	return CBCGPGridItem::OnEndEdit();
}

void SmAccountItem::OnBeginInplaceEdit()
{
	CBCGPGridItem::OnBeginInplaceEdit();
}

void SmAccountItem::OnEndInplaceEdit()
{
	int value = GetValue();

	if (_Account) _Account->SeungSu(value);
	if (_FundAccountGrid) _FundAccountGrid->InitFund();

	CBCGPGridItem::OnEndInplaceEdit();
}
