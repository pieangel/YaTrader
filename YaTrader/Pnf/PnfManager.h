#pragma once
#include "pnfDefine.h"

#include <map>
#include <memory>
#include <string>
namespace DarkHorse {
	class SmChartData;
	class PnfMaker;
	class PnfManager
	{
	public:
		PnfManager() {};
		~PnfManager() {};
		void MakePnf(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& box_size, const int& reverse);
		void MakePnfByEdge(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& box_size, const int& reverse, std::map<double, std::shared_ptr<PnfItem>>& pnf_map);
		void MakePnfBar(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& box_size, const int& reverse, std::map<double, std::shared_ptr<PnfItem>>& pnf_map);
		void MakePnfBar(std::shared_ptr<DarkHorse::SmChartData> chart_data, const double& box_size = 500.0, const int& reverse = 5);

		void MakePnfBar(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& box_size, const int& reverse, std::map<double, std::shared_ptr<PnfItem>>& pnf_map, std::shared_ptr<DarkHorse::SmChartData> dst_data);
		void MakePnfBarByEdge(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& box_size, const int& reverse, std::map<double, std::shared_ptr<PnfItem>>& pnf_map, std::shared_ptr<DarkHorse::SmChartData> dst_data);
		void MakePnfBar(std::shared_ptr<DarkHorse::SmChartData> chart_data, std::shared_ptr<DarkHorse::SmChartData> dst_data, const double& box_size = 500.0, const int& reverse = 5);
		const std::map<double, PnfItem>& GetPnfMap() const {
			return _PnfMap;
		}
		std::shared_ptr<PnfMaker> FindPnfMaker(const std::string data_key);
	private:
		std::map<double, PnfItem> _PnfMap;
		std::map<std::string, std::shared_ptr<PnfMaker>> _PnfMakerMap;
	};
}

