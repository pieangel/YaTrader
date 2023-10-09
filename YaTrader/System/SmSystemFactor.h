#pragma once
namespace DarkHorse {
	class SmSystemFactor
	{
	public:
		bool UseHa2 = false;
		// 0 : normal, 1 : ha1 , 2 : ha2
		int MainDataType = 0;
		bool RealOrder = false;
		bool RealSignal = false;
		bool ShowWave = true;
		bool ShowSignal = true;
		bool ShowOrder = true;
		bool ShowChannel = true;
		bool ShowShortLowess = true;
		double ShortLowess = 0.08;
		double ShortMidLowess = 0.1;
		int ShortLowessIteration = 0;
		int ShortLowessSpan = 100;

		int ChannelCycle = 15;
		double ChannelBandHeight = 5;
		bool ShowLowess = true;
		double LowessSmoothness = 0.005;
		int LowessIteration = 0;
		double LowessBandHeight = 150;
		bool ShowHaLowess = false;
		double HaLowessSmoothness = 0.005;
		int HaLowessIteration = 0;
		bool ShowMidLowess = true;
		double MidLowessSmoothness = 0.005;
		int MidLowessIteration = 0;
		/// <summary>
		///  0 : normal, 1 : ha1, 2 : ha2
		/// </summary>
		int SignalLowessType = 0;

		int IndicatorType1 = 0;
		int IndicatorHeight1 = 20;
		int IndicatorType2 = 1;
		int IndicatorHeight2 = 20;

		int SimulTimeInterval = 200;

		bool ShowMa = false;
		bool ShowMa2 = false;
		bool ShowMa3 = false;
		bool ShowMa4 = false;
		bool ShowMa5 = false;
		bool ShowMa6 = false;
		bool ShowMa7 = false;
		bool ShowMa10 = false;
		bool ShowMa20 = false;
		bool ShowMa30 = false;
		bool ShowMa40 = false;
		bool ShowMa50 = false;
		bool ShowMa60 = false;
		bool ShowMa90 = false;
		bool ShowMa120 = false;
		bool ShowMa240 = false;

		/// <summary>
		/// 파동이 바뀌었을 때 주문 진입 값
		/// </summary>
		int EntryThresHold = 500;
		/// <summary>
		/// 파동이 바뀌는 기준값
		/// </summary>
		int WaveThresHold = 500;

		int ShortWaveThresHold = 500;
		int MiddleWaveThresHold = 1000;
		int LongWaveThresHold = 2000;

		int LossLimitTick = 60;
		int TrailingStopTick = 80;
		int LeastProfitHeight = 2000;
		int TrailingStartMinTick = 200;

		int StartHour = 8;
		int StartMin = 0;
		int EndHour = 6;
		int EndMin = 0;

		/// <summary>
		/// 전략번호
		/// </summary>
		int Strategy = 0;
		int LeastChannelHeight = 2500;

		int BoxSize = 500;
		int ReverseSize = 2;
		int BoxCut = 5;
	};
}

