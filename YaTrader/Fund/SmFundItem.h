

#pragma once
#include <BCGCBProInc.h>
#include <memory>

namespace DarkHorse {
	class SmFund;
	class SmAccount;
}
class SmFundGrid;
class  SmFundItem : public CBCGPGridItem
{
	// Construction
public:
	SmFundItem(const long& init = 0);

	virtual BOOL OnEndEdit();

	virtual void OnBeginInplaceEdit();
	virtual void OnEndInplaceEdit();


	void Fund(std::shared_ptr<DarkHorse::SmFund> val) { _Fund = val; }

	void FundGrid(SmFundGrid* val) { _FundGrid = val; }
private:
	SmFundGrid* _FundGrid = nullptr;
	CString _AccountNo;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;
};

