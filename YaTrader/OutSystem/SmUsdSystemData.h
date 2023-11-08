#pragma once
#include <string>
#include <map>
#include "../Log/MyLogger.h"

namespace DarkHorse {
	class SmUsdSystemData
	{
	public:
		SmUsdSystemData() {
			data_map["kbs"] = 0;
			data_map["kas"] = 0;
			data_map["kbc"] = 0;
			data_map["kac"] = 0;
			data_map["qbs"] = 0;
			data_map["qas"] = 0;
			data_map["qbc"] = 0;
			data_map["qac"] = 0;
			data_map["ubs"] = 0;
			data_map["uas"] = 0;
			data_map["ubc"] = 0;
			data_map["uac"] = 0;
		}
		int get_data(std::string name) {
			auto it = data_map.find(name);
			if (it != data_map.end()) {
				return it->second;
			}
			return -1;
		}
		void update_data(const std::string& symbol_code, int bs, int as, int bc, int ac) {
			
			//LOGINFO(CMyLogger::getInstance(), "update_data symbol_code[%s], bs[%d], as[%d], bc[%d], ac[%d]", symbol_code.c_str(), bs, as, bc, ac);
			std::string prefix = symbol_code.substr(0, 3);
			if (prefix == "101") {
				//LOGINFO(CMyLogger::getInstance(), "update_data symbol_code[%s], bs[%d], as[%d], bc[%d], ac[%d]", symbol_code.c_str(), bs, as, bc, ac);
				data_map["kbs"] = bs;
				data_map["kas"] = as;
				data_map["kbc"] = bc;
				data_map["kac"] = ac;
			}
			else if (prefix == "106") {
				//LOGINFO(CMyLogger::getInstance(), "update_data symbol_code[%s], bs[%d], as[%d], bc[%d], ac[%d]", symbol_code.c_str(), bs, as, bc, ac);
				data_map["qbs"] = bs;
				data_map["qas"] = as;
				data_map["qbc"] = bc;
				data_map["qac"] = ac;
			}
			else if (prefix == "175") {
				//LOGINFO(CMyLogger::getInstance(), "update_data symbol_code[%s], bs[%d], as[%d], bc[%d], ac[%d]", symbol_code.c_str(), bs, as, bc, ac);
				data_map["ubs"] = bs;
				data_map["uas"] = as;
				data_map["ubc"] = bc;
				data_map["uac"] = ac;
			}
		}
		/*
		// kospi 200 total buy hoga quantity.
		int kbs = 0;
		// kospi 200 total sell hoga quantity.
		int kas = 0;
		// kospi 200 total buy hoga count.
		int kbc = 0;
		// kospi 200 total sell hoga count.
		int kac = 0;
		// kodaq 150 total buy hoga quantity.
		int qbs = 0;
		// kosdaq 150 total sell hoga quantity.
		int qas = 0;
		// kosdaq 150 total buy hoga count.
		int qbc = 0;
		// kosdaq 150 total sell hoga count.
		int qac = 0;
		// dollar total buy hoga quantity.
		int ubs = 0;
		// dollar total sell hoga quantity.
		int uas = 0;
		// dollar total buy hoga count.
		int ubc = 0;
		// dollar total sell hoga count.
		int uac = 0;
		*/
	private:
		// key : data name, value : data value
		std::map<std::string, int> data_map;
	};

}
