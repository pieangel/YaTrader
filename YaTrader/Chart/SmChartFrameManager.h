#pragma once
#include <memory>
#include <map>
class SmChartFrame;
namespace DarkHorse {
	class SmChartFrameManager
	{
	public:
		// Static Members
		
		static int GetId() { return ++_Id; }

		SmChartFrameManager() {};
		~SmChartFrameManager();
		void AddChartFrame(std::shared_ptr<SmChartFrame> chart_frame);
		std::shared_ptr<SmChartFrame> FindChartFrame(const int& chart_id);
	private:
		static int _Id;
		std::map<int, std::shared_ptr<SmChartFrame>> _ChartMap;
	};
}

