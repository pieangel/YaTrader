#pragma once
#include <memory>
#include <vector>
#include <string>
#include <set>

#include "SmGridResource.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
}

// 셀을 미리 만들어 놓고 매칭 시킨다. 
// 그리고 만들어진 셀에 대하여 부분적으로 보여준다.
class SmAcceptedArea : public CBCGPStatic
{
public:
	SmAcceptedArea();
	~SmAcceptedArea();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
	
public:
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	void UpdateAcceptedOrder();
	size_t GetRowCount();

	void UpdateAccountFilledOrders();
	void UpdateFundFilledOrders();
	void Update();
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void RecalSize();
private:
	void Clear();
	bool _Init = false;
	bool _EnableOrderShow = false;
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::set<int> _OldContentRowSet;
	void ClearOldCotents();
	void ProcessCheck(const CPoint& point);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
};

