#pragma once
#include <BCGCBProInc.h>	

struct SmButtonResource {
	// Border Color
	CBCGPColor BorderColor_Black = RGB(255, 255, 255);
	CBCGPColor BorderColor = RGB(0x6c, 0x6c, 0x6c);
	// Background Color
	CBCGPColor BackColor = RGB(0x28, 0x28, 0x28);
	CBCGPColor BackColor_Black = RGB(0xff, 0xff, 0xff);
	// Text Color
	CBCGPColor TextColor_Black = RGB(255, 255, 255);
	CBCGPColor TextColor = RGB(0, 0, 0);
	// hover Color
	CBCGPColor HoverColor = RGB(255, 0, 0);
	// Selected Color
	CBCGPColor SelectColor = RGB(0x00, 0x7A, 0xCC);

	CBCGPColor FilledColor = RGB(0xf0, 0x70, 0x00);

	CBCGPColor DownColor = RGB(0, 0, 255);


	CBCGPBrush BorderBrush = CBCGPBrush(BorderColor);
	CBCGPBrush BackBrush = CBCGPBrush(BackColor_Black);
	CBCGPBrush TextBrush = CBCGPBrush(TextColor);
	CBCGPBrush HoverBrush = CBCGPBrush(HoverColor);
	CBCGPBrush SelectBrush = CBCGPBrush(SelectColor);

	CBCGPBrush FilledBrush = CBCGPBrush(FilledColor);

	CBCGPBrush OrderBrush = CBCGPBrush(FilledColor);
	CBCGPBrush DownBrush = CBCGPBrush(DownColor);


	CBCGPTextFormat	TextFormat;
	CBCGPTextFormat	LeftTextFormat;
	CBCGPTextFormat	RightTextFormat;
	
	CBCGPStrokeStyle ButtonStroke;
};