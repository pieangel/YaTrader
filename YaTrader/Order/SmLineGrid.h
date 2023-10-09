#pragma once
class SmLineGrid : public CBCGPStatic
{
public:
	CBCGPGraphicsManager* m_pGM = nullptr;
	SmLineGrid();
	~SmLineGrid();
	CBCGPColor LineColor = RGB(255, 255, 255);
	CBCGPBrush LineBrush = CBCGPBrush(LineColor);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

