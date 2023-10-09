#pragma once
#include <vector>
#include "../DataFrame/DataFrame.h"

namespace DarkHorse {
	class SmArrayMath
	{
	public:
		explicit SmArrayMath(std::vector<double>& srcData) : _Data(srcData) {};
		SmArrayMath& add(const std::vector<double>& another);
		SmArrayMath& add(const double& value);
		SmArrayMath& sub(const std::vector<double>& another);
		SmArrayMath& sub(const double& value);
		SmArrayMath& mul(const std::vector<double>& another);
		SmArrayMath& mul(const double& value);
		SmArrayMath& div(const std::vector<double>& another);
		SmArrayMath& div(const double& value);
		SmArrayMath& shift_left(const int offset, const double fill_value);
		SmArrayMath& shift_right(const int offset, const double fill_value);
		SmArrayMath& delta(const int offset);
		// 0인 값은 이전값으로 채운다.
		SmArrayMath& delta_backward(const int offset);
		SmArrayMath& delta_backward(const int start_index, const int end_index, const int offset, std::vector<double>& out);
		SmArrayMath& rate(const int offset);
		SmArrayMath& abs();
		SmArrayMath& acc();
		SmArrayMath& trim(const int start_index, const int len = -1);
		SmArrayMath& insert(const std::vector<double> another, const int insert_point = -1);
		SmArrayMath& insert2(const std::vector<double> another, const int len, const int insert_point = -1);
		SmArrayMath& replace(const double to_be_replaced, const double new_value);
		SmArrayMath& mov_avg(const int& period);
		SmArrayMath& mov_avg(const int& period, hmdf::StdDataFrame<hmdf::DateTime>& df);
		SmArrayMath& mov_avg(const int& period, const std::string& col_name, hmdf::StdDataFrame<hmdf::DateTime>& df);
		SmArrayMath& mov_avg(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& mov_avg(const int start_index, const int period);
		SmArrayMath& exp_avg(const int period);
		SmArrayMath& exp_avg(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& kama_avg(const int period);
		SmArrayMath& kama_avg(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& t3_avg(const int period, const double vf = 0.618);
		SmArrayMath& t3_avg(const int start_index, const int end_index, const int period, const double vf, std::vector<double>& out);
		SmArrayMath& tema_avg(const int period);
		SmArrayMath& tema_avg(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& mama_avg(const int period, const double SlowLimit = 0.05);
		SmArrayMath& mama_avg(const int start_index, const int end_index, const int period, const double SlowLimit, std::vector<double>& mama, std::vector<double>& fmama);
		SmArrayMath& trima_avg(const int period);
		SmArrayMath& trima_avg(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& wma_avg(const int period);
		SmArrayMath& wma_avg(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& sma_avg(const int period);
		SmArrayMath& sma_avg(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& mov_med(const int period);
		SmArrayMath& mov_corr(const int period, const std::vector<double>& another);
		SmArrayMath& mov_corr(const int start_index, const int end_index, const int period, const std::vector<double>& another, std::vector<double>& out);
		SmArrayMath& mov_min(const int period);
		SmArrayMath& mov_min(const int period, hmdf::StdDataFrame<hmdf::DateTime>& data_frame, const std::string& col_name);
		SmArrayMath& mov_min(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& mov_max(const int period);
		SmArrayMath& mov_max(const int period, hmdf::StdDataFrame<hmdf::DateTime>& data_frame, const std::string& col_name);
		SmArrayMath& mov_max(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& mov_std(const int period, const double deviation);
		SmArrayMath& mov_std(const int start_index, const int end_index, const int period, const double deviation, std::vector<double>& out);
		SmArrayMath& mov_sum(const int period);
		SmArrayMath& mov_sum(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& tsf(const int period);
		SmArrayMath& tsf(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& ht_sine();
		SmArrayMath& ht_sine(const int start_index, const int end_index, std::vector<double>& sine, std::vector<double>& lead_sine);
		SmArrayMath& ht_trend_line();
		SmArrayMath& ht_trend_line(const int start_index, const int end_index, std::vector<double>& out);
		SmArrayMath& ht_trendmode();
		SmArrayMath& ht_trendmode(const int start_index, const int end_index, std::vector<int>& out);
		SmArrayMath& ht_phasor();
		SmArrayMath& ht_phasor(const int start_index, const int end_index, std::vector<double>& phase, std::vector<double>& quadrature);
		SmArrayMath& ht_dcphase();
		SmArrayMath& ht_dcphase(const int start_index, const int end_index, std::vector<double>& out);
		SmArrayMath& ht_dcperiod();
		SmArrayMath& ht_dcperiod(const int start_index, const int end_index, std::vector<double>& out);

		SmArrayMath& ht_linear_reg_angle(const int period);
		SmArrayMath& ht_linear_reg_angle(const int start_index, const int end_index, const int period, std::vector<double>& out);

		SmArrayMath& atr(const int period);
		SmArrayMath& atr(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& avg_price(
			const std::vector<double>& open,
			const std::vector<double>& high,
			const std::vector<double>& low,
			const std::vector<double>& close);
		SmArrayMath& avg_price(const int start_index, const int end_index,
			const std::vector<double>& open,
			const std::vector<double>& high,
			const std::vector<double>& low,
			const std::vector<double>& close, std::vector<double>& out);
		SmArrayMath& med_price(const std::vector<double>& high, const std::vector<double>& low);
		SmArrayMath& med_price(const int start_index, const int end_index, const std::vector<double>& high, const std::vector<double>& low, std::vector<double>& out);
		SmArrayMath& max_index(const int period);
		SmArrayMath& max_index(const int start_index, const int end_index, const int period, std::vector<int>& out);
		SmArrayMath& min_index(const int period);
		SmArrayMath& min_index(const int start_index, const int end_index, const int period, std::vector<int>& out);
		static int max_index(const std::vector<double> data, const int start, const int end);
		static int min_index(const std::vector<double> data, const int start, const int end);
		static int min_delta_index(const std::vector<double> data, const double source);
		SmArrayMath& lowess1(const double& fraction, const int& step, hmdf::StdDataFrame<hmdf::DateTime>& data_frame);
		SmArrayMath& lowess1(const double& fraction, const int& step, hmdf::StdDataFrame<hmdf::DateTime>& data_frame, const std::string& col_name);
		SmArrayMath& lowess2(const double& fraction, const int& step, hmdf::StdDataFrame<hmdf::DateTime>& data_frame);
		SmArrayMath& lowess2(const double& fraction, const int& step, hmdf::StdDataFrame<hmdf::DateTime>& data_frame, const std::string& col_name);
		SmArrayMath& curve_fit(const int type, const int poly_num);
		SmArrayMath& curve_fit(const int start_index, const int end_index, const int type, const int poly_num);
		SmArrayMath& curve_fit(const int type, const int poly_num, std::vector<double>& out);
		SmArrayMath& curve_fit(const int start_index, const int end_index, const int type, const int poly_num, std::vector<double>& out);
		SmArrayMath& lin_reg(const double accuracy, const int half_len);
		SmArrayMath& lin_reg(const int period);
		SmArrayMath& lin_reg(const int start_index, const int end_index, const int period, std::vector<double>& out);
		SmArrayMath& trend_line(const int start_index, const int end_index, std::vector<double>& out);
		//SmArrayMath& macd(const int fast_period, const int slow_period, const int signal_period);
		//SmArrayMath& macd(const int start_index, const int end_index, const int fast_period, const int slow_period, const int signal_period, std::vector<double>& macd, std::vector<double>& signal, std::vector<double>& hist);
		SmArrayMath& heiken_ashi_smooth(const int& period, hmdf::StdDataFrame<hmdf::DateTime>& data_frame);
		SmArrayMath& heiken_ashi(hmdf::StdDataFrame<hmdf::DateTime>& data_frame);
		SmArrayMath& heiken_ashi_smooth(const int& period, hmdf::StdDataFrame<hmdf::DateTime>& data_frame, const std::string& prefix);
		SmArrayMath& heiken_ashi(hmdf::StdDataFrame<hmdf::DateTime>& data_frame, const std::string& prefix);
		std::vector<double>& result() { return _Data; }
	private:
		std::vector<double>& _Data;
	};
}

