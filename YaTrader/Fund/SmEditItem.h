#pragma once
#include <BCGCBProInc.h>
#include <memory>

namespace DarkHorse {
	class SmFund;
	class SmAccount;
}
class SmFundAccountGrid;
class  SmEditItem : public CBCGPGridItem
{
	// Construction
public:
	SmEditItem(const long& init = 0);

	virtual BOOL OnEndEdit();

	virtual void OnBeginInplaceEdit();
	virtual void OnEndInplaceEdit();

	
	void Account(std::shared_ptr<DarkHorse::SmAccount> val) { _Account = val; }
	
	void FundAccountGrid(SmFundAccountGrid* val) { _FundAccountGrid = val; }
private:
	SmFundAccountGrid* _FundAccountGrid = nullptr;
	CString _AccountNo;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
};

