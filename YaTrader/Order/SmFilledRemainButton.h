
#pragma once

#include "../Util/SmButtonResource.h"
#include <string>
#include <memory>
namespace DarkHorse {
	class SmSymbol;
}

class SmFundOrderCenterWnd;
class SmFundOrderArea;
class SmFilledRemainButton : public CBCGPStatic
{
public:
	SmFilledRemainButton();
	~SmFilledRemainButton();
	std::string Text() const { return _Text; }
	void Text(std::string val) { _Text = val; }
	bool Selected() const { return _Selected; }
	void Selected(bool val) { _Selected = val; }
	SmFundOrderCenterWnd* FundOrderCenterWnd() const { return _FundOrderCenterWnd; }
	void FundOrderCenterWnd(SmFundOrderCenterWnd* val) { _FundOrderCenterWnd = val; }
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	bool EnableFilledOrder() const { return _EnableFilledOrder; }
	void EnableFilledOrder(bool val) { _EnableFilledOrder = val; }
	void SetFundOrderArea(SmFundOrderArea* fund_order_area);
public:
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void OnQuoteEvent(const std::string& symbol_code);
	void CreateResource();
	void SetUp();
	void SetTextRect(const CRect& rc);

	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	DECLARE_MESSAGE_MAP()
private:
	SmFundOrderArea* _FundOrderArea = nullptr;
	bool _EnableFilledOrder = false;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	SmFundOrderCenterWnd* _FundOrderCenterWnd = nullptr;
	bool _EnableOrderShow = false;
	bool _EnableQuoteShow = false;
	CRect _TextRect;
	SmButtonResource _Resource;
	CBCGPGraphicsManager* m_pGM{ nullptr };
	bool _Fill{ true };
	bool _DrawBorder{ true };
	bool _DrawText{ true };
	std::string _Text;
	BOOL m_bTrackMouse{ FALSE };
	bool _Hover{ false };
	bool _Selected{ false };
	int _Index{ 0 };
	CBCGPScrollBar m_HScroll;
	CBCGPScrollBar m_VScroll;
public:
	int Index() const { return _Index; }
	void Index(int val) { _Index = val; }
	virtual void PreSubclassWindow();
	afx_msg void OnDestroy();
};


