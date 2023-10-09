#pragma once
#include <string>
#include <memory>
#include "SmSystemFactor.h"
#include "SmSystemData.h"

namespace DarkHorse {
	class SmChartData;
	class SmSystem : public std::enable_shared_from_this<SmSystem>
	{
	public:
		SmSystem(const int& id, const std::string& system_name);
		DarkHorse::SmSystemFactor Factor() const { return _Factor; }
		void Factor(DarkHorse::SmSystemFactor val) { _Factor = val; }
		std::shared_ptr<DarkHorse::SmSystemData> Data() const { return _Data; }
		void Data(std::shared_ptr<DarkHorse::SmSystemData> val) { _Data = val; }
		void BindSystemData();
		std::string Name() const { return _Name; }
		void Name(std::string val) { _Name = val; }
		void ProcessData(std::shared_ptr<DarkHorse::SmChartData> chart_data);
		bool DataProcessed() const { return _DataProcessed; }
		void DataProcessed(bool val) { _DataProcessed = val; }
	private:
		/// <summary>
		/// System Name
		/// </summary>
		std::string _Name;
		/// <summary>
		/// System Configuration Settings.
		/// </summary>
		DarkHorse::SmSystemFactor _Factor;
		/// <summary>
		/// System Data
		/// </summary>
		std::shared_ptr<DarkHorse::SmSystemData> _Data = nullptr;
		/// <summary>
		/// Symstem ID.
		/// </summary>
		int _Id = 0;
		bool _DataProcessed = false;
	};
}

