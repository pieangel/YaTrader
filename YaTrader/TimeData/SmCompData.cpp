#include "stdafx.h"
#include "SmCompData.h"
#include "../Util/SmUtil.h"
#include "../Chart/SmChartData.h"
#include "../Chart/SmChartDataManager.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include <chrono>
#include <chartdir.h>
#include <algorithm>
#include "../TaLib/tapp.h"

using namespace std::chrono;

DarkHorse::SmCompData::SmCompData()
{
	_Id = SmChartDataManager::GetId();
	_TimerId = 0;
}

DarkHorse::SmCompData::~SmCompData()
{
	_Timer.remove(_TimerId);
}

bool DarkHorse::SmCompData::RequestChartData(const int& wnd_id)
{
	if (_KospiSymbolCode.length() > 0) {
		auto chart_data = mainApp.ChartDataMgr()->FindAddChartData(_KospiSymbolCode, _ChartType, _Cycle);
		if (!chart_data || !chart_data->Received()) {
			mainApp.ChartDataMgr()->RequestChartData(_KospiSymbolCode, _ChartType, _Cycle, 1500, wnd_id, 0);
			mainApp.SymMgr()->RegisterSymbolToServer(_KospiSymbolCode, true);
			return true;
		}
	}
	if (_NqSymbolCode.length() > 0) {
		auto chart_data = mainApp.ChartDataMgr()->FindAddChartData(_NqSymbolCode, _ChartType, _Cycle);
		if (!chart_data || !chart_data->Received()) {
			mainApp.ChartDataMgr()->RequestChartData(_NqSymbolCode, _ChartType, _Cycle, 2000, wnd_id, 0);
			mainApp.SymMgr()->RegisterSymbolToServer(_NqSymbolCode, true);
			return true;
		}
	}

	if (_NikkeiCode.length() > 0) {
		auto chart_data = mainApp.ChartDataMgr()->FindAddChartData(_NikkeiCode, _ChartType, _Cycle);
		if (!chart_data || !chart_data->Received()) {
			mainApp.ChartDataMgr()->RequestChartData(_NikkeiCode, _ChartType, _Cycle, 2000, wnd_id, 0);
			mainApp.SymMgr()->RegisterSymbolToServer(_NikkeiCode, true);
			return true;
		}
	}

	if (_HangSengCode.length() > 0) {
		auto chart_data = mainApp.ChartDataMgr()->FindAddChartData(_HangSengCode, _ChartType, _Cycle);
		if (!chart_data || !chart_data->Received()) {
			mainApp.ChartDataMgr()->RequestChartData(_HangSengCode, _ChartType, _Cycle, 2000, wnd_id, 0);
			mainApp.SymMgr()->RegisterSymbolToServer(_HangSengCode, true);
			return true;
		}
	}


	return false;
}

void DarkHorse::SmCompData::OnChartEvent(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& action)
{

}
// 1. 나스닥데이터에서 시간을 추출한다. 
// 2. 위에서 추출한 시간 데이터 개수와 일치하게 코스피, 니케이, 항생 개수를 조절한다. 
// 3. 코스피, 나스닥, 니케이, 항생 데이터를 위에서 추출한 시간 데이터를 참조하여 다시 뽑는다. 이때 그 시간에 없는 데이터는 값없음 처리된다. 
void DarkHorse::SmCompData::MakeChartData()
{
	auto _KospiChartData = mainApp.ChartDataMgr()->FindChartData(_KospiSymbolCode, _ChartType, _Cycle);
	auto _NqChartData = mainApp.ChartDataMgr()->FindChartData(_NqSymbolCode, _ChartType, _Cycle);
	auto _NkChartData = mainApp.ChartDataMgr()->FindChartData(_NikkeiCode, _ChartType, _Cycle);
	auto _HsChartData = mainApp.ChartDataMgr()->FindChartData(_HangSengCode, _ChartType, _Cycle);

	if (!_KospiChartData || !_NqChartData || !_NkChartData || !_HsChartData) return;

	_KpPrefix = _KospiChartData->MakePrefix();
	_NqPrefix = _NqChartData->MakePrefix();
	_NkPrefix = _NkChartData->MakePrefix();
	_HsPrefix = _HsChartData->MakePrefix();

	_KospiChartData->AddCompData(this);
	_NqChartData->AddCompData(this);
	_NkChartData->AddCompData(this);
	_HsChartData->AddCompData(this);

	

	hmdf::StdDataFrame<hmdf::DateTime>& kospi_data_frame = _KospiChartData->GetDataFrame();

	// 1. 나스닥데이터에서 시간을 추출한다.
	hmdf::StdDataFrame<hmdf::DateTime>&  nq_data_frame = _NqChartData->GetDataFrame();

	// 2. 위에서 추출한 시간 데이터 개수와 일치하게 코스피, 니케이, 항생 개수를 조절한다. 
	// 3. 코스피, 나스닥, 니케이, 항생 데이터를 위에서 추출한 시간 데이터를 참조하여 다시 뽑는다. 이때 그 시간에 없는 데이터는 값없음 처리된다. 
	hmdf::StdDataFrame<hmdf::DateTime>& nk_data_frame = _NkChartData->GetDataFrame();
	hmdf::StdDataFrame<hmdf::DateTime>& hs_data_frame = _HsChartData->GetDataFrame();



	hmdf::StdDataFrame<hmdf::DateTime> join_df1, join_df2, join_df3, join_df4;

	join_df1 = kospi_data_frame.join_by_index<decltype(join_df1), double>(nq_data_frame, hmdf::join_policy::left_right_join);

	join_df2 = join_df1.join_by_index<decltype(join_df2), double>(nk_data_frame, hmdf::join_policy::left_right_join);

	join_df3 = join_df2.join_by_index<decltype(join_df3), double>(hs_data_frame, hmdf::join_policy::left_right_join);

	
	std::vector<hmdf::DateTime>& joined_index = join_df3.get_index();
	std::string col_kp(_KpPrefix), col_nq(_NqPrefix), col_nk(_NkPrefix), col_hs(_HsPrefix);
	col_kp.append("close");
	col_nq.append("close");
	col_nk.append("close");
	col_hs.append("close");
	auto& kp = join_df3.get_column<double>(col_kp.c_str());
	auto& nq = join_df3.get_column<double>(col_nq.c_str());
	auto& nk = join_df3.get_column<double>(col_nk.c_str());
	auto& hs = join_df3.get_column<double>(col_hs.c_str());
	std::vector<double> cv_dt(joined_index.size());
	for (size_t i = 0; i < joined_index.size(); i++) {
		//if (isnan(kp[i])) kp[i] = Chart::NoValue;
		//if (isnan(nq[i])) nq[i] = Chart::NoValue;
		//if (isnan(nk[i])) nk[i] = Chart::NoValue;
		//if (isnan(hs[i])) hs[i] = Chart::NoValue;
		double dt = Chart::chartTime(joined_index[i].year(), static_cast<int>(joined_index[i].month()), joined_index[i].dmonth(), joined_index[i].hour(), joined_index[i].minute(), joined_index[i].sec());
		cv_dt[i] = dt;
	}

	std::vector<hmdf::DateTime> new_index;
	copy(joined_index.begin(), joined_index.end(), back_inserter(new_index));
	size_t count = _TempFrame.load_data(std::move(new_index),
		std::make_pair("cv_dt", cv_dt),
		std::make_pair("kp", kp),
		std::make_pair("nq", nq),
		std::make_pair("nk", nk),
		std::make_pair("hs", hs));


	//std::vector<const char*> coll_names;
	//coll_names.push_back("main"); // { "main", "comp", "nk", "hs"}
	_TempFrame.fill_missing<double>({ "kp", "nq", "nk", "hs" }, hmdf::fill_policy::fill_forward);
	_TempFrame.fill_missing<double>({ "kp", "nq", "nk", "hs" }, hmdf::fill_policy::fill_backward);
	
	ProcessChartData();

	CreateTimer();

	_Ready = true;
}

void DarkHorse::SmCompData::ProcessChartData()
{
	std::vector<double> kp;
	std::vector<double> nq;
	std::vector<double> nk;
	std::vector<double> hs;

	hmdf::StdDataFrame<hmdf::DateTime> df2 = _TempFrame.get_data_by_loc<hmdf::DateTime, double>(hmdf::Index2D<long> { -1500, -1 });

	auto& nq_index_v = df2.get_index();
	switch (_OpOption)
	{
	case OP_KOSPI:
		_IdxTime.first = 15;
		_IdxTime.second = 45;
		break;
	case OP_HS:
		_IdxTime.first = 17;
		_IdxTime.second = 30;
		break;
	case OP_NK:
		_IdxTime.first = 16;
		_IdxTime.second = 0;
		break;
	case OP_NQ:
		_IdxTime.first = 16;
		_IdxTime.second = 0;
		break;
	default:
		_IdxTime.first = 15;
		_IdxTime.second = 45;
		break;
	}
	_BaseValues.clear();

	for (auto it = nq_index_v.begin(); it != nq_index_v.end(); ++it) {
		auto cur_dt = *it;
		if (cur_dt.hour() == _IdxTime.first && cur_dt.minute() == _IdxTime.second) {
			double cv_dt = Chart::chartTime(cur_dt.year(), static_cast<int>(cur_dt.month()), cur_dt.dmonth(), cur_dt.hour(), cur_dt.minute(), 0);
			_BaseValues.insert(cv_dt);
		}
	}


	kp = df2.get_column<double>("kp");
	nq = df2.get_column<double>("nq");
	nk = df2.get_column<double>("nk");
	hs = df2.get_column<double>("hs");

	// 하나의 값을 얻고 싶으면 startIdx, endIdx를 같게 한다. 그리고 outBegIdx 는 startIdx와 같게 된다. 
	int startIdx = 0, endIdx = nq.size() - 1;
	int outBegIdx = 0, outNbElement = 0;
	std::vector<double> nq_ma20(nq.size());
	std::vector<double> nq_ma40(nq.size());
	std::vector<double> nq_ma60(nq.size());
	std::vector<double> nq_ma120(nq.size());
	/* ... initialize your closing price here... */
	int retCode = TA_MA(startIdx, endIdx,
		nq.data(),
		20, TA_MAType_SMA,
		&outBegIdx, &outNbElement, nq_ma20.data());

	std::rotate(nq_ma20.rbegin(), nq_ma20.rbegin() + outBegIdx, nq_ma20.rend());

	retCode = TA_MA(startIdx, endIdx,
		nq.data(),
		40, TA_MAType_SMA,
		&outBegIdx, &outNbElement, nq_ma40.data());

	std::rotate(nq_ma40.rbegin(), nq_ma40.rbegin() + outBegIdx, nq_ma40.rend());

	retCode = TA_MA(startIdx, endIdx,
		nq.data(),
		60, TA_MAType_SMA,
		&outBegIdx, &outNbElement, nq_ma60.data());

	std::rotate(nq_ma60.rbegin(), nq_ma60.rbegin() + outBegIdx, nq_ma60.rend());

	retCode = TA_MA(startIdx, endIdx,
		nq.data(),
		120, TA_MAType_SMA,
		&outBegIdx, &outNbElement, nq_ma120.data());

	std::rotate(nq_ma120.rbegin(), nq_ma120.rbegin() + outBegIdx, nq_ma120.rend());

	df2.fill_missing<double>({ "kp", "nq", "nk", "hs" }, hmdf::fill_policy::fill_forward);
	std::vector<double> vir1;
	std::vector<double> vir2;
	std::vector<double> vir3;
	std::vector<double> kp_index_data;
	std::vector<double> nq_index_data;
	std::vector<double> nk_index_data;
	std::vector<double> hs_index_data;
	std::vector<double> price_delta1;
	std::vector<double> price_delta2;
	std::vector<double> price_delta3;
	vir1.resize(df2.shape().first, std::numeric_limits<double>::quiet_NaN());
	vir2.resize(df2.shape().first, std::numeric_limits<double>::quiet_NaN());
	vir3.resize(df2.shape().first, std::numeric_limits<double>::quiet_NaN());
	price_delta1.resize(df2.shape().first, std::numeric_limits<double>::quiet_NaN());
	price_delta2.resize(df2.shape().first, std::numeric_limits<double>::quiet_NaN());
	price_delta3.resize(df2.shape().first, std::numeric_limits<double>::quiet_NaN());
	kp_index_data.resize(df2.shape().first, std::numeric_limits<double>::quiet_NaN());
	nq_index_data.resize(df2.shape().first, std::numeric_limits<double>::quiet_NaN());
	nk_index_data.resize(df2.shape().first, std::numeric_limits<double>::quiet_NaN());
	hs_index_data.resize(df2.shape().first, std::numeric_limits<double>::quiet_NaN());
	double kp_index_value = 0;
	double nq_index_value = 0;
	double nk_index_value = 0;
	double hs_index_value = 0;
	for (size_t i = 0; i < df2.shape().first; i++) {
		auto dt = df2.get_index();
		bool set_index_value = false;
		if (dt[i].hour() == _IdxTime.first && dt[i].minute() == _IdxTime.second) {
			kp_index_value = kp[i];
			nq_index_value = nq[i];
			nk_index_value = nk[i];
			hs_index_value = hs[i];
			kp_index_data[i] = kp_index_value;
			nq_index_data[i] = nq_index_value;
			nk_index_data[i] = nk_index_value;
			hs_index_data[i] = hs_index_value;
			if (_OpOption == OP_HS) {
				vir1[i] = hs_index_value;
				vir2[i] = hs_index_value;
				vir3[i] = hs_index_value;
			}
			else if (_OpOption == OP_NQ) {
				vir1[i] = nq_index_value;
				vir2[i] = nq_index_value;
				vir3[i] = nq_index_value;
			}
			else if (_OpOption == OP_NK) {
				vir1[i] = nk_index_value;
				vir2[i] = nk_index_value;
				vir3[i] = nk_index_value;
			}
			else {
				vir1[i] = kp_index_value;
				vir2[i] = kp_index_value;
				vir3[i] = kp_index_value;
			}
			set_index_value = true;
		}
		if (!set_index_value && i == 0) {
			kp_index_value = kp[i];
			nq_index_value = nq[i];
			nk_index_value = nk[i];
			hs_index_value = hs[i];
			kp_index_data[i] = kp_index_value;
			nq_index_data[i] = nq_index_value;
			nk_index_data[i] = nk_index_value;
			hs_index_data[i] = hs_index_value;
			if (_OpOption == OP_HS) {
				vir1[i] = hs_index_value;
				vir2[i] = hs_index_value;
				vir3[i] = hs_index_value;
			}
			else if (_OpOption == OP_NQ) {
				vir1[i] = nq_index_value;
				vir2[i] = nq_index_value;
				vir3[i] = nq_index_value;
			}
			else if (_OpOption == OP_NK) {
				vir1[i] = nk_index_value;
				vir2[i] = nk_index_value;
				vir3[i] = nk_index_value;
			}
			else {
				vir1[i] = kp_index_value;
				vir2[i] = kp_index_value;
				vir3[i] = kp_index_value;
			}
		}
		else {
			kp_index_data[i] = kp_index_value;
			nq_index_data[i] = nq_index_value;
			nk_index_data[i] = nk_index_value;
			hs_index_data[i] = hs_index_value;

			double nq_delta = nq[i] - nq_index_value;
			double nk_delta = nk[i] - nk_index_value;
			double hs_delta = hs[i] - hs_index_value;
			double kp_delta = kp[i] - kp_index_value;

			double nq_data_percent = nq_delta / nq_index_value;
			double nk_data_percent = nk_delta / nk_index_value;
			double hs_data_percent = hs_delta / hs_index_value;
			double kp_data_percent = kp_delta / kp_index_value;
			
			double nq_price1 = nq_data_percent * _NqRatio1;
			double nq_price2 = nq_data_percent * _NqRatio2;
			double nq_price3 = nq_data_percent * _NqRatio3;

			double nk_price1 = nk_data_percent * _NkRatio1;
			double nk_price2 = nk_data_percent * _NkRatio2;
			double nk_price3 = nk_data_percent * _NkRatio3;

			double hs_price1 = hs_data_percent * _HsRatio1;
			double hs_price2 = hs_data_percent * _HsRatio2;
			double hs_price3 = hs_data_percent * _HsRatio3;

			double kp_price1 = kp_data_percent * _KpRatio1;
			double kp_price2 = kp_data_percent * _KpRatio2;
			double kp_price3 = kp_data_percent * _KpRatio3;


			if (_OpOption == OP_HS) {
				vir1[i] = hs_index_data[i] + hs_index_data[i] * (kp_price1 * _KpAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
				vir2[i] = hs_index_data[i] + hs_index_data[i] * (kp_price2 * _KpAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
				vir3[i] = hs_index_data[i] + hs_index_data[i] * (kp_price3 * _KpAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

				price_delta1[i] = hs[i] - vir1[i];
				price_delta2[i] = hs[i] - vir2[i];
				price_delta3[i] = hs[i] - vir3[i];


			}
			else if (_OpOption == OP_NQ) {
				vir1[i] = nq_index_data[i] + nq_index_data[i] * (nq_price1 * _NqAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
				vir2[i] = nq_index_data[i] + nq_index_data[i] * (nq_price2 * _NqAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
				vir3[i] = nq_index_data[i] + nq_index_data[i] * (nq_price3 * _NqAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

				price_delta1[i] = nk[i] - vir1[i];
				price_delta2[i] = nk[i] - vir2[i];
				price_delta3[i] = nk[i] - vir3[i];


			}
			else if (_OpOption == OP_NK) {
				

				vir1[i] = nk_index_data[i] + nk_index_data[i] * (nq_price1 * _NqAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
				vir2[i] = nk_index_data[i] + nk_index_data[i] * (nq_price2 * _NqAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
				vir3[i] = nk_index_data[i] + nk_index_data[i] * (nq_price3 * _NqAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

				price_delta1[i] = nq[i] - vir1[i];
				price_delta2[i] = nq[i] - vir2[i];
				price_delta3[i] = nq[i] - vir3[i];


			}
			else {
				vir1[i] = kp_index_data[i] + kp_index_data[i] * (nq_price1 * _NqAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
				vir2[i] = kp_index_data[i] + kp_index_data[i] * (nq_price2 * _NqAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
				vir3[i] = kp_index_data[i] + kp_index_data[i] * (nq_price3 * _NqAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

				price_delta1[i] = kp[i] - vir1[i];
				price_delta2[i] = kp[i] - vir2[i];
				price_delta3[i] = kp[i] - vir3[i];


			}
		}
	}

	df2.load_column("nq_index_value", std::move(nq_index_data));
	df2.load_column("kospi_index_value", std::move(kp_index_data));
	df2.load_column("nk_index_value", std::move(nk_index_data));
	df2.load_column("hs_index_value", std::move(hs_index_data));
	df2.load_column("price_delta1", std::move(price_delta1));
	df2.load_column("price_delta2", std::move(price_delta2));
	df2.load_column("price_delta3", std::move(price_delta3));
	df2.load_column("vir1", std::move(vir1));
	df2.load_column("vir2", std::move(vir2));
	df2.load_column("vir3", std::move(vir3));

	df2.load_column("nq_ma20", std::move(nq_ma20));
	df2.load_column("nq_ma40", std::move(nq_ma40));
	df2.load_column("nq_ma60", std::move(nq_ma60));
	df2.load_column("nq_ma120", std::move(nq_ma120));

	df2.fill_missing<double>({ "kospi_index_value", "nq_index_value", "nk_index_value", "hs_index_value" }, hmdf::fill_policy::fill_forward);

	//std::vector<double> miv = df2.get_column<double>("main_index_value");
	//std::vector<double> civ = df2.get_column<double>("comp_index_value");

	_DataFrame = std::move(df2);
}

void DarkHorse::SmCompData::ReprocessChartData()
{
	_Ready = false;
	auto _KospiChartData = mainApp.ChartDataMgr()->FindChartData(_KospiSymbolCode, _ChartType, _Cycle);
	auto _NqChartData = mainApp.ChartDataMgr()->FindChartData(_NqSymbolCode, _ChartType, _Cycle);
	auto _NkChartData = mainApp.ChartDataMgr()->FindChartData(_NikkeiCode, _ChartType, _Cycle);
	auto _HsChartData = mainApp.ChartDataMgr()->FindChartData(_HangSengCode, _ChartType, _Cycle);

	if (!_KospiChartData || !_NqChartData || !_NkChartData || !_HsChartData) { _Ready = true;  return; }

	_KospiChartData->AddCompData(this);
	_NqChartData->AddCompData(this);
	_NkChartData->AddCompData(this);
	_HsChartData->AddCompData(this);

// 	std::set<std::string> index_set;
// 	std::vector<double> kospi;
// 	std::vector<double> nq;
// 	std::vector<double> nk;
// 	std::vector<double> hs;

	hmdf::StdDataFrame<hmdf::DateTime>& kospi_data_frame = _KospiChartData->GetDataFrame();

	// 1. 나스닥데이터에서 시간을 추출한다.
	hmdf::StdDataFrame<hmdf::DateTime>& nq_data_frame = _NqChartData->GetDataFrame();

	// 2. 위에서 추출한 시간 데이터 개수와 일치하게 코스피, 니케이, 항생 개수를 조절한다. 
	// 3. 코스피, 나스닥, 니케이, 항생 데이터를 위에서 추출한 시간 데이터를 참조하여 다시 뽑는다. 이때 그 시간에 없는 데이터는 값없음 처리된다. 
	hmdf::StdDataFrame<hmdf::DateTime>& nk_data_frame = _NkChartData->GetDataFrame();
	hmdf::StdDataFrame<hmdf::DateTime>& hs_data_frame = _HsChartData->GetDataFrame();
	

	hmdf::StdDataFrame<hmdf::DateTime> join_df1, join_df2, join_df3, join_df4;

	join_df1 = kospi_data_frame.join_by_index<decltype(join_df1), double>(nq_data_frame, hmdf::join_policy::left_right_join);

	join_df2 = join_df1.join_by_index<decltype(join_df2), double>(nk_data_frame, hmdf::join_policy::left_right_join);

	join_df3 = join_df2.join_by_index<decltype(join_df3), double>(hs_data_frame, hmdf::join_policy::left_right_join);

	std::vector<hmdf::DateTime>& joined_index = join_df3.get_index();
	std::string col_kp(_KpPrefix), col_nq(_NqPrefix), col_nk(_NkPrefix), col_hs(_HsPrefix);
	col_kp.append("close");
	col_nq.append("close");
	col_nk.append("close");
	col_hs.append("close");
	auto& kp = join_df3.get_column<double>(col_kp.c_str());
	auto& nq = join_df3.get_column<double>(col_nq.c_str());
	auto& nk = join_df3.get_column<double>(col_nk.c_str());
	auto& hs = join_df3.get_column<double>(col_hs.c_str());
	std::vector<double> cv_dt(joined_index.size());
	for (size_t i = 0; i < joined_index.size(); i++) {
		//if (isnan(kp[i])) kp[i] = Chart::NoValue;
		//if (isnan(nq[i])) nq[i] = Chart::NoValue;
		//if (isnan(nk[i])) nk[i] = Chart::NoValue;
		//if (isnan(hs[i])) hs[i] = Chart::NoValue;
		double dt = Chart::chartTime(joined_index[i].year(), static_cast<int>(joined_index[i].month()), joined_index[i].dmonth(), joined_index[i].hour(), joined_index[i].minute(), joined_index[i].sec());
		cv_dt[i] = dt;
	}

	std::vector<hmdf::DateTime> new_index;
	copy(joined_index.begin(), joined_index.end(), back_inserter(new_index));
	size_t count = _TempFrame.load_data(std::move(new_index),
		std::make_pair("cv_dt", cv_dt),
		std::make_pair("kp", kp),
		std::make_pair("nq", nq),
		std::make_pair("nk", nk),
		std::make_pair("hs", hs));


	//std::vector<const char*> coll_names;
	//coll_names.push_back("main"); // { "main", "comp", "nk", "hs"}
	_TempFrame.fill_missing<double>({ "kp", "nq", "nk", "hs" }, hmdf::fill_policy::fill_forward);
	_TempFrame.fill_missing<double>({ "kp", "nq", "nk", "hs" }, hmdf::fill_policy::fill_backward);

	ProcessChartData();

	_Ready = true;
}

void DarkHorse::SmCompData::CalcMultiLineColorPolicy()
{
	
}

// 날짜를 찾아 본다.
// 날짜가 없으면 날짜를 만들어 준다.
// 인덱스를 추가해 준다.
// 해당 시리즈에 값을 추가해 준다.
// 지정된 시간이 되었으면 메인 데이터와 비교 데이터를 다시 설정해 준다.
// 그리고 가상 데이터의 초기 값을 설정해 준다.
// 지정된 시간이 아니라면 메인 데이터 상승률을 구하고 가상 데이터 값을 계산하여 설정해 준다.
// 차트를 갱신한다.
void DarkHorse::SmCompData::AddChartData(const double& h, const double& l, const double& o, const double& c, const double& v, const int& date, const int& time, const int& mil_sec)
{
	int i = 0;
	i = i + 0;
}

std::vector<double> DarkHorse::SmCompData::GetLastData()
{
	double value = _DataFrame.get_column<double>("kp")[_DataFrame.shape().first - 1];
	std::vector<double> value_vec;
	value_vec.push_back(value);
	value = _DataFrame.get_column<double>("nq")[_DataFrame.shape().first - 1];
	value_vec.push_back(value);
	value = _DataFrame.get_column<double>("vir1")[_DataFrame.shape().first - 1];
	value_vec.push_back(value);
	return value_vec;
}

void DarkHorse::SmCompData::RemoveChartRef()
{
	auto _MainChartData = mainApp.ChartDataMgr()->FindChartData(_KospiSymbolCode, _ChartType, _Cycle);
	if (_MainChartData) _MainChartData->RemoveCompData(this);
	auto _CompChartData = mainApp.ChartDataMgr()->FindChartData(_NqSymbolCode, _ChartType, _Cycle);
	if (_CompChartData) _CompChartData->RemoveCompData(this);
}

void DarkHorse::SmCompData::UpdateChartData(const std::string& symbol_code, const double& close)
{
	if (!_Ready) return;

	std::string col_kp(_KpPrefix), col_nq(_NqPrefix), col_nk(_NkPrefix), col_hs(_HsPrefix);
	col_kp.append("close");
	col_nq.append("close");
	col_nk.append("close");
	col_hs.append("close");

	std::vector<double>& kp = _DataFrame.get_column<double>("kp");
	std::vector<double>& nq = _DataFrame.get_column<double>("nq");
	std::vector<double>& nk = _DataFrame.get_column<double>("nk");
	std::vector<double>& hs = _DataFrame.get_column<double>("hs");
	std::vector<double>& vir1 = _DataFrame.get_column<double>("vir1");
	std::vector<double>& vir2 = _DataFrame.get_column<double>("vir2");
	std::vector<double>& vir3 = _DataFrame.get_column<double>("vir3");
	std::vector<double>& kiv = _DataFrame.get_column<double>("kospi_index_value");
	std::vector<double>& qiv = _DataFrame.get_column<double>("nq_index_value");
	std::vector<double>& niv = _DataFrame.get_column<double>("nk_index_value");
	std::vector<double>& hiv = _DataFrame.get_column<double>("hs_index_value");
	std::vector<double>& price_delta1 = _DataFrame.get_column<double>("price_delta1");
	std::vector<double>& price_delta2 = _DataFrame.get_column<double>("price_delta2");
	std::vector<double>& price_delta3 = _DataFrame.get_column<double>("price_delta3");

	if (_KospiSymbolCode.compare(symbol_code) == 0) {
		auto _MainChartData = mainApp.ChartDataMgr()->FindChartData(_KospiSymbolCode, _ChartType, _Cycle);
		kp[kp.size() - 1] = _MainChartData->GetDataFrame().get_column<double>(col_kp.c_str()).back();
	}
	if (_NqSymbolCode.compare(symbol_code) == 0) {
		auto _CompChartData = mainApp.ChartDataMgr()->FindChartData(_NqSymbolCode, _ChartType, _Cycle);
		nq[nq.size() - 1] = _CompChartData->GetDataFrame().get_column<double>(col_nq.c_str()).back();
	}

	if (_NikkeiCode.compare(symbol_code) == 0) {
		auto _CompChartData = mainApp.ChartDataMgr()->FindChartData(_NikkeiCode, _ChartType, _Cycle);
		nk[nk.size() - 1] = _CompChartData->GetDataFrame().get_column<double>(col_nk.c_str()).back();
	}

	if (_HangSengCode.compare(symbol_code) == 0) {
		auto _CompChartData = mainApp.ChartDataMgr()->FindChartData(_HangSengCode, _ChartType, _Cycle);
		hs[hs.size() - 1] = _CompChartData->GetDataFrame().get_column<double>(col_hs.c_str()).back();
	}

	double nq_delta = nq.back() - qiv.back();
	double nk_delta = nk.back() - niv.back();
	double hs_delta = hs.back() - hiv.back();
	double kp_delta = kp.back() - kiv.back();

	double nq_data_percent = nq_delta  / qiv.back();
	double nk_data_percent = nk_delta  / niv.back();
	double hs_data_percent = hs_delta  / hiv.back();
	double kp_data_percent = kp_delta / kiv.back();

	double nq_price1 = nq_data_percent * _NqRatio1;
	double nq_price2 = nq_data_percent * _NqRatio2;
	double nq_price3 = nq_data_percent * _NqRatio3;

	double nk_price1 = nk_data_percent * _NkRatio1;
	double nk_price2 = nk_data_percent * _NkRatio2;
	double nk_price3 = nk_data_percent * _NkRatio3;

	double hs_price1 = hs_data_percent * _HsRatio1;
	double hs_price2 = hs_data_percent * _HsRatio2;
	double hs_price3 = hs_data_percent * _HsRatio3;

	double kp_price1 = kp_data_percent * _KpRatio1;
	double kp_price2 = kp_data_percent * _KpRatio2;
	double kp_price3 = kp_data_percent * _KpRatio3;

	

	if (_OpOption == OP_HS) {
		vir1[vir1.size() - 1] = hiv[vir1.size() - 1] + hiv[vir1.size() - 1] * (kp_price1 * _KpAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
		vir2[vir2.size() - 1] = hiv[vir1.size() - 1] + hiv[vir1.size() - 1] * (kp_price2 * _KpAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
		vir3[vir3.size() - 1] = hiv[vir1.size() - 1] + hiv[vir1.size() - 1] * (kp_price3 * _KpAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

		price_delta1[price_delta1.size() - 1] = hs.back() - vir1.back();
		price_delta2[price_delta2.size() - 1] = hs.back() - vir2.back();
		price_delta2[price_delta3.size() - 1] = hs.back() - vir3.back();


	}
	else if (_OpOption == OP_NQ) {
		vir1[vir1.size() - 1] = qiv[vir1.size() - 1] + qiv[vir1.size() - 1] * (nq_price1 * _NqAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
		vir2[vir2.size() - 1] = qiv[vir1.size() - 1] + qiv[vir1.size() - 1] * (nq_price2 * _NqAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
		vir3[vir3.size() - 1] = qiv[vir1.size() - 1] + qiv[vir1.size() - 1] * (nq_price3 * _NqAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

		price_delta1[price_delta1.size() - 1] = nq.back() - vir1.back();
		price_delta2[price_delta2.size() - 1] = nq.back() - vir2.back();
		price_delta2[price_delta3.size() - 1] = nq.back() - vir3.back();


	}
	else if (_OpOption == OP_NK) {
		vir1[vir1.size() - 1] = niv[vir1.size() - 1] + niv[vir1.size() - 1] * (nq_price1 * _NqAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
		vir2[vir2.size() - 1] = niv[vir1.size() - 1] + niv[vir1.size() - 1] * (nq_price2 * _NqAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
		vir3[vir3.size() - 1] = niv[vir1.size() - 1] + niv[vir1.size() - 1] * (nq_price3 * _NqAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

		price_delta1[price_delta1.size() - 1] = nk.back() - vir1.back();
		price_delta2[price_delta2.size() - 1] = nk.back() - vir2.back();
		price_delta2[price_delta3.size() - 1] = nk.back() - vir3.back();


	}
	else {
		vir1[vir1.size() - 1] = kiv[vir1.size() - 1] + kiv[vir1.size() - 1] * (nq_price1 * _NqAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
		vir2[vir2.size() - 1] = kiv[vir1.size() - 1] + kiv[vir1.size() - 1] * (nq_price2 * _NqAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
		vir3[vir3.size() - 1] = kiv[vir1.size() - 1] + kiv[vir1.size() - 1] * (nq_price3 * _NqAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

		price_delta1[price_delta1.size() - 1] = kp.back() - vir1.back();
		price_delta2[price_delta2.size() - 1] = kp.back() - vir2.back();
		price_delta2[price_delta3.size() - 1] = kp.back() - vir3.back();


	}

	if (_Parent) _Parent->SendMessage(UM_COMP_UPDATED);
}

void DarkHorse::SmCompData::Multiply(int val)
{
	_Multiply = val;
}

void DarkHorse::SmCompData::OnTimer()
{
	if (!_Ready) return;

	auto _KospiChartData = mainApp.ChartDataMgr()->FindChartData(_KospiSymbolCode, _ChartType, _Cycle);
	auto _NqChartData = mainApp.ChartDataMgr()->FindChartData(_NqSymbolCode, _ChartType, _Cycle);
	auto _NkChartData = mainApp.ChartDataMgr()->FindChartData(_NikkeiCode, _ChartType, _Cycle);
	auto _HsChartData = mainApp.ChartDataMgr()->FindChartData(_HangSengCode, _ChartType, _Cycle);
	if (!_KospiChartData || !_NqChartData || !_NkChartData || !_HsChartData) return;


	DarkHorse::SmStockData kp_data, nq_data, nk_data, hs_data;
	_KospiChartData->GetLastData(kp_data);
	_NqChartData->GetLastData(nq_data);
	_NkChartData->GetLastData(nk_data);
	_HsChartData->GetLastData(hs_data);

	hmdf::DateTime new_date;
	if (new_date.is_sunday()) return;
	if (new_date.is_saturday() && new_date.hour() > 6) return;
	if (new_date.is_monday() && new_date.hour() < 7) return;

	std::vector<hmdf::DateTime>& index = _DataFrame.get_index();
	hmdf::DateTime last_date = index[index.size() - 1];
	if (last_date.compare(new_date) < 0) {
		std::rotate(index.begin(), index.begin() + 1, index.end());
		index.data()[index.size() - 1] = new_date;

		auto& dt_v = _DataFrame.get_column<double>("cv_dt"); // Converted To Chart Director DateTime

		double cv_dt = Chart::chartTime(new_date.year(), static_cast<int>(new_date.month()), new_date.dmonth(), new_date.hour(), new_date.minute(), new_date.sec());

		std::rotate(dt_v.begin(), dt_v.begin() + 1, dt_v.end());
		dt_v.data()[dt_v.size() - 1] = cv_dt;

		std::vector<double>& kp = _DataFrame.get_column<double>("kp");
		std::vector<double>& nq = _DataFrame.get_column<double>("nq");
		std::vector<double>& nk = _DataFrame.get_column<double>("nk");
		std::vector<double>& hs = _DataFrame.get_column<double>("hs");
		std::vector<double>& vir1 = _DataFrame.get_column<double>("vir1");
		std::vector<double>& vir2 = _DataFrame.get_column<double>("vir2");
		std::vector<double>& vir3 = _DataFrame.get_column<double>("vir3");
		std::vector<double>& kiv = _DataFrame.get_column<double>("kospi_index_value");
		std::vector<double>& qiv = _DataFrame.get_column<double>("nq_index_value");
		std::vector<double>& niv = _DataFrame.get_column<double>("nk_index_value");
		std::vector<double>& hiv = _DataFrame.get_column<double>("hs_index_value");
		std::vector<double>& price_delta1 = _DataFrame.get_column<double>("price_delta1");
		std::vector<double>& price_delta2 = _DataFrame.get_column<double>("price_delta2");
		std::vector<double>& price_delta3 = _DataFrame.get_column<double>("price_delta3");
		if (last_date.hour() == _IdxTime.first && last_date.minute() == _IdxTime.second) {
			
			_BaseValues.insert(cv_dt);

			std::shift_left(kiv.begin(), kiv.end(), 1);
			kiv[kiv.size() - 1] = kp_data.close;

			std::shift_left(qiv.begin(), qiv.end(), 1);
			qiv[qiv.size() - 1] = nq_data.close;

			std::shift_left(niv.begin(), niv.end(), 1);
			niv[niv.size() - 1] = nk_data.close;

			std::shift_left(hiv.begin(), hiv.end(), 1);
			hiv[hiv.size() - 1] = hs_data.close;

			std::shift_left(kp.begin(), kp.end(), 1);
			kp[kp.size() - 1] = kp_data.close;

			std::shift_left(nq.begin(), nq.end(), 1);
			nq[nq.size() - 1] = nq_data.close;

			std::shift_left(nk.begin(), nk.end(), 1);
			nk[nk.size() - 1] = nk_data.close;

			std::shift_left(hs.begin(), hs.end(), 1);
			hs[hs.size() - 1] = hs_data.close;

			std::shift_left(vir1.begin(), vir1.end(), 1);
			vir1[vir1.size() - 1] = kp_data.close;

			std::shift_left(vir2.begin(), vir2.end(), 1);
			vir2[vir2.size() - 1] = kp_data.close;

			std::shift_left(vir3.begin(), vir3.end(), 1);
			vir3[vir3.size() - 1] = kp_data.close;

			std::shift_left(price_delta1.begin(), price_delta1.end(), 1);
			price_delta1[price_delta1.size() - 1] = 0;

			std::shift_left(price_delta2.begin(), price_delta2.end(), 1);
			price_delta2[price_delta2.size() - 1] = 0;

			std::shift_left(price_delta3.begin(), price_delta3.end(), 1);
			price_delta3[price_delta3.size() - 1] = 0;
		}
		else {
			std::shift_left(kp.begin(), kp.end(), 1);
			kp[kp.size() - 1] = kp_data.close;

			std::shift_left(nq.begin(), nq.end(), 1);
			nq[nq.size() - 1] = nq_data.close;

			std::shift_left(nk.begin(), nk.end(), 1);
			nk[nk.size() - 1] = nk_data.close;

			std::shift_left(hs.begin(), hs.end(), 1);
			hs[hs.size() - 1] = hs_data.close;

			double last_data = kiv.back();
			std::shift_left(kiv.begin(), kiv.end(), 1);
			kiv[kiv.size() - 1] = last_data;

			last_data = qiv.back();
			std::shift_left(qiv.begin(), qiv.end(), 1);
			qiv[qiv.size() - 1] = last_data;

			last_data = niv.back();
			std::shift_left(niv.begin(), niv.end(), 1);
			niv[niv.size() - 1] = last_data;

			last_data = hiv.back();
			std::shift_left(hiv.begin(), hiv.end(), 1);
			hiv[hiv.size() - 1] = last_data;



			double nq_delta = nq_data.close - qiv.back();
			double nk_delta = nk_data.close - niv.back();
			double hs_delta = hs_data.close - hiv.back();
			double kp_delta = kp_data.close - kiv.back();

			double nq_data_percent = nq_delta  / qiv.back();
			double nk_data_percent = nk_delta  / niv.back();
			double hs_data_percent = hs_delta  / hiv.back();
			double kp_data_percent = kp_delta / kiv.back();

			double nq_price1 = nq_data_percent * _NqRatio1;
			double nq_price2 = nq_data_percent * _NqRatio2;
			double nq_price3 = nq_data_percent * _NqRatio3;

			double nk_price1 = nk_data_percent * _NkRatio1;
			double nk_price2 = nk_data_percent * _NkRatio2;
			double nk_price3 = nk_data_percent * _NkRatio3;

			double hs_price1 = hs_data_percent * _HsRatio1;
			double hs_price2 = hs_data_percent * _HsRatio2;
			double hs_price3 = hs_data_percent * _HsRatio3;

			double kp_price1 = kp_data_percent * _KpRatio1;
			double kp_price2 = kp_data_percent * _KpRatio2;
			double kp_price3 = kp_data_percent * _KpRatio3;

			std::shift_left(vir1.begin(), vir1.end(), 1);
			std::shift_left(vir2.begin(), vir2.end(), 1);
			std::shift_left(vir3.begin(), vir3.end(), 1);
			std::shift_left(price_delta1.begin(), price_delta1.end(), 1);
			std::shift_left(price_delta2.begin(), price_delta2.end(), 1);
			std::shift_left(price_delta3.begin(), price_delta3.end(), 1);

			if (_OpOption == OP_HS) {
				vir1[vir1.size() - 1] = hiv[vir1.size() - 1] + hiv[vir1.size() - 1] * (kp_price1 * _KpAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
				vir2[vir2.size() - 1] = hiv[vir2.size() - 1] + hiv[vir1.size() - 1] * (kp_price2 * _KpAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
				vir3[vir3.size() - 1] = hiv[vir3.size() - 1] + hiv[vir1.size() - 1] * (kp_price3 * _KpAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

				price_delta1[price_delta1.size() - 1] = hs.back() - vir1.back();
				price_delta2[price_delta2.size() - 1] = hs.back() - vir2.back();
				price_delta3[price_delta3.size() - 1] = hs.back() - vir3.back();


			}
			else if (_OpOption == OP_NQ) {
				vir1[vir1.size() - 1] = qiv[vir1.size() - 1] + qiv[vir1.size() - 1] * (nq_price1 * _NqAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
				vir2[vir2.size() - 1] = qiv[vir2.size() - 1] + qiv[vir1.size() - 1] * (nq_price2 * _NqAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
				vir3[vir3.size() - 1] = qiv[vir3.size() - 1] + qiv[vir1.size() - 1] * (nq_price3 * _NqAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

				price_delta1[price_delta1.size() - 1] = nq.back() - vir1.back();
				price_delta2[price_delta2.size() - 1] = nq.back() - vir2.back();
				price_delta3[price_delta3.size() - 1] = nq.back() - vir3.back();


			}
			else if (_OpOption == OP_NK) {
				vir1[vir1.size() - 1] = niv[vir1.size() - 1] + niv[vir1.size() - 1] * (nq_price1 * _NqAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
				vir2[vir2.size() - 1] = niv[vir2.size() - 1] + niv[vir1.size() - 1] * (nq_price2 * _NqAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
				vir3[vir3.size() - 1] = niv[vir3.size() - 1] + niv[vir1.size() - 1] * (nq_price3 * _NqAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

				price_delta1[price_delta1.size() - 1] = nk.back() - vir1.back();
				price_delta2[price_delta2.size() - 1] = nk.back() - vir2.back();
				price_delta3[price_delta3.size() - 1] = nk.back() - vir3.back();


			}
			else {
				vir1[vir1.size() - 1] = kiv[vir1.size() - 1] + kiv[vir1.size() - 1] * (nq_price1 * _NqAvgRatio1 + nk_price1 * _NkAvgRatio1 + hs_price1 * _HsAvgRatio1);
				vir2[vir2.size() - 1] = kiv[vir2.size() - 1] + kiv[vir1.size() - 1] * (nq_price2 * _NqAvgRatio2 + nk_price2 * _NkAvgRatio2 + hs_price2 * _HsAvgRatio2);
				vir3[vir3.size() - 1] = kiv[vir3.size() - 1] + kiv[vir1.size() - 1] * (nq_price3 * _NqAvgRatio3 + nk_price3 * _NkAvgRatio3 + hs_price3 * _HsAvgRatio3);

				price_delta1[price_delta1.size() - 1] = kp.back() - vir1.back();
				price_delta2[price_delta2.size() - 1] = kp.back() - vir2.back();
				price_delta3[price_delta3.size() - 1] = kp.back() - vir3.back();


			}
		}
	}

	if (_Parent) {
		_Parent->SendMessage(UM_COMP_ADDED);
	}
}

void DarkHorse::SmCompData::CreateTimer()
{
	std::vector<int> date_time = SmUtil::GetLocalDateTime();
	int minMod = date_time[4] % _Cycle;
	int waitTime = _Cycle * 60 - (minMod * 60 + date_time[5]);
	// Add to the timer.
	_TimerId = _Timer.add(seconds(waitTime), std::bind(&SmCompData::OnTimer, this), seconds(_Cycle * 60));
}
