#pragma once
#include "SmOutSystem.h"
namespace DarkHorse {
	class SmUsdSystem : public SmOutSystem
	{
	private:
		std::string start_time_begin_;
		std::string start_time_end_;
		std::string end_time_;
		int order_limit_count_{ 0 };
		int order_limit_count() const { return order_limit_count_; }
		void order_limit_count(int val) { order_limit_count_ = val; }
	public:
		std::string start_time_begin() const { return start_time_begin_; }
		void start_time_begin(std::string val) { start_time_begin_ = val; }
		std::string start_time_end() const { return start_time_end_; }
		void start_time_end(std::string val) { start_time_end_ = val; }
		std::string end_time() const { return end_time_; }
		void end_time(std::string val) { end_time_ = val; }
	};
}

