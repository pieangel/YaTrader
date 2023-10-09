#include "stdafx.h"
#include "SmFundItem.h"


#include "../Fund/SmFund.h"
#include "SmFundGrid.h"

SmFundItem::SmFundItem(const long& init)
	: CBCGPGridItem(init)
{
}

BOOL SmFundItem::OnEndEdit()
{
	return CBCGPGridItem::OnEndEdit();
}

void SmFundItem::OnBeginInplaceEdit()
{
	CBCGPGridItem::OnBeginInplaceEdit();
}

void SmFundItem::OnEndInplaceEdit()
{
	int value = GetValue();

	//if (_Account) _Account->SeungSu(value);
	//if (_FundAccountGrid) _FundAccountGrid->InitFund();

	CBCGPGridItem::OnEndInplaceEdit();
}
