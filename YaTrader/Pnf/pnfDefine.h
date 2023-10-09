#pragma once
#include <map>
namespace DarkHorse {
	struct PnfItem {
		double box_size;
		double time;
		double close_value;
		double open_value;
		double high_value;
		double low_value;
		int count;
		int acc_count;
		int direction;
		bool start_point = false;
		int reverse;
	};
}
