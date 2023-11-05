#pragma once
#include "SmOutSystem.h"
#include "../Util/VtTime.h"
#include <string>
#include <vector>
#include "SmUsdStrategy.h"
namespace DarkHorse {
	class SmUsdSystem : public SmOutSystem
	{
	private:
		bool was_liq{ false };
		int entrance_count_{ 0 };
		/// <summary>
		/// 진입 시작 시간
		/// </summary>
		VtTime start_time_begin_;
		/// <summary>
		/// 진입 종료 시간
		/// </summary>
		VtTime start_time_end_;
		/// <summary>
		/// 청산 시간
		/// </summary>
		VtTime end_time_;

		std::string strategy_type_;
		int order_limit_count_{ 0 };
		SmUsdStrategy strategy_;
		bool check_condition(const SysArg& arg);
		void check_group_condition(const GroupArg& group_arg, std::vector<bool>& arg_cond);
		bool check_entrance(const int index);
	public:
		bool CheckEntranceBar();
		bool CheckEntranceForBuy();
		bool CheckEntranceForSell();
		bool CheckLiqForSell();
		bool CheckLiqForBuy();
		bool CheckOrderLimit();
		bool CheckEnterableByTime();
		bool CheckLigByTime();
		void on_timer();

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
		DarkHorse::SmUsdStrategy strategy() const { return strategy_; }
		void strategy(DarkHorse::SmUsdStrategy val) { strategy_ = val; }
	};
}

