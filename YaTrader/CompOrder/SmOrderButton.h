#pragma once
#include "../Util/SmButtonResource.h"
#include <string>
#define UM_CHANGE_SYMBOL WM_USER + 1
class SmOrderRightComp;
class SmFundCompRightDialog;
class SmOrderButton : public CBCGPStatic
{
public:
	SmOrderButton();
	~SmOrderButton();
	std::string Text() const { return _Text; }
	void Text(std::string val) { _Text = val; }
	bool Selected() const { return _Selected; }
	void Selected(bool val) { _Selected = val; }
	SmOrderRightComp* RightWnd() const { return _RightWnd; }
	void RightWnd(SmOrderRightComp* val) { _RightWnd = val; }
	SmFundCompRightDialog* RightCompFund() const { return _RightCompFund; }
	void RightCompFund(SmFundCompRightDialog* val) { _RightCompFund = val; }
public:
	void CreateResource();
	void SetUp();
	void SetTextRect(const CRect& rc);

	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
private:
	SmFundCompRightDialog* _RightCompFund = nullptr;
	SmOrderRightComp* _RightWnd = nullptr;
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
};


