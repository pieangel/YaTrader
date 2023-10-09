AS IS

LeftWindow
SmAccountArea -> AccountProfitLossControl
SmAcceptedGrid -> AccountOrderControl
SmPositionGrid -> AccountPositionControl
SmFavoriteGrid -> FavoriteSymbolControl
// domestic
FutureGrid     ->FutureCloseControl
			   ->FuturePositionControl
               ->FutureExpectedControl

OptionGrid     ->OptionCloseControl
               ->OptionPositionControl
               ->OptionExpectedControl			   


CenterWindow
SmPositionArea -> SymbolPositionControl
SmQuoteArea -> SymbolTickControl
SmOrderArea -> TotalHogaInfoControl
            -> SymbolProfitLossControl
			-> SellStopOrderControl
			-> SellHogaControl
			-> SellOrderControl
			-> PriceRangeControl
			-> OrderableInfoControl
			-> BuyStopOrderControl
			-> BuyOrderControl
			-> BuyHogaControl
			

RightWindow
SmAssetArea -> AssetControl
SmSymbolArea -> SymbolInfoControl

TO-BE

LeftWindow
AccountProfitLossView -> AccountProfitLossControl
AccountOrderView -> AccountOrderControl
AccountPositionView -> AccountPositionControl
FavoriteSymbolView -> FavoriteSymbolControl
// domestic
FutureView     ->FutureCloseControl
			   ->FuturePositionControl
               ->FutureExpectedControl

OptionView     ->OptionCloseControl
               ->OptionPositionControl
               ->OptionExpectedControl			   


CenterWindow
SymbolPositionView -> SymbolPositionControl
SymbolTickView -> SymbolTickControl
OrderView   -> TotalHogaInfoControl
            -> SymbolProfitLossControl
			-> SellStopOrderControl
			-> SellHogaControl
			-> SellOrderControl
			-> PriceRangeControl
			-> OrderableInfoControl
			-> BuyStopOrderControl
			-> BuyOrderControl
			-> BuyHogaControl
			

RightWindow
AssetView -> AssetControl
SymbolInfoView -> SymbolInfoControl