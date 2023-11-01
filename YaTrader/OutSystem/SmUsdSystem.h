#pragma once
#include "SmOutSystem.h"
#include "../Util/VtTime.h"
#include <string>
#include <vector>
namespace DarkHorse {
	class SmUsdSystem : public SmOutSystem
	{
	private:
		/// <summary>
		/// ���� ���� �ð�
		/// </summary>
		VtTime start_time_begin_;
		/// <summary>
		/// ���� ���� �ð�
		/// </summary>
		VtTime start_time_end_;
		/// <summary>
		/// û�� �ð�
		/// </summary>
		VtTime end_time_;

		std::string strategy_type_;
		int order_limit_count_{ 0 };
		std::vector<double> buy_in_param_;
		std::vector<double> sell_in_param_;
		std::vector<double> buy_out_param_;
		std::vector<double> sell_out_param_;
	public:
		SmUsdSystem(std::string strategy_type);
		int order_limit_count() const { return order_limit_count_; }
		void order_limit_count(int val) { order_limit_count_ = val; }
		std::string strategy_type() const { return strategy_type_; }
		void strategy_type(std::string val) { strategy_type_ = val; }
		VtTime start_time_begin() const { return start_time_begin_; }
		void start_time_begin(VtTime val) { start_time_begin_ = val; }
		VtTime start_time_end() const { return start_time_end_; }
		void start_time_end(VtTime val) { start_time_end_ = val; }
		VtTime end_time() const { return end_time_; }
		void end_time(VtTime val) { end_time_ = val; }
		const std::vector<double>& buy_in_param() const { return buy_in_param_; }
		void buy_in_param(std::vector<double> val) { buy_in_param_ = val; }
		const std::vector<double>& sell_in_param() const { return sell_in_param_; }
		void sell_in_param(std::vector<double> val) { sell_in_param_ = val; }
		const std::vector<double>& buy_out_param() const { return buy_out_param_; }
		void buy_out_param(std::vector<double> val) { buy_out_param_ = val; }
		const std::vector<double>& sell_out_param() const { return sell_out_param_; }
		void sell_out_param(std::vector<double> val) { sell_out_param_ = val; }
	};
}

