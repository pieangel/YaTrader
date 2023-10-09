#include "stdafx.h"
#include "SmChartData.h"
#include "SmChartDataManager.h"
#include "../Util/SmUtil.h"
#include "../Util/VtStringUtil.h"
#include "../Event/SmCallbackManager.h"
#include "../Global/SmTotalManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../DataFrame/Utils/DateTime.h"
#include "../TimeData/SmCompData.h"
#include "../Pnf/PnfMaker.h"
#include "../Pnf/PnfManager.h"
#include <time.h>
#include <type_traits>
#include <chartdir.h>
#include <algorithm>
#include "../TaLib/tapp.h"
#include "../System/SmArrayMath.h"
#include <format>

using namespace DarkHorse;

std::string DarkHorse::SmChartData::GetChartDataKey()
{
	return SmChartDataManager::MakeDataKey(_SymbolCode, _ChartType, _Cycle);
}

void DarkHorse::SmChartData::AddChartData(const double& h, const double& l, const double& o, const double& c, const double& v, const int& date, const int& time, const int& mil_sec)
{
	std::vector<int> hms = SmUtil::IntToTime(time);
	std::vector<int> ymd = SmUtil::IntToDate(date);
	hmdf::DateTime    chart_time(date, hms[0], hms[1], hms[2], mil_sec);
	std::vector<hmdf::DateTime>& index = _DataFrame.get_index();

	bool bCopy = std::is_trivially_copyable<hmdf::DateTime>::value;

	std::rotate(index.begin(), index.begin() + 1, index.end());
	index.data()[index.size() - 1] = chart_time;

	hmdf::StdDataFrame<hmdf::DateTime>& df = GetDataFrame();

	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	auto& dt_v = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime

	double cv_dt = Chart::chartTime(ymd[0], ymd[1], ymd[2], hms[0], hms[1], hms[2]);

	std::vector<double>& h_v = df.get_column<double>(col_high.c_str());
	std::vector<double>& l_v = df.get_column<double>(col_low.c_str());
	std::vector<double>& o_v = df.get_column<double>(col_open.c_str());
	std::vector<double>& c_v = df.get_column<double>(col_close.c_str());
	std::vector<double>& v_v = df.get_column<double>(col_volume.c_str());

	std::rotate(dt_v.begin(), dt_v.begin() + 1, dt_v.end());
	dt_v.data()[dt_v.size() - 1] = cv_dt;

	std::rotate(h_v.begin(), h_v.begin() + 1, h_v.end());
	h_v.data()[h_v.size() - 1] = h;

	std::rotate(l_v.begin(), l_v.begin() + 1, l_v.end());
	l_v.data()[l_v.size() - 1] = l;

	std::rotate(o_v.begin(), o_v.begin() + 1, o_v.end());
	o_v.data()[o_v.size() - 1] = o;

	std::rotate(c_v.begin(), c_v.begin() + 1, c_v.end());
	c_v.data()[c_v.size() - 1] = c;

	std::rotate(v_v.begin(), v_v.begin() + 1, v_v.end());
	v_v.data()[v_v.size() - 1] = v;

	int startIdx = c_v.size() - 1, endIdx = c_v.size() - 1;
	int outBegIdx = 0, outNbElement = 0;
	std::vector<double> m20(1);
	std::vector<double> m40(1);
	std::vector<double> m60(1);
	std::vector<double> m120(1);

	int retCode = TA_MA(startIdx, endIdx,
		c_v.data(),
		20, TA_MAType_SMA,
		&outBegIdx, &outNbElement, m20.data());



	retCode = TA_MA(startIdx, endIdx,
		c_v.data(),
		40, TA_MAType_SMA,
		&outBegIdx, &outNbElement, m40.data());


	retCode = TA_MA(startIdx, endIdx,
		c_v.data(),
		60, TA_MAType_SMA,
		&outBegIdx, &outNbElement, m60.data());


	retCode = TA_MA(startIdx, endIdx,
		c_v.data(),
		120, TA_MAType_SMA,
		&outBegIdx, &outNbElement, m120.data());

	std::string col_ma20(prefix), col_ma40(prefix), col_ma60(prefix), col_ma120(prefix);
	col_ma20.append("ma20");
	col_ma40.append("ma40");
	col_ma60.append("ma60");
	col_ma120.append("ma120");
	auto& ma20 = _DataFrame.get_column<double>(col_ma20.c_str());
	auto& ma40 = _DataFrame.get_column<double>(col_ma40.c_str());
	auto& ma60 = _DataFrame.get_column<double>(col_ma60.c_str());
	auto& ma120 = _DataFrame.get_column<double>(col_ma120.c_str());

	std::string col_point_type(prefix);
	col_point_type.append("point_type");

	auto& point_type = _DataFrame.get_column<int>(col_point_type.c_str());

	std::rotate(point_type.begin(), point_type.begin() + 1, point_type.end());
	point_type.data()[point_type.size() - 1] = GetMultiColorValue(c, m20[0], m40[0], m60[0], m120[0]);

	std::rotate(ma20.begin(), ma20.begin() + 1, ma20.end());
	ma20.data()[ma20.size() - 1] = m20[0];

	std::rotate(ma40.begin(), ma40.begin() + 1, ma40.end());
	ma40.data()[ma40.size() - 1] = m40[0];

	std::rotate(ma60.begin(), ma60.begin() + 1, ma60.end());
	ma60.data()[ma60.size() - 1] = m60[0];

	std::rotate(ma120.begin(), ma120.begin() + 1, ma120.end());
	ma120.data()[ma120.size() - 1] = m120[0];

	std::string col_ha_high(prefix), col_ha_close(prefix), col_ha_low(prefix), col_ha_open(prefix);
	col_ha_high.append("ha_high");
	col_ha_close.append("ha_close");
	col_ha_low.append("ha_low");
	col_ha_open.append("ha_open");
	auto& ha_high = _DataFrame.get_column<double>(col_ha_high.c_str());
	auto& ha_close = _DataFrame.get_column<double>(col_ha_close.c_str());
	auto& ha_low = _DataFrame.get_column<double>(col_ha_low.c_str());
	auto& ha_open = _DataFrame.get_column<double>(col_ha_open.c_str());

	std::rotate(ha_high.begin(), ha_high.begin() + 1, ha_high.end());
	std::rotate(ha_low.begin(), ha_low.begin() + 1, ha_low.end());
	std::rotate(ha_open.begin(), ha_open.begin() + 1, ha_open.end());
	std::rotate(ha_close.begin(), ha_close.begin() + 1, ha_close.end());

	size_t last_index = _DataFrame.shape().first - 1;
	ha_open[last_index] = o;
	ha_close[last_index] = (o + h + l + c) / 4.0;
	ha_high[last_index] = (std::max)({ ha_open[last_index], ha_close[last_index], h });
	ha_low[last_index] = (std::min)({ ha_open[last_index], ha_close[last_index], l });

	//int outBegIdx = 0, outNbElement = 0;
	std::vector<double> mahigh(1);
	std::vector<double> malow(1);
	std::vector<double> maopen(1);
	std::vector<double> maclose(1);

	retCode = TA_MA(last_index, last_index,
		h_v.data(),
		heiken_ashi_smooth_period, TA_MAType_SMA,
		&outBegIdx, &outNbElement, mahigh.data());
	retCode = TA_MA(last_index, last_index,
		l_v.data(),
		heiken_ashi_smooth_period, TA_MAType_SMA,
		&outBegIdx, &outNbElement, malow.data());
	retCode = TA_MA(last_index, last_index,
		o_v.data(),
		heiken_ashi_smooth_period, TA_MAType_SMA,
		&outBegIdx, &outNbElement, maopen.data());
	retCode = TA_MA(last_index, last_index,
		c_v.data(),
		heiken_ashi_smooth_period, TA_MAType_SMA,
		&outBegIdx, &outNbElement, maclose.data());


	std::string col_ha2_high(prefix), col_ha2_close(prefix), col_ha2_low(prefix), col_ha2_open(prefix);
	col_ha2_high.append("ha2_high");
	col_ha2_close.append("ha2_close");
	col_ha2_low.append("ha2_low");
	col_ha2_open.append("ha2_open");

	auto& ha2_high = _DataFrame.get_column<double>(col_ha2_high.c_str());
	auto& ha2_close = _DataFrame.get_column<double>(col_ha2_close.c_str());
	auto& ha2_low = _DataFrame.get_column<double>(col_ha2_low.c_str());
	auto& ha2_open = _DataFrame.get_column<double>(col_ha2_open.c_str());

	ha2_close[last_index] = (maopen[0] + mahigh[0] + malow[0] + maclose[0]) / 4.0;
	ha2_high[last_index] = (std::max)({ ha2_open[last_index], ha2_close[last_index], mahigh[0] });
	ha2_low[last_index] = (std::min)({ ha2_open[last_index], ha2_close[last_index], malow[0] });




	COleDateTime ole_date_time;
	ole_date_time.SetDateTime(ymd[0], ymd[1], ymd[2], hms[0], hms[1], hms[2]);
	auto symbol = mainApp.SymMgr()->FindSymbol(_SymbolCode);
	if (!symbol) return;
	CBCGPChartStockData data(o , h , l, c, ole_date_time);
	ExternalStorage.ShiftData(data);
	mainApp.CallbackMgr()->OnChartEvent(shared_from_this(), 2);
}

void SmChartData::SetChartData(const double& dt, const double& close, const double& high, const double& low)
{
	std::vector<double> target;
	target.push_back(dt);

	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	hmdf::StdDataFrame<hmdf::DateTime>& df = GetDataFrame();
	auto& dt_v = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high_v = df.get_column<double>(col_high.c_str());
	auto& low_v = df.get_column<double>(col_low.c_str());
	auto& open_v = df.get_column<double>(col_open.c_str());
	auto& close_v = df.get_column<double>(col_close.c_str());

	const auto it = std::search(dt_v.begin(), dt_v.end(), target.begin(), target.end());
	if (it != dt_v.end()) {
		const int index = std::distance(dt_v.begin(), it);
		if (high > high_v[index])
			high_v[index] = high;
		if (low < low_v[index])
			low_v[index] = low;
		close_v[index] = close;
	}
}

void DarkHorse::SmChartData::ShiftData(int* data, const int& len, const int& newValue)
{
	memmove(data, data + 1, sizeof(*data) * (len - 1));
	data[len - 1] = newValue;
}

void DarkHorse::SmChartData::ShiftData(double* data, const int& len, const double& newValue)
{
	memmove(data, data + 1, sizeof(*data) * (len - 1));
	data[len - 1] = newValue;
}

size_t SmChartData::CreateDataFrame(std::vector<double>& cd_v, std::vector<double>& h_v, std::vector<double>& l_v, std::vector<double>& o_v, std::vector<double>& c_v, std::vector<double>& v_v, std::vector<hmdf::DateTime>& dt_v)
{
	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	size_t count = _DataFrame.load_data(std::move(dt_v),
		std::make_pair(col_dt.c_str(), cd_v),
		std::make_pair(col_high.c_str(), h_v),
		std::make_pair(col_low.c_str(), l_v),
		std::make_pair(col_open.c_str(), o_v),
		std::make_pair(col_close.c_str(), c_v),
		std::make_pair(col_volume.c_str(), v_v));
	return count;
}

void SmChartData::ClearData()
{
	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	hmdf::StdDataFrame<hmdf::DateTime>& df = GetDataFrame();
	auto& dt_v = df.get_index();
	auto& cv_dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high_v = df.get_column<double>(col_high.c_str());
	auto& low_v = df.get_column<double>(col_low.c_str());
	auto& open_v = df.get_column<double>(col_open.c_str());
	auto& close_v = df.get_column<double>(col_close.c_str());
	auto& vol_v = df.get_column<double>(col_volume.c_str());

	dt_v.clear();
	cv_dt.clear();
	high_v.clear();
	low_v.clear();
	open_v.clear();
	close_v.clear();
	vol_v.clear();
}

void SmChartData::AddChartData(const double& dt, const double& open, const double& high, const double& low, const double& close)
{
	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	hmdf::StdDataFrame<hmdf::DateTime>& df = GetDataFrame();
	auto& dt_v = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high_v = df.get_column<double>(col_high.c_str());
	auto& low_v = df.get_column<double>(col_low.c_str());
	auto& open_v = df.get_column<double>(col_open.c_str());
	auto& close_v = df.get_column<double>(col_close.c_str());
	auto& vol_v = df.get_column<double>(col_volume.c_str());
	dt_v.push_back(dt);
	open_v.push_back(open);
	high_v.push_back(high);
	low_v.push_back(low);
	close_v.push_back(close);
	vol_v.push_back(0);
}

void SmChartData::SetChartData(std::vector<double>&& cd_v, std::vector<double>&& h_v, std::vector<double>&& l_v, std::vector<double>&& o_v, std::vector<double>&& c_v, std::vector<double>&& v_v, std::vector<hmdf::DateTime>&& dt_v)
{
	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");
	size_t count = _DataFrame.load_data(std::move(dt_v),
		std::make_pair(col_dt.c_str(), cd_v),
		std::make_pair(col_high.c_str(), h_v),
		std::make_pair(col_low.c_str(), l_v),
		std::make_pair(col_open.c_str(), o_v),
		std::make_pair(col_close.c_str(), c_v),
		std::make_pair(col_volume.c_str(), v_v));

	auto symbol = mainApp.SymMgr()->FindSymbol(_SymbolCode);
	if (!symbol) return;

	
	double divedend = pow(10, symbol->decimal());
	double open = 0, high = 0, low = 0, close = 0;
	for (size_t i = 0; i < _DataFrame.shape().first; i++) {
		const std::vector<hmdf::DateTime>& index = _DataFrame.get_index();
		COleDateTime ole_date_time;
		ole_date_time.SetDateTime(index[i].year(), static_cast<int>(index[i].month()), index[i].dmonth(), index[i].hour(), index[i].minute(), index[i].sec());
		open = _DataFrame.get_column<double>(col_open.c_str())[i] ;
		open = open / divedend;
		high = _DataFrame.get_column<double>(col_high.c_str())[i];
		high = high / divedend;
		low = _DataFrame.get_column<double>(col_low.c_str())[i];
		low = low / divedend;
		close = _DataFrame.get_column<double>(col_close.c_str())[i];
		close = close / divedend;
		_DataFrame.get_column<double>(col_open.c_str())[i] = open;
		_DataFrame.get_column<double>(col_high.c_str())[i] = high;
		_DataFrame.get_column<double>(col_low.c_str())[i] = low;
		_DataFrame.get_column<double>(col_close.c_str())[i] = close;
		ExternalStorage.AddData(
			open,
			high,
			low,
			close, ole_date_time);
	}
	
}

void SmChartData::InitDataFrame()
{
	std::vector<double> cd_v;
	std::vector<double> h_v;
	std::vector<double> l_v;
	std::vector<double> o_v;
	std::vector<double> c_v;
	std::vector<double> v_v;
	std::vector<hmdf::DateTime> dt_v;
	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	size_t count = _DataFrame.load_data(std::move(dt_v),
		std::make_pair(col_dt.c_str(), cd_v),
		std::make_pair(col_high.c_str(), h_v),
		std::make_pair(col_low.c_str(), l_v),
		std::make_pair(col_open.c_str(), o_v),
		std::make_pair(col_close.c_str(), c_v),
		std::make_pair(col_volume.c_str(), v_v));
}

void DarkHorse::SmChartData::MakeNewBarByTimer()
{
	if (_ChartType != SmChartType::MIN) return;

	auto symbol = mainApp.SymMgr()->FindSymbol(_SymbolCode);
	if (!symbol) return;

	// 차트데이터를 한번도 받지 않았으면 진행시키지 않는다.
	if (!_Received) return;
	// 현재 날짜와 시간을 받아 온다.
	hmdf::DateTime    new_date;

	std::string product_code = _SymbolCode.substr(0, 3);
	bool is_kospi200 = false;
	if (product_code.compare("101") == 0) is_kospi200 = true;

	if (new_date.is_sunday()) return;
	if (new_date.is_saturday() && new_date.hour() > 6) return;
	if (new_date.is_monday() && new_date.hour() < 7) return;
	if (new_date.hour() == 6 && (new_date.minute() > 0 || new_date.minute() <= 59)) return;

	if (is_kospi200 && new_date.hour() < 9) return;
	if (is_kospi200 && new_date.hour() == 15 && new_date.minute() > 45) return;
	if (is_kospi200 && new_date.hour() >= 16) return;

	int next_hour = new_date.hour();
	int next_min = new_date.minute() + _Cycle;
	if (next_min >= 60) {
		next_hour = next_hour + 1;
		next_min = 60 - next_min;
	}

	std::string time = SmUtil::Format("%02d%02d", next_hour, next_min);
	std::string date = VtStringUtil::getCurentDate();
	time.append("00");

	int msec = abs(new_date.msec()) % 1000;
	CString msg;
	msg.Format("date = %d, hour = %d, min = %d, sec = %d, msec =%d\n", new_date.date(), new_date.hour(), new_date.minute(), new_date.sec(), msec);
	//TRACE(msg);

	// 이미 차트에 맞게 변환된 값이므로 변환하지 않는다. 
	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	
	col_close.append("close");
	double close = _DataFrame.get_column<double>(col_close.c_str())[_DataFrame.shape().first -1];
	AddChartData(close, close, close, close, 0, std::stoi(date), std::stoi(time), msec);
}

void SmChartData::UpdateMa()
{
	
}

void SmChartData::UpdatHeikenAsh()
{
	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	std::string col_ha_high(prefix), col_ha_close(prefix), col_ha_low(prefix), col_ha_open(prefix);
	col_ha_high.append("ha_high");
	col_ha_close.append("ha_close");
	col_ha_low.append("ha_low");
	col_ha_open.append("ha_open");

	std::string col_ha2_high(prefix), col_ha2_close(prefix), col_ha2_low(prefix), col_ha2_open(prefix);
	col_ha2_high.append("ha2_high");
	col_ha2_close.append("ha2_close");
	col_ha2_low.append("ha2_low");
	col_ha2_open.append("ha2_open");

	auto& high = _DataFrame.get_column<double>(col_high.c_str());
	auto& low = _DataFrame.get_column<double>(col_low.c_str());
	auto& open = _DataFrame.get_column<double>(col_open.c_str());
	auto& close = _DataFrame.get_column<double>(col_close.c_str());
	auto& ha_high = _DataFrame.get_column<double>(col_ha_high.c_str());
	auto& ha_close = _DataFrame.get_column<double>(col_ha_close.c_str());
	auto& ha_low = _DataFrame.get_column<double>(col_ha_low.c_str());
	auto& ha_open = _DataFrame.get_column<double>(col_ha_open.c_str());
	size_t last_index = _DataFrame.shape().first - 1;
	ha_close[last_index] = (open[last_index] + high[last_index] + low[last_index] + close[last_index]) / 4.0;
	ha_high[last_index] = (std::max)({ ha_open[last_index], ha_close[last_index], high[last_index]});
	ha_low[last_index] = (std::min)({ ha_open[last_index], ha_close[last_index], low[last_index]});

	int outBegIdx = 0, outNbElement = 0;
	std::vector<double> mahigh(1);
	std::vector<double> malow(1);
	std::vector<double> maopen(1);
	std::vector<double> maclose(1);

	int retCode = TA_MA(last_index, last_index,
		high.data(),
		heiken_ashi_smooth_period, TA_MAType_SMA,
		&outBegIdx, &outNbElement, mahigh.data());
	retCode = TA_MA(last_index, last_index,
		low.data(),
		heiken_ashi_smooth_period, TA_MAType_SMA,
		&outBegIdx, &outNbElement, malow.data());
	retCode = TA_MA(last_index, last_index,
		open.data(),
		heiken_ashi_smooth_period, TA_MAType_SMA,
		&outBegIdx, &outNbElement, maopen.data());
	retCode = TA_MA(last_index, last_index,
		close.data(),
		heiken_ashi_smooth_period, TA_MAType_SMA,
		&outBegIdx, &outNbElement, maclose.data());

	auto& ha2_high = _DataFrame.get_column<double>(col_ha2_high.c_str());
	auto& ha2_close = _DataFrame.get_column<double>(col_ha2_close.c_str());
	auto& ha2_low = _DataFrame.get_column<double>(col_ha2_low.c_str());
	auto& ha2_open = _DataFrame.get_column<double>(col_ha2_open.c_str());

	ha2_close[last_index] = (maopen[0] + mahigh[0] + malow[0] + maclose[0]) / 4.0;
	ha2_high[last_index] = (std::max)({ ha2_open[last_index], ha2_close[last_index], mahigh[0]});
	ha2_low[last_index] = (std::min)({ ha2_open[last_index], ha2_close[last_index], malow[0]});
}

void DarkHorse::SmChartData::OnTimer()
{
	// 주기가 60분 이하는 실시간 수집을 한다. 
		// 그보다 크면 서버에 요청을 한다.
	if (_Cycle < 60)
		MakeNewBarByTimer();
	//else
	//	GetCyclicDataFromServer();
}

void DarkHorse::SmChartData::OnTickData(const SmTick& tick_data)
{
	// 차트데이터를 받지 않았다면 처리하지 않는다.
	if (!_Received) return;
	
	auto symbol = mainApp.SymMgr()->FindSymbol(_SymbolCode);
	if (!symbol) return;
	
	// 차트데이터 타입이 틱데이터일 때 처리
	if (_ChartType == SmChartType::TICK) {
		// 틱카운트를 하나 증가시킨다.
		_TickCount++;
		// 한사이클에 도달하면 봉을 새로 만들어 준다.
		if (_TickCount == _Cycle) {
			// 틱카운트를 재설정한다.
			_TickCount = 0;
			// 새로운 봉이 생겼으므로 새로운 봉을 만들어 준다.
			// 이때 새로운 봉은 시작 시간을 기준으로 한다.
			std::string date = VtStringUtil::getCurentDate();
			hmdf::DateTime    local_now;

			int msec = abs(local_now.msec()) % 1000;

			//CString msg;
			//msg.Format("date = %d, hour = %d, min = %d, sec = %d, msec =%d\n", local_now.date(), local_now.hour(), local_now.minute(), local_now.sec(), msec);
			//TRACE(msg);
			double divedend = pow(10, symbol->decimal());
			// 변환되지 않은 상태로 오기 때문에 차트에 맞게 변환을 해준다. 
			const double close = tick_data.close / divedend;
			AddChartData(close, close, close, close, tick_data.qty, std::stoi(date), std::stoi(tick_data.time), msec);
		}
		// 무조건 마지막 데이터를 업데이트 해준다.
		else {
			UpdateLastData(tick_data);
		}
	}
	else if (_ChartType == SmChartType::MIN) { // 분데이터 타입일 때 처리
		// 분데이터는 무조건 업데이트만 한다.
		// 새 분봉 데이터는 자체에서 만들어 진다.
		UpdateLastData(tick_data);
	}
	
}

void DarkHorse::SmChartData::UpdateLastData(const SmTick& tick_data)
{
	// 메인 쓰레드에서 SmCompData 포인터를 없앨때 뻑이 날 수 있다.
	// 따라서 쓰레드 동기화 보호를 해야 한다.
	std::lock_guard<std::mutex> lock_guard(_m);

	if (_DataFrame.shape().first == 0) return;
	auto symbol = mainApp.SymMgr()->FindSymbol(_SymbolCode);
	if (!symbol) return;
	double divedend = pow(10, symbol->decimal());

	const double close = tick_data.close / divedend;

	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	if (close > _DataFrame.get_column<double>(col_high.c_str())[GetChartDataCount() - 1])
		_DataFrame.get_column<double>(col_high.c_str())[GetChartDataCount() - 1] = close;
	if (close < _DataFrame.get_column<double>(col_low.c_str())[GetChartDataCount() - 1])
		_DataFrame.get_column<double>(col_low.c_str())[GetChartDataCount() - 1] = close;
	_DataFrame.get_column<double>(col_close.c_str())[GetChartDataCount() - 1] = close;
	_DataFrame.get_column<double>(col_volume.c_str())[GetChartDataCount() - 1] += tick_data.qty;

	auto& c_v = _DataFrame.get_column<double>(col_close.c_str());
	int startIdx = c_v.size() - 1, endIdx = c_v.size() - 1;
	int outBegIdx = 0, outNbElement = 0;
	std::vector<double> m20(1);
	std::vector<double> m40(1);
	std::vector<double> m60(1);
	std::vector<double> m120(1);

	int retCode = TA_MA(startIdx, endIdx,
		c_v.data(),
		20, TA_MAType_SMA,
		&outBegIdx, &outNbElement, m20.data());



	retCode = TA_MA(startIdx, endIdx,
		c_v.data(),
		40, TA_MAType_SMA,
		&outBegIdx, &outNbElement, m40.data());


	retCode = TA_MA(startIdx, endIdx,
		c_v.data(),
		60, TA_MAType_SMA,
		&outBegIdx, &outNbElement, m60.data());


	retCode = TA_MA(startIdx, endIdx,
		c_v.data(),
		120, TA_MAType_SMA,
		&outBegIdx, &outNbElement, m120.data());


	std::string col_ma20(prefix), col_ma40(prefix), col_ma60(prefix), col_ma120(prefix), col_point_type(prefix);
	col_ma20.append("ma20");
	col_ma40.append("ma40");
	col_ma60.append("ma60");
	col_ma120.append("ma120");
	col_point_type.append("point_type");

	auto& ma20 = _DataFrame.get_column<double>(col_ma20.c_str());
	auto& ma40 = _DataFrame.get_column<double>(col_ma40.c_str());
	auto& ma60 = _DataFrame.get_column<double>(col_ma60.c_str());
	auto& ma120 = _DataFrame.get_column<double>(col_ma120.c_str());

	auto& point_type = _DataFrame.get_column<int>(col_point_type.c_str());

	point_type.data()[point_type.size() - 1] = GetMultiColorValue(close, m20[0], m40[0], m60[0], m120[0]);

	ma20.data()[ma20.size() - 1] = m20[0];

	ma40.data()[ma40.size() - 1] = m40[0];

	ma60.data()[ma60.size() - 1] = m60[0];

	ma120.data()[ma120.size() - 1] = m120[0];

	UpdatHeikenAsh();

	ExternalStorage.UpdateData(close);
	for (auto it = _CompSet.begin(); it != _CompSet.end(); it++) {
		(*it)->UpdateChartData(_SymbolCode, close);
	}

	mainApp.CallbackMgr()->OnChartEvent(shared_from_this(), 1);
}

void DarkHorse::SmChartData::GetLastData(SmStockData& last_data)
{
	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");
	col_volume.append("volume");

	const size_t last_index = GetChartDataCount() - 1;
	last_data.date_time = _DataFrame.get_index()[last_index];
	last_data.open = _DataFrame.get_column<double>(col_open.c_str())[last_index];
	last_data.high = _DataFrame.get_column<double>(col_high.c_str())[last_index];
	last_data.low = _DataFrame.get_column<double>(col_low.c_str())[last_index];
	last_data.close = _DataFrame.get_column<double>(col_close.c_str())[last_index];
	last_data.volume = _DataFrame.get_column<double>(col_volume.c_str())[last_index];
}

/// <summary>
/// 차트타입 약자 : DA : day, WE : week, MO : month, HO : hour, MI : minute, TI : Tick, Year : YE
/// 차트데이터 칼럼 이름 붙이기 규칙 : 차트타입(2자)_(사이클)_데이터이름
/// 실례) 120틱 종가 : TI_120_C
/// 1분 고가 : MI_1_H
/// 일데이터 저가 : DA_1_L
std::string SmChartData::MakePrefix()
{
	std::string prefix(_ProductCode);
	prefix.append("_");
	switch (_ChartType)
	{
	case DarkHorse::NONE:
		prefix.append("MI");
		break;
	case DarkHorse::TICK:
		prefix.append("TI");
		break;
	case DarkHorse::MIN:
		prefix.append("MI");
		break;
	case DarkHorse::DAY:
		prefix.append("DA");
		break;
	case DarkHorse::WEEK:
		prefix.append("WE");
		break;
	case DarkHorse::MON:
		prefix.append("MO");
		break;
	case DarkHorse::YEAR:
		prefix.append("YE");
		break;
	default:
		break;
	}
	prefix.append(std::format("_{0:0>3}_", _Cycle));

	return prefix;
}

void SmChartData::JoinChartData(const SmChartType& src_chart_type, const int& src_cycle)
{
	const std::string& data_key1 = SmChartDataManager::MakeDataKey(_SymbolCode, src_chart_type, src_cycle);
	std::shared_ptr<SmChartData> chart_data1 = mainApp.ChartDataMgr()->FindChartData(data_key1);

	
	hmdf::StdDataFrame<hmdf::DateTime> join_df;

	join_df = GetDataFrame().join_by_index<decltype(join_df), double>(chart_data1->GetDataFrame(), hmdf::join_policy::left_right_join);


	auto& first_index = chart_data1->GetDataFrame().get_index();
	for (auto it = first_index.begin(); it != first_index.end(); ++it) {
		auto cur_dt = *it;
		CString msg;
		msg.Format("h:m:s = %d:%d:%d \n", cur_dt.hour(), cur_dt.minute(), cur_dt.sec());
		TRACE(msg);
	}

	auto& nq_index_v = join_df.get_index();
	for (auto it = nq_index_v.begin(); it != nq_index_v.end(); ++it) {
		auto cur_dt = *it;
		CString msg;
		msg.Format("h:m:s = %d:%d:%d \n", cur_dt.hour(), cur_dt.minute(), cur_dt.sec());
		TRACE(msg);
	}
	

	/*
	std::cout << "\nTesting Index Left Right Join ..." << std::endl;

	std::vector<unsigned long>  idx =
	{ 123466, 123451, 123452, 123453, 123454, 123455, 123456, 123457, 123458, 123459, 123460, 123461, 123462, 123450 };
	std::vector<double> d1 = { 14, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 1 };
	std::vector<double> d2 = { 1.89, 9, 10, 11, 12, 13, 14, 20, 22, 23, 30, 31, 32, 8 };
	std::vector<double> d3 = { 19.0, 16, 15, 18, 19, 16, 21, 0.34, 1.56, 0.34, 2.3, 0.34, 15.0 };
	std::vector<int>    i1 = { 22, 23, 24, 25, 99 };
	hmdf::StdDataFrame<unsigned long>         df;

	df.load_data(std::move(idx),
		std::make_pair("col_1", d1),
		std::make_pair("col_2", d2),
		std::make_pair("col_3", d3),
		std::make_pair("col_4", i1));

	std::vector<unsigned long>  idx2 =
	{ 123452, 123453, 123455, 123458, 123466, 223450, 223451, 223454, 223456, 223457, 223459, 223461, 223460, 223462 };
	std::vector<double> d12 = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 110, 111, 113, 112, 114 };
	std::vector<double> d22 = { 18, 19, 110, 111, 112, 113, 114, 120, 122, 123, 130, 132, 131, 11.89 };
	std::vector<double> d32 = { 115, 116, 115, 118, 119, 116, 121, 10.34, 11.56, 10.34, 10.34, 12.3, 119.0 };
	std::vector<int>    i12 = { 122, 123, 124, 125, 199 };
	hmdf::StdDataFrame<unsigned long>         df2;

	df2.load_data(std::move(idx2),
		std::make_pair("col_5", d12),
		std::make_pair("col_6", d22),
		std::make_pair("col_7", d32),
		std::make_pair("col_8", i12));

	std::cout << "First DF:" << std::endl;
	df.write<std::ostream, double, int>(std::cout);
	std::cout << "Second DF2:" << std::endl;
	df2.write<std::ostream, double, int>(std::cout);

	hmdf::StdDataFrame<unsigned long> join_df =
		df.join_by_index<decltype(df2), double, int>(df2, hmdf::join_policy::left_right_join);

	std::cout << "Now The joined DF:" << std::endl;
	join_df.write<std::ostream, double, int>(std::cout);

	auto& col_1 = join_df.get_column<double>("col_1");
	auto& col_2 = join_df.get_column<double>("col_2");
	auto& col_3 = join_df.get_column<double>("col_3");
	auto& col_4 = join_df.get_column<int>("col_4");
	auto& col_5 = join_df.get_column<double>("col_5");
	auto& col_6 = join_df.get_column<double>("col_6");
	auto& col_7 = join_df.get_column<double>("col_7");
	auto& col_8 = join_df.get_column<int>("col_8");
	*/
	//_DataFrame = std::move(join_df);
}

int SmChartData::GetMultiColorValue(const double& close, const double& ma20, const double& ma40, const double& ma60, const double& ma120)
{
	// Gray : 0, LightRed : 1, Red : 2, Blue : 3, LightBlue 4;
	int point_type = 0;
	/*
	if (close > ma120) {
		if (close > ma20 && close > ma40 && close > ma60)
			point_type = 2;
	}

	if (close <= ma120) {
		if (close > ma20 && close > ma40 && close > ma60)
			point_type = 1;
	}

	if (close > ma120) {
		if (close < ma20 && close < ma40 && close < ma60)
			point_type = 4;
	}

	if (close <= 120) {
		if (close < ma20 && close < ma40 && close < ma60)
			point_type = 3;
	}

	if ((close <= ma20 && close >= ma60) ||
		(close <= ma40 && close >= ma60))
		point_type = 0;

	if (close > ma120) {
		if (close >= ma20 && close <= ma60)
			point_type = 0;
	}

	if (close <= ma120) {
		if (close >= ma20 && close <= ma40)
			point_type = 0;
		if (close >= ma40 && close <= ma60)
			point_type = 0;
	}
	*/

	/*
	/// 중립구간
Input : Short(5),Mid(20), Long(60);

Value1 = Average(c, 20);
Value2 = Average(c, 40);
value3 = Average(c, 60);
value4 = Average(c, 120);

//매수후 중립구간

If (c<=value1 and c>=value3 ) or (c<=value2 and c>=Value3) Then
 begin
 PlotPaintBar( High, Low, "상태", GRAY);
 end

//매도후 중립구간

If c>value4 Then {

If (c>=value1 and c<=Value3) Then
 begin
 PlotPaintBar( High, Low, "상태", GRAY);
 end

}


If c<=value4 Then {

If (c>=value1 and c<=Value2) Then
 begin
 PlotPaintBar( High, Low, "상태", GRAY);
 end

If (c>=value2 and c<=Value3) Then
 begin
 PlotPaintBar( High, Low, "상태", GRAY);
 end

}


///////////////////
// 매수구간

//강매수_레드

If c>value4 Then {
If c>value1 and c>value2 and c>Value3 Then begin
 PlotPaintBar( High, Low, "상태",Red);
 end
 }

//약매수_핑크

If c<=value4 Then {
If c>value1 and c>value2 and c>Value3 Then begin
 PlotPaintBar( High, Low, "상태",RGB(255,182,193));
 end
 }


 //////////////////////
//매도구간


If c>value4 Then Begin
if c<value1 and c<value2 and c<Value3 Then begin
 PlotPaintBar( High, Low, "상태", RGB(67, 218, 236));
 end
End;

If c<=value4 Then Begin
If c<value1 and c<value2 and c<Value3 Then begin
 PlotPaintBar( High, Low, "상태",Blue);
 end

	
	*/
	
	// 종가가 120이평보다 클 때
	if (close > ma120) {
		// 종가가 20, 40, 60 이평보다 모두 작을 때 매도
		if (close < ma20 && close < ma40 && close < ma60) {
			// 약매도
			//tempColorArray[i] = ColorUtil.rgb(67, 218, 236);
			point_type = 4;
		}

		// 종가가 20, 40, 60 이평보다 모두 클 때 매수
		if (close > ma20 && close > ma40 && close > ma60) {
			// 강매수
			//tempColorArray[i] = ColorUtil.Red;
			point_type = 2;
		}

		if (close >= ma20 && close <= ma60) {
			//tempColorArray[i] = ColorUtil.Grey;
			point_type = 0;
		}
	}

	// 종가가 120이평보다 작거나 같을 때
	if (close <= ma120) {
		// 종가가 20, 40, 60 이평보다 모두 작을 때 매도
		if (close < ma20 && close < ma40 && close < ma60) {
			// 강력 매도
			//tempColorArray[i] = ColorUtil.Blue;
			point_type = 3;
		}

		// 종가가 20, 40, 60 이평보다 모두 클 때 매수
		if (close > ma20 && close > ma40 && close > ma60) {
			// 약매수
			//tempColorArray[i] = ColorUtil.rgb(255, 182, 193);
			point_type = 1;
		}

		if (close >= ma20 && close <= ma60) {
			//tempColorArray[i] = ColorUtil.Grey;
			point_type = 0;
		}
	}

	if ((close >= ma20 && close <= ma40) ||
		(close >= ma40 && close <= ma60)) {
		//tempColorArray[i] = ColorUtil.Grey;
		point_type = 0;
	}
	

	return point_type;
}

void SmChartData::ProcessMultiColorLine()
{
	std::string prefix = MakePrefix();
	std::string col_ma20(prefix), col_ma40(prefix), col_ma60(prefix), col_ma120(prefix), col_point_type(prefix), col_close(prefix);
	col_close.append("close");
	col_ma20.append("ma20");
	col_ma40.append("ma40");
	col_ma60.append("ma60");
	col_ma120.append("ma120");
	col_point_type.append("point_type");

	auto& close_v = _DataFrame.get_column<double>(col_close.c_str());
	auto& ma20 = _DataFrame.get_column<double>(col_ma20.c_str());
	auto& ma40 = _DataFrame.get_column<double>(col_ma40.c_str());
	auto& ma60 = _DataFrame.get_column<double>(col_ma60.c_str());
	auto& ma120 = _DataFrame.get_column<double>(col_ma120.c_str());
	std::vector<int> point_type(close_v.size());
	// Gray : 0, LightRed : 1, Red : 2, Blue : 3, LightBlue 4;
	for (size_t i = 0; i < close_v.size(); i++) {
		// 이동평균이 적용되지 않을 때는 기본 색상을 적용한다.
		if (i < 119) {
			point_type[i] = 0; // Grey
		}
		else {
			point_type[i] = GetMultiColorValue(close_v[i], ma20[i], ma40[i], ma60[i], ma120[i]);
		}
	}

	_DataFrame.load_column(col_point_type.c_str(), std::move(point_type));
}

void SmChartData::ProcessDataPerBar()
{
	// 명시적으로 선언된 생성자가 있어 모든 매개변수를 생성자에 제공해 줘야 한다. 
	_PnfData = std::make_shared<DarkHorse::SmChartData>(this->SymbolCode(), this->ChartType(), this->Cycle(), DarkHorse::SmChartDataManager::GetId());
	_PnfData->InitDataFrame();
	mainApp.PnfMgr()->MakePnfBar(shared_from_this(), _PnfData, (double)5, 5);

	std::string prefix = MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");

	const auto& high_v = _DataFrame.get_column<double>(col_high.c_str());
	const auto& low_v = _DataFrame.get_column<double>(col_low.c_str());
	const auto& open_v = _DataFrame.get_column<double>(col_open.c_str());
	const auto& close_v = _DataFrame.get_column<double>(col_close.c_str());

	//std::string prefix = MakePrefix();
	std::string col_ma20(prefix), col_ma40(prefix), col_ma60(prefix), col_ma120(prefix), col_point_type(prefix)/*, col_close(prefix)*/;
	//col_close.append("close");
	col_ma20.append("ma20");
	col_ma40.append("ma40");
	col_ma60.append("ma60");
	col_ma120.append("ma120");
	col_point_type.append("point_type");

	//auto& close_v = _DataFrame.get_column<double>(col_close.c_str());
	int data_count = GetChartDataCount();
	int startIdx = 0, endIdx = data_count - 1;
	int outBegIdx = 0, outNbElement = 0;
	std::vector<double> ma20(data_count);
	std::vector<double> ma40(data_count);
	std::vector<double> ma60(data_count);
	std::vector<double> ma120(data_count);

	std::vector<double> test(1);
	int retCode = TA_MA(startIdx, endIdx,
		close_v.data(),
		20, TA_MAType_SMA,
		&outBegIdx, &outNbElement, ma20.data());

	std::rotate(ma20.rbegin(), ma20.rbegin() + outBegIdx, ma20.rend());

	retCode = TA_MA(startIdx, endIdx,
		close_v.data(),
		40, TA_MAType_SMA,
		&outBegIdx, &outNbElement, ma40.data());

	std::rotate(ma40.rbegin(), ma40.rbegin() + outBegIdx, ma40.rend());

	retCode = TA_MA(startIdx, endIdx,
		close_v.data(),
		60, TA_MAType_SMA,
		&outBegIdx, &outNbElement, ma60.data());

	std::rotate(ma60.rbegin(), ma60.rbegin() + outBegIdx, ma60.rend());

	retCode = TA_MA(startIdx, endIdx,
		close_v.data(),
		120, TA_MAType_SMA,
		&outBegIdx, &outNbElement, ma120.data());

	std::rotate(ma120.rbegin(), ma120.rbegin() + outBegIdx, ma120.rend());

	_DataFrame.load_column(col_ma20.c_str(), std::move(ma20));
	_DataFrame.load_column(col_ma40.c_str(), std::move(ma40));
	_DataFrame.load_column(col_ma60.c_str(), std::move(ma60));
	_DataFrame.load_column(col_ma120.c_str(), std::move(ma120));

	std::vector<double> dummy(close_v.size());
	SmArrayMath(dummy).heiken_ashi(_DataFrame, prefix);
	SmArrayMath(dummy).heiken_ashi_smooth(heiken_ashi_smooth_period, _DataFrame, prefix);

	ProcessMultiColorLine();
}
