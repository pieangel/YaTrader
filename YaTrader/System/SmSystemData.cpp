#include "stdafx.h"
#include "SmSystemData.h"
#include <algorithm>
#include "../Chart/SmChartData.h"
#include "SmArrayMath.h"
#include "SmSystemFactor.h"
#include "SmSystem.h"
#include "../Chart/SmChartDataManager.h"
#include "../Global/SmTotalManager.h"
#include <chartdir.h>

using namespace DarkHorse;
void DarkHorse::SmSystemData::SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data)
{
	if (!chart_data) return;
	const auto& df = chart_data->GetDataFrame();
	auto& index_v = chart_data->GetDataFrame().get_index();
	std::vector<hmdf::DateTime> new_index;
	copy(index_v.begin(), index_v.end(), back_inserter(new_index));

	std::string prefix = chart_data->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

 	size_t count = _DataFrame.load_data(std::move(new_index),
 		std::make_pair(col_dt.c_str(), df.get_column<double>(col_dt.c_str())),
 		std::make_pair(col_open.c_str(), df.get_column<double>(col_open.c_str())),
 		std::make_pair(col_high.c_str(), df.get_column<double>(col_high.c_str())),
		std::make_pair(col_low.c_str(), df.get_column<double>(col_low.c_str())),
 		std::make_pair(col_close.c_str(), df.get_column<double>(col_close.c_str())),
		std::make_pair(col_volume.c_str(), df.get_column<double>(col_volume.c_str())));

	_ChartDataMap[chart_data->GetChartDataKey()] = chart_data;
	_MainChartData = chart_data;
}

void SmSystemData::JoinAllChartData(const std::string& symbol_code)
{
	SmChartType chart_type1 = SmChartType::MIN;
	SmChartType chart_type2 = SmChartType::MIN;
	SmChartType chart_type3 = SmChartType::MIN;
	SmChartType chart_type4 = SmChartType::MIN;
	SmChartType chart_type5 = SmChartType::TICK;
	SmChartType chart_type6 = SmChartType::TICK;
	SmChartType chart_type7 = SmChartType::DAY;
	int cycle1 = 1;
	int cycle2 = 5;
	int cycle3 = 60;
	int cycle4 = 240;
	int cycle5 = 120;
	int cycle6 = 300;
	int cycle7 = 1;


	std::string data_key1 = SmChartDataManager::MakeDataKey(symbol_code, chart_type1, cycle1);
	std::shared_ptr<SmChartData> chart_data1 = mainApp.ChartDataMgr()->FindChartData(data_key1);

	std::string data_key2 = SmChartDataManager::MakeDataKey(symbol_code, chart_type2, cycle2);
	std::shared_ptr<SmChartData> chart_data2 = mainApp.ChartDataMgr()->FindChartData(data_key2);

	std::string data_key3 = SmChartDataManager::MakeDataKey(symbol_code, chart_type3, cycle3);
	std::shared_ptr<SmChartData> chart_data3 = mainApp.ChartDataMgr()->FindChartData(data_key3);

	std::string data_key4 = SmChartDataManager::MakeDataKey(symbol_code, chart_type4, cycle4);
	std::shared_ptr<SmChartData> chart_data4 = mainApp.ChartDataMgr()->FindChartData(data_key4);

	std::string data_key5 = SmChartDataManager::MakeDataKey(symbol_code, chart_type5, cycle5);
	std::shared_ptr<SmChartData> chart_data5 = mainApp.ChartDataMgr()->FindChartData(data_key5);

	std::string data_key6 = SmChartDataManager::MakeDataKey(symbol_code, chart_type6, cycle6);
	std::shared_ptr<SmChartData> chart_data6 = mainApp.ChartDataMgr()->FindChartData(data_key6);

	std::string data_key7 = SmChartDataManager::MakeDataKey(symbol_code, chart_type7, cycle7);
	std::shared_ptr<SmChartData> chart_data7 = mainApp.ChartDataMgr()->FindChartData(data_key7);

	hmdf::StdDataFrame<hmdf::DateTime> join_df1, join_df2, join_df3, join_df4, join_df5, join_df6, join_df7;

	join_df1 = chart_data2->GetDataFrame().join_by_index<decltype(join_df1), double>(chart_data1->GetDataFrame(), hmdf::join_policy::left_right_join);

	join_df2 = join_df1.join_by_index<decltype(join_df2), double>(chart_data3->GetDataFrame(), hmdf::join_policy::left_right_join);

	join_df3 = join_df2.join_by_index<decltype(join_df3), double>(chart_data4->GetDataFrame(), hmdf::join_policy::left_right_join);

	join_df4 = join_df3.join_by_index<decltype(join_df4), double>(chart_data5->GetDataFrame(), hmdf::join_policy::left_right_join);

	join_df5 = join_df4.join_by_index<decltype(join_df5), double>(chart_data6->GetDataFrame(), hmdf::join_policy::left_right_join);
	join_df6 = join_df5.join_by_index<decltype(join_df6), double>(chart_data7->GetDataFrame(), hmdf::join_policy::left_right_join);

	

	std::vector<hmdf::DateTime>& joined_index = join_df6.get_index();
	std::vector<double> cv_dt(joined_index.size());
	for (size_t i = 0; i < joined_index.size(); i++) {
		//if (isnan(kp[i])) kp[i] = Chart::NoValue;
		//if (isnan(nq[i])) nq[i] = Chart::NoValue;
		//if (isnan(nk[i])) nk[i] = Chart::NoValue;
		//if (isnan(hs[i])) hs[i] = Chart::NoValue;
		//double dt = Chart::chartTime(joined_index[i].year(), static_cast<int>(joined_index[i].month()), joined_index[i].dmonth(), joined_index[i].hour(), joined_index[i].minute(), joined_index[i].sec());
		//cv_dt[i] = dt;
		auto    result = join_df6.get_columns_info<double>();
		for (auto citer : result) {
			auto& col_name = std::get<0>(citer);
			auto& col = join_df6.get_column<double>(col_name.c_str());
			if (isnan(col[i])) col[i] = Chart::NoValue;
		}
	}
}

void DarkHorse::SmSystemData::ProcessData()
{
	if (!_System.lock()) return;
	if (!_MainChartData) return;

	std::string prefix = _MainChartData->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	const SmSystemFactor& fact = _System.lock()->Factor();
	auto& open = _DataFrame.get_column<double>(col_open.c_str());
	auto& high = _DataFrame.get_column<double>(col_high.c_str());
	auto& low = _DataFrame.get_column<double>(col_low.c_str());
	auto& close  = _DataFrame.get_column<double>(col_close.c_str());
	std::vector<double> high_channel_upper(high.begin(), high.end());
	std::vector<double> high_channel_lower(high.begin(), high.end());

	std::vector<double> low_channel_upper(low.begin(), low.end());
	std::vector<double> low_channel_lower(low.begin(), low.end());


	SmArrayMath(high_channel_upper).mov_max(fact.ChannelCycle);
	SmArrayMath(high_channel_lower).mov_max(fact.ChannelCycle).sub(fact.ChannelBandHeight);
	SmArrayMath(low_channel_upper).mov_min(fact.ChannelCycle);
	SmArrayMath(low_channel_lower).mov_min(fact.ChannelCycle).add(fact.ChannelBandHeight);
	std::vector<double> mid_channel(high_channel_upper.begin(), high_channel_upper.end());
	SmArrayMath(mid_channel).add(low_channel_lower).mul(0.5);

	std::string col_hcu(prefix), col_hcl(prefix), col_lcu(prefix), col_lcl(prefix), col_mc(prefix), col_mp(prefix);
	col_hcu.append("high_channel_upper");
	col_hcl.append("high_channel_lower");
	col_lcu.append("low_channel_upper");
	col_lcl.append("low_channel_lower");
	col_mc.append("mid_channel");
	col_mp.append("med_price");

	std::string col_hl(prefix), col_ll(prefix), col_ml(prefix);
	col_hl.append("high_lowess");
	col_ll.append("low_lowess");
	col_ml.append("med_lowess");

	_DataFrame.load_column(col_hcu.c_str(), std::move(high_channel_upper));
	_DataFrame.load_column(col_hcl.c_str(), std::move(high_channel_lower));
	_DataFrame.load_column(col_lcu.c_str(), std::move(low_channel_upper));
	_DataFrame.load_column(col_lcl.c_str(), std::move(low_channel_lower));
	_DataFrame.load_column(col_mc.c_str(), std::move(mid_channel));


	SmArrayMath(high).lowess1(fact.LowessSmoothness, fact.LowessIteration, _DataFrame, col_hl.c_str());

	SmArrayMath(low).lowess1(fact.LowessSmoothness, fact.LowessIteration, _DataFrame, col_ll.c_str());

	std::vector<double> med_price(close.size());
	SmArrayMath(med_price).med_price(high, low);
	SmArrayMath(med_price).lowess1(fact.MidLowessSmoothness, fact.MidLowessIteration, _DataFrame, col_ml.c_str());
	_DataFrame.load_column("", std::move(med_price));

	std::string col_ma2(prefix), col_ma3(prefix), col_ma4(prefix);
	std::string col_ma5(prefix), col_ma6(prefix), col_ma7(prefix);
	std::string col_ma10(prefix), col_ma20(prefix), col_ma30(prefix);
	std::string col_ma40(prefix), col_ma50(prefix), col_ma60(prefix);
	std::string col_ma90(prefix), col_ma120(prefix), col_ma240(prefix);

	col_ma2.append("ma2");
	col_ma3.append("ma3");
	col_ma4.append("ma4");
	col_ma5.append("ma5");
	col_ma6.append("ma6");
	col_ma7.append("ma7");
	col_ma10.append("ma10");
	col_ma20.append("ma20");
	col_ma30.append("ma30");
	col_ma40.append("ma40");
	col_ma50.append("ma50");
	col_ma60.append("ma60");
	col_ma90.append("ma90");
	col_ma120.append("ma120");
	col_ma240.append("ma240");

	SmArrayMath(close).mov_avg(2, col_ma2.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(3, col_ma3.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(4, col_ma4.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(5, col_ma5.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(6, col_ma6.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(7, col_ma7.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(10, col_ma10.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(20, col_ma20.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(30, col_ma30.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(40, col_ma40.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(50, col_ma50.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(60, col_ma60.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(90, col_ma90.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(120, col_ma120.c_str(), _DataFrame);
	SmArrayMath(close).mov_avg(240, col_ma240.c_str(), _DataFrame);
}
