#pragma once
#include <map>
#include <memory>

namespace DarkHorse {
	struct PnfItem;
	class SmChartData;
	class PnfMaker
	{
	public:
		explicit PnfMaker(const double& box_size = 500, const int& reverse = 5);
		~PnfMaker() = default;
		PnfMaker(const PnfMaker& other) = delete;
		PnfMaker& operator=(const PnfMaker& rhs) = delete;

		void OnNewData(const double& time, const double& high, const double& low, const double& close);
		void OnNewDataForChartData(const double& time, const double& high, const double& low, const double& close, std::shared_ptr<SmChartData> dst_data);
		double BoxSize() const { return _BoxSize; }
		//void BoxSize(double val) { _BoxSize = val; }
		int Reverse() const { return _Reverse; }
		//void Reverse(int val) { _Reverse = val; }
		std::map<double, std::shared_ptr<PnfItem>>& GetPnfMap()
		{
			return _PnfMap;
		}
	private:
		double _BaseValue = 0;
		int _Direction = 0;
		double _High = 0;
		double _Low = 0;
		const double _BoxSize = 0;
		const int _Reverse = 0;
		double _ChartTime = 0;
		std::map<double, std::shared_ptr<PnfItem>> _PnfMap;
	};
}

