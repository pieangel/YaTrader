#pragma once
#include <BCGCBProInc.h>	

struct SmOrderGridResource {
	// 헤더 버튼 색상
	CBCGPColor HeaderButtonColor_Black = RGB(0xff, 0xff, 0xff);
	CBCGPColor HeaderButtonColor = RGB(0x6c, 0x6c, 0x6c);
	// 헤더 배경 색상
	CBCGPColor HeaderBackColor_Black = RGB(0x28, 0x28, 0x28);
	CBCGPColor HeaderBackColor = RGB(0xd2, 0xe0, 0xed);
	// 매도 버튼 색상
	//CBCGPColor SellButtonColor = RGB(0x10, 0x84, 0xF9);
	CBCGPColor SellButtonColor_Black = RGB(0, 0, 255);
	// 매수 버튼 색상
	//CBCGPColor BuyButtonColor = RGB(0xf0, 0x33, 0x3a);
	CBCGPColor BuyButtonColor_Black = RGB(255, 0, 0);
	// 매도 호가 배경 색상
	CBCGPColor SellHogaBackColor = RGB(0x6c, 0x6c, 0x6c);
	// 매수 호가 배경 색상
	CBCGPColor BuyHogaBackColor_Black = RGB(0x6c, 0x6c, 0x6c);
	// 매도 호가 텍스트 색상
	CBCGPColor SellHogaTextColor_Black = RGB(0x64, 0xf9, 0xed);
	// 매수 호가 텍스트 색상
	CBCGPColor BuyHogaTextColor_Black = RGB(0xff, 0xa5, 0x00);
	// 종가 텍스트 색상
	CBCGPColor CloseTextColor_Black = RGB(255, 255, 255);
	CBCGPColor CloseTextColor = RGB(0, 0, 0);
	// 종가 배경 색상
	CBCGPColor CloseBackColor = RGB(0xf0, 0x70, 0x00);
	// 일반 텍스트 색상
	CBCGPColor TextColor_Black = RGB(255, 255, 255);
	CBCGPColor TextColor = RGB(0, 0, 0);
	// 행 그리드 색상
	CBCGPColor RowGridColor_Black = RGB(0x6c, 0x6c, 0x6c);
	CBCGPColor RowGridColor = RGB(0xc0, 0xc0, 0xc0);

	// 열 그리드 색상
	CBCGPColor ColGridColor_Black = RGB(0x6c, 0x6c, 0x6c);
	CBCGPColor ColGridColor = RGB(0xc0, 0xc0, 0xc0);
	// 시세 배경 색상
	CBCGPColor QuoteBackColor = RGB(255, 0, 0);
	// 시세 텍스트 색상
	CBCGPColor QuoteTextColor_Black = RGB(255, 255, 255);
	CBCGPColor QuoteTextColor = RGB(0, 0, 0);
	// 그리드 경계선 버튼 색상
	CBCGPColor GridBorderColor_Black = RGB(255, 255, 255);
	CBCGPColor GridBorderColor = RGB(0x6c, 0x6c, 0x6c);

	CBCGPColor OrderBuyPresentColor = RGB(139, 0, 0);
	CBCGPColor OrderSellPresentColor = RGB(0, 0, 128);
	CBCGPColor OrderPresentColor = RGB(101, 67, 33);
	CBCGPColor OrderHasBeenColor = RGB(0, 100, 0);

	CBCGPColor SelectedValueColor = RGB(255, 0, 0);

	CBCGPColor GridNormalColor = RGB(0xff, 0xff, 0xff);
	CBCGPColor GridNormalAlterColor = RGB(0x23, 0x23, 0x23);

	CBCGPColor GridNormalColor_Black = RGB(0x46, 0x46, 0x4b);
	CBCGPColor GridNormalAlterColor_Black = RGB(0x64, 0x64, 0x64);

	CBCGPColor SelectedColor_Black = RGB(0xf0, 0x70, 0x00);
	CBCGPColor SelectedColor = RGB(0xff, 0x00, 0x00);

	CBCGPColor QuoteHighColor = RGB(255, 0, 0);
	CBCGPColor QuoteLowColor = RGB(0, 0, 255);
	CBCGPColor QuoteCloseColor = RGB(0, 0, 0);
	CBCGPColor QuoteCloseBackColor = RGB(0xff, 0xff, 0x00);
	CBCGPColor QuotePreCloseColor = RGB(0, 0, 0);
	CBCGPColor QuoteOpenColor = RGB(0x6c, 0x6c, 0x6c);

	CBCGPColor BuyTextColor = RGB(0xfe, 0x2f, 0x19);
	CBCGPColor SellPositionBackColor = RGB(0, 0, 255);
	//CBCGPColor SellPositionBackColor = RGB(0x10, 0x84, 0xF9);
	//CBCGPColor BuyPositionBackColor = RGB(0xf0, 0x33, 0x3a);
	CBCGPColor BuyPositionBackColor = RGB(255, 0, 0);

	CBCGPColor SellOrderColor = RGB(0xda, 0xe2, 0xf5);
	CBCGPColor BuyOrderColor = RGB(0xfc, 0xe3, 0xe4);
	CBCGPColor SellButtonColor = RGB(0x13, 0x89, 0xf8);
	CBCGPColor BuyButtonColor = RGB(0xf0, 0x33, 0x3a);
	CBCGPColor SiseBackColor = RGB(0xf2, 0xf2, 0xf2);
	CBCGPColor SellSiseBackColor = RGB(0xda, 0xe2, 0xf5);
	CBCGPColor BuySiseBackColor = RGB(0xfc, 0xe2, 0xe4);
	CBCGPColor HiLoSiseBackColor = RGB(0xff, 0xff, 0xff);
	CBCGPColor SellHogaColor1 = RGB(0x9f, 0xd6, 0xff);
	CBCGPColor SellHogaColor2 = RGB(0xb9, 0xe4, 0xff);
	CBCGPColor SellHogaColor3 = RGB(0xcc, 0xe6, 0xfa);
	CBCGPColor SellHogaColor4 = RGB(0xdd, 0xf3, 0xff);
	CBCGPColor SellHogaColor5 = RGB(0xe6, 0xf7, 0xff);
	CBCGPColor BuyHogaColor1 = RGB(0xfd, 0xad, 0xb0);
	CBCGPColor BuyHogaColor2 = RGB(0xff, 0xc4, 0xc7);
	CBCGPColor BuyHogaColor3 = RGB(0xff, 0xd6, 0xd4);
	CBCGPColor BuyHogaColor4 = RGB(0xff, 0xe0, 0xe1);
	CBCGPColor BuyHogaColor5 = RGB(0xff, 0xe8, 0xe8);
	//CBCGPColor BuyHogaTextColor_Black = RGB(0xff, 0, 0);
	//CBCGPColor SellHogaTextColor_Black = RGB(0, 0, 0xff);
	CBCGPColor BuyHogaTextColor = RGB(0, 0, 0);
	CBCGPColor SellHogaTextColor = RGB(0, 0, 0);

	CBCGPColor PositionSellTextColor = RGB(0, 0, 0xff);
	CBCGPColor PositionBuyTextColor = RGB(0xff, 0, 0);
	CBCGPColor RemainSellTextColor = RGB(0, 0, 255);
	CBCGPColor RemainBuyTextColor = RGB(255, 0, 0);
	CBCGPColor SymbolProfitTextColor = RGB(255, 0, 0);
	CBCGPColor SymbolLossTextColor = RGB(0, 0, 255);
	CBCGPColor RemainBackColor = RGB(255, 255, 255);
	CBCGPColor DefaultBackColor = RGB(255, 255, 255);
	CBCGPColor DefaultTextColor = RGB(0, 0, 0);
	CBCGPColor NormalMarkColor = RGB(0xf2, 0xf2, 0xf2);

	CBCGPColor OrderExistColor = RGB(212, 186, 188);
	CBCGPColor PositionExistColor = RGB(255, 255, 255);
	CBCGPColor ExperienceExistColor = RGB(255, 255, 255);
	CBCGPColor CallDefaultColor = RGB(252, 226, 228);
	CBCGPColor PutDefaultColor = RGB(218, 226, 245);
	CBCGPColor SymbolPositionProfit = RGB(255, 0, 0);
	CBCGPColor SymbolPositionLoss = RGB(0, 0, 255);

	CBCGPColor OptionHeadTextColosr = RGB(255, 255, 255);
	CBCGPColor OptionHeadCallBackColor = RGB(240, 51, 58);
	CBCGPColor OptionHeadPutBackColor = RGB(19, 137, 255);
	CBCGPColor OptionHeadCenterBackColor = RGB(0, 0, 0);
	CBCGPColor OrderButtonTextColor = RGB(255, 255, 255);

	CBCGPBrush OptionHeadCallBackBrush = CBCGPBrush(OptionHeadCallBackColor);
	CBCGPBrush OptionHeadPutBackBrush = CBCGPBrush(OptionHeadPutBackColor);
	CBCGPBrush OptionHeadCenterBackBrush = CBCGPBrush(OptionHeadCenterBackColor);

	CBCGPBrush SymbolPositionProfitBrush = CBCGPBrush(SymbolPositionProfit);
	CBCGPBrush SymbolPositionLossBrush = CBCGPBrush(SymbolPositionLoss);

	CBCGPBrush OrderExistBrush = CBCGPBrush(OrderExistColor);
	CBCGPBrush PositionExistBrush = CBCGPBrush(PositionExistColor);
	CBCGPBrush ExperienceExistBrush = CBCGPBrush(ExperienceExistColor);
	CBCGPBrush CallDefaultBrush = CBCGPBrush(CallDefaultColor);
	CBCGPBrush PutDefaultBrush = CBCGPBrush(PutDefaultColor);

	CBCGPBrush NormalMarkBrush = CBCGPBrush(NormalMarkColor);
	CBCGPBrush PositionSellBrush = CBCGPBrush(PositionSellTextColor);
	CBCGPBrush PositionBuyBrush = CBCGPBrush(PositionBuyTextColor);
	CBCGPBrush RemainSellBrush = CBCGPBrush(RemainSellTextColor);
	CBCGPBrush RemainBuyBrush = CBCGPBrush(RemainBuyTextColor);
	CBCGPBrush SymbolProfitBrush = CBCGPBrush(SymbolProfitTextColor);
	CBCGPBrush SymbolLossBrush = CBCGPBrush(SymbolLossTextColor);
	CBCGPBrush RemainBackBrush = CBCGPBrush(RemainBackColor);

	CBCGPBrush DefaultBackBrush = CBCGPBrush(DefaultBackColor);
	CBCGPBrush DefaultTextBrush = CBCGPBrush(DefaultTextColor);


	CBCGPBrush SellOrderBrush = CBCGPBrush(SellOrderColor);
	CBCGPBrush BuyOrderBrush = CBCGPBrush(BuyOrderColor);
	CBCGPBrush SellButtonBrush = CBCGPBrush(SellButtonColor);
	CBCGPBrush BuyButtonBrush = CBCGPBrush(BuyButtonColor);
	CBCGPBrush SiseBackBrush = CBCGPBrush(SiseBackColor);
	CBCGPBrush SellSiseBackBrush = CBCGPBrush(SellSiseBackColor);
	CBCGPBrush BuySiseBackBrush = CBCGPBrush(BuySiseBackColor);
	CBCGPBrush HiLoSiseBackBrush = CBCGPBrush(HiLoSiseBackColor);
	CBCGPBrush SellHogaBrush1 = CBCGPBrush(SellHogaColor1);
	CBCGPBrush SellHogaBrush2 = CBCGPBrush(SellHogaColor2);
	CBCGPBrush SellHogaBrush3 = CBCGPBrush(SellHogaColor3);
	CBCGPBrush SellHogaBrush4 = CBCGPBrush(SellHogaColor4);
	CBCGPBrush SellHogaBrush5 = CBCGPBrush(SellHogaColor5);
	CBCGPBrush BuyHogaBrush1 = CBCGPBrush(BuyHogaColor1);
	CBCGPBrush BuyHogaBrush2 = CBCGPBrush(BuyHogaColor2);
	CBCGPBrush BuyHogaBrush3 = CBCGPBrush(BuyHogaColor3);
	CBCGPBrush BuyHogaBrush4 = CBCGPBrush(BuyHogaColor4);
	CBCGPBrush BuyHogaBrush5 = CBCGPBrush(BuyHogaColor5);


	CBCGPBrush OrderBuyPresentBrush = CBCGPBrush(OrderBuyPresentColor);
	CBCGPBrush OrderSellPresentBrush = CBCGPBrush(OrderSellPresentColor);
	CBCGPBrush OrderPresentBrush = CBCGPBrush(OrderPresentColor);
	CBCGPBrush OrderHasBennBrush = CBCGPBrush(OrderHasBeenColor);

	CBCGPBrush BuyPositionBrush = CBCGPBrush(BuyPositionBackColor);
	CBCGPBrush SellPositionBrush = CBCGPBrush(SellPositionBackColor);

	CBCGPBrush BuyTextBrush = CBCGPBrush(BuyTextColor);
	CBCGPBrush QuoteHighBrush = CBCGPBrush(QuoteHighColor);
	CBCGPBrush QuoteLowBrush = CBCGPBrush(QuoteLowColor);
	CBCGPBrush QuoteCloseBrush = CBCGPBrush(QuoteCloseColor);
	CBCGPBrush QuotePreCloseBrush = CBCGPBrush(QuotePreCloseColor);
	CBCGPBrush QuoteOpenBrush = CBCGPBrush(QuoteOpenColor);

	CBCGPBrush SelectedValueBrush = CBCGPBrush(SelectedValueColor);
	CBCGPBrush SelectedBrush = CBCGPBrush(SelectedColor);
	CBCGPBrush QuotCloseBackBrush = CBCGPBrush(QuoteCloseBackColor);

	CBCGPBrush GridNormalBrush = CBCGPBrush(GridNormalColor);
	CBCGPBrush GridNormalAlterBrush = CBCGPBrush(GridNormalAlterColor);

	CBCGPBrush QMNormalBrush = CBCGPBrush(GridNormalColor);
	CBCGPBrush QMBuyBrush = CBCGPBrush(BuyPositionBackColor, 0.2);
	CBCGPBrush QMSellBrush = CBCGPBrush(SellPositionBackColor, 0.2);
	CBCGPBrush QMHighLowBrush = CBCGPBrush(GridBorderColor, 0.1);


	// 헤더 버튼 색상
	CBCGPBrush HeaderButtonBrush = CBCGPBrush(HeaderButtonColor);
	// 헤더 배경 색상
	CBCGPBrush HeaderBackBrush = CBCGPBrush(HeaderBackColor);
	// 매도 버튼 색상
	CBCGPBrush SellButtonBrush_Black = CBCGPBrush(SellButtonColor);
	// 매수 버튼 색상
	CBCGPBrush BuyButtonBrush_Black = CBCGPBrush(BuyButtonColor);
	// 매도 호가 배경 색상
	CBCGPBrush SellHogaBackBrush = CBCGPBrush(SellHogaBackColor);
	// 매수 호가 배경 색상
	//CBCGPBrush BuyHogaBackBrush = CBCGPBrush(BuyHogaBackColor);
	// 매도 호가 텍스트 색상
	CBCGPBrush SellHogaTextBrush = CBCGPBrush(SellHogaTextColor);
	// 매수 호가 텍스트 색상
	CBCGPBrush BuyHogaTextBrush = CBCGPBrush(BuyHogaTextColor);
	// 종가 텍스트 색상
	CBCGPBrush CloseTextBrush = CBCGPBrush(CloseTextColor);
	// 종가 배경 색상
	CBCGPBrush CloseBackBrush = CBCGPBrush(CloseBackColor);
	// 일반 텍스트 색상
	CBCGPBrush TextBrush = CBCGPBrush(TextColor);
	// 행 그리드 색상
	CBCGPBrush RowGridBrush = CBCGPBrush(RowGridColor);
	// 열 그리드 색상
	CBCGPBrush ColGridBrush = CBCGPBrush(ColGridColor);
	// 시세 배경 색상
	CBCGPBrush QuoteBackBrush = CBCGPBrush(QuoteBackColor);
	// 시세 텍스트 색상
	CBCGPBrush QuoteTextBrush = CBCGPBrush(QuoteTextColor);
	// 그리드 경계선 브러쉬
	CBCGPBrush GridBorderBrush = CBCGPBrush(GridBorderColor);

	CBCGPTextFormat	TextFormat;
	CBCGPTextFormat	LeftTextFormat;
	CBCGPTextFormat	RightTextFormat;
	CBCGPTextFormat	QuoteTextFormat;
	CBCGPTextFormat	QuoteNumberFormat;
	CBCGPStrokeStyle OrderStroke;
};