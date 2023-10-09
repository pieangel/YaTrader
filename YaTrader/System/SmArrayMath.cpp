#include "stdafx.h"
#include "SmArrayMath.h"
#include <chartdir.h>
#include "../TaLib/tapp.h"
#include <format>
#include <string>

using namespace DarkHorse;

SmArrayMath& SmArrayMath::add(const std::vector<double>& another)
{
	for (size_t i = 0; i < _Data.size(); ++i) {
		if (another[i] == Chart::NoValue) {
			_Data[i] = Chart::NoValue;
			continue;
		}
		_Data[i] += another[i];
	}
	return *this;
}

SmArrayMath& SmArrayMath::add(const double& value)
{
	for (size_t i = 0; i < _Data.size(); ++i) {
		if (_Data[i] == Chart::NoValue) {
			continue;
		}
		_Data[i] += value;
	}
	return *this;
}

SmArrayMath& SmArrayMath::sub(const std::vector<double>& another)
{
	for (size_t i = 0; i < _Data.size(); ++i) {
		if (another[i] == Chart::NoValue) {
			_Data[i] = Chart::NoValue;
			continue;
		}
		_Data[i] -= another[i];
	}
	return *this;
}

SmArrayMath& SmArrayMath::sub(const double& value)
{
	for (size_t i = 0; i < _Data.size(); ++i) {
		if (_Data[i] == Chart::NoValue) {
			continue;
		}
		_Data[i] -= value;
	}
	return *this;
}

SmArrayMath& SmArrayMath::mul(const std::vector<double>& another)
{
	for (size_t i = 0; i < _Data.size(); ++i) {
		if (another[i] == Chart::NoValue) {
			_Data[i] = Chart::NoValue;
			continue;
		}
		_Data[i] *= another[i];
	}
	return *this;
}

SmArrayMath& SmArrayMath::mul(const double& value)
{
	for (size_t i = 0; i < _Data.size(); ++i) {
		if (_Data[i] == Chart::NoValue) {
			continue;
		}
		_Data[i] *= value;
	}
	return *this;
}

SmArrayMath& SmArrayMath::div(const std::vector<double>& another)
{
	for (size_t i = 0; i < _Data.size(); ++i) {
		if (another[i] == 0 || another[i] == Chart::NoValue)
			_Data[i] = Chart::NoValue;
		else
			_Data[i] /= another[i];
	}
	return *this;
}

SmArrayMath& SmArrayMath::div(const double& value)
{
	if (value == 0.0)
		return *this;

	for (size_t i = 0; i < _Data.size(); ++i) {
		if (_Data[i] == Chart::NoValue) {
			continue;
		}
		double div_val = _Data[i] / value;
		_Data[i] = div_val;
	}
	return *this;
}

SmArrayMath& SmArrayMath::shift_left(const int offset, const double fill_value)
{
	memmove(_Data.data(), _Data.data() + offset, sizeof(double) * (_Data.size() - offset));
	for (int i = (int)(_Data.size() - offset); i < (int)_Data.size(); ++i) {
		_Data[i] = fill_value;
	}
	return *this;
}

SmArrayMath& SmArrayMath::shift_right(const int offset, const double fill_value)
{
	memmove(_Data.data() + offset, _Data.data(), sizeof(double) * (_Data.size() - offset));
	for (int i = 0; i < offset; ++i) {
		_Data[i] = fill_value;
	}
	return *this;
}

SmArrayMath& SmArrayMath::delta(const int offset)
{
	for (int i = (int)(_Data.size() - 1); i >= 0; --i) {
		// 인덱스 끝에 이르렀을 때 혹은 차트 값이 정의되지 않으면 
		// 정의 되지 않은 값을 넣어 준다.
		if (i - offset < 0 || _Data[i] == Chart::NoValue)
			_Data[i] = Chart::NoValue;
		else
			_Data[i] = _Data[i] - _Data[i - offset];
	}
	return *this;
}

SmArrayMath& SmArrayMath::delta_backward(const int offset)
{
	for (int i = (int)(_Data.size() - 1); i >= 0; --i) {
		// 인덱스 끝에 이르렀을 때 혹은 차트 값이 정의되지 않으면 
		// 정의 되지 않은 값을 넣어 준다.
		if (i - offset < 0 || _Data[i] == Chart::NoValue)
			_Data[i] = Chart::NoValue;
		else {
			_Data[i] = _Data[i] - _Data[i - offset];
		}
	}

	for (size_t i = 0; i < _Data.size(); i++) {
		if (_Data[i] == Chart::NoValue)
			continue;
		if (_Data[i] == 0.0) {
			_Data[i] = _Data[i - 1];
		}
	}

	return *this;
}

SmArrayMath& SmArrayMath::delta_backward(const int start_index, const int end_index, const int offset, std::vector<double>& out)
{
	if (start_index > end_index)
		return *this;

	int start = start_index, end = end_index;
	if (start - offset < 0)
		start = 0;
	if (end >= (int)_Data.size())
		end = (int)(_Data.size() - 1);
	for (int i = start; i <= end; ++i) {
		// 인덱스 끝에 이르렀을 때 혹은 차트 값이 정의되지 않으면 
		// 정의 되지 않은 값을 넣어 준다.
		if (i - offset < 0 || _Data[i] == Chart::NoValue)
			out.push_back(Chart::NoValue);
		else {
			out.push_back(_Data[i] - _Data[i - offset]);
		}
	}

	for (size_t i = 0; i < out.size(); i++) {
		if (out[i] == Chart::NoValue)
			continue;
		if (out[i] == 0.0) {
			out[i] = out[i - 1];
		}
	}

	return *this;
}

SmArrayMath& SmArrayMath::rate(const int offset)
{
	for (int i = (int)(_Data.size() - 1); i >= 0; --i) {
		if (i - offset < 0 || _Data[i] == Chart::NoValue)
			_Data[i] = Chart::NoValue;
		else
			_Data[i] = _Data[i] / _Data[i - offset];
	}
	return *this;
}

SmArrayMath& SmArrayMath::abs()
{
	for (size_t i = 0; i < _Data.size(); ++i) {
		if (_Data[i] == Chart::NoValue) {
			continue;
		}
		_Data[i] = std::abs(_Data[i]);
	}
	return *this;
}

SmArrayMath& SmArrayMath::acc()
{
	for (size_t i = 1; i < _Data.size(); ++i) {
		_Data[i] += _Data[i - 1];
	}
	return *this;
}

SmArrayMath& SmArrayMath::trim(const int start_index, const int len /*= -1*/)
{
	if (start_index < 0 || start_index + len >= (int)_Data.size())
		return *this;

	auto first = _Data.cbegin() + start_index;
	auto last = _Data.cbegin() + start_index + len;
	if (len < 0)
		last = _Data.cend();

	std::vector<double> vec(first, last);
	_Data = vec;
	return *this;
}

SmArrayMath& SmArrayMath::insert(const std::vector<double> another, const int insert_point /*= -1*/)
{
	if (insert_point < 0)
		_Data.insert(_Data.end(), another.begin(), another.end());
	else
		_Data.insert(std::next(_Data.begin(), insert_point), another.begin(), another.end());
	return *this;
}

SmArrayMath& SmArrayMath::insert2(const std::vector<double> another, const int len, const int insert_point /*= -1*/)
{
	if (another.end() == another.begin() + len)
		return *this;

	if (insert_point < 0)
		_Data.insert(_Data.end(), another.begin(), another.end());
	else
		_Data.insert(std::next(_Data.begin(), insert_point), another.begin(), another.begin() + len);
	return *this;
}

SmArrayMath& SmArrayMath::replace(const double to_be_replaced, const double new_value)
{
	for (size_t i = 0; i < _Data.size(); ++i) {
		if (_Data[i] == to_be_replaced)
			_Data[i] = new_value;
	}

	return *this;
}

SmArrayMath& SmArrayMath::mov_avg(const int& period, hmdf::StdDataFrame<hmdf::DateTime>& df)
{
	std::string col_name = std::format("ma_{0}", period);
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_SMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	df.load_column(col_name.c_str(), std::move(out));

	return *this;
}

SmArrayMath& SmArrayMath::mov_avg(const int start_index, const int period)
{
	const int startIdx = start_index + period - 1, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_SMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + start_index + period - 1);

	_Data = out;

	return *this;
}

SmArrayMath& SmArrayMath::mov_avg(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_SMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	// + (endIdx - startIdx - outNbElement )
	return *this;
}

SmArrayMath& SmArrayMath::mov_avg(const int& period, const std::string& col_name, hmdf::StdDataFrame<hmdf::DateTime>& df)
{
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_SMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	df.load_column(col_name.c_str(), std::move(out));

	return *this;
}

DarkHorse::SmArrayMath& SmArrayMath::mov_avg(const int& period)
{
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_SMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);
	_Data = std::move(out);
	return *this;
}

SmArrayMath& SmArrayMath::exp_avg(const int period)
{
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_EMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;

	return *this;
}

SmArrayMath& SmArrayMath::exp_avg(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_EMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::kama_avg(const int period)
{
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_KAMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;

	return *this;
}

SmArrayMath& SmArrayMath::kama_avg(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_KAMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::t3_avg(const int period, const double vf)
{
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_T3(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		vf,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;

	return *this;
}

SmArrayMath& SmArrayMath::t3_avg(const int start_index, const int end_index, const int period, double vf, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_T3(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		vf,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::tema_avg(const int period)
{
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_TEMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;

	return *this;
}

SmArrayMath& SmArrayMath::tema_avg(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_TEMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::mama_avg(const int period, const double SlowLimit)
{
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	std::vector<double> out2(_Data.size(), Chart::NoValue);
	int retCode = TA_MAMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		SlowLimit,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1,
		out2.data() + period - 1);

	_Data = out;

	return *this;
}

SmArrayMath& SmArrayMath::mama_avg(const int start_index, const int end_index, const int period, const double SlowLimit, std::vector<double>& mama, std::vector<double>& fmama)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	mama.resize(endIdx - start_index + 1, Chart::NoValue);
	fmama.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_MAMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		SlowLimit,
		&outBegIdx,
		&outNbElement,
		mama.data(),
		fmama.data());
	return *this;
}

SmArrayMath& SmArrayMath::trima_avg(const int period)
{
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_TRIMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;

	return *this;
}

SmArrayMath& SmArrayMath::trima_avg(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_TRIMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::wma_avg(const int period)
{
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_WMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;

	return *this;
}

SmArrayMath& SmArrayMath::wma_avg(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_WMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::sma_avg(const int period)
{
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_SMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;

	return *this;
}

SmArrayMath& SmArrayMath::sma_avg(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_SMA(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::mov_med(const int period)
{
	return *this;
}

SmArrayMath& SmArrayMath::mov_corr(const int period, const std::vector<double>& another)
{
	int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_CORREL(
		startIdx,
		endIdx,
		_Data.data(),
		another.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;

	return *this;
}

SmArrayMath& SmArrayMath::mov_corr(const int start_index, const int end_index, const int period, const std::vector<double>& another, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_CORREL(
		startIdx,
		endIdx,
		_Data.data(),
		another.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::mov_min(const int period)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_MIN(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = std::move(out);
	return *this;
}

SmArrayMath& SmArrayMath::mov_min(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_MIN(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

DarkHorse::SmArrayMath& SmArrayMath::mov_min(const int period, hmdf::StdDataFrame<hmdf::DateTime>& df, const std::string& col_name)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_MIN(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);
	df.load_column(col_name.c_str(), std::move(out));

	return *this;
}

SmArrayMath& SmArrayMath::mov_max(const int period)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_MAX(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = std::move(out);
	return *this;
}

SmArrayMath& SmArrayMath::mov_max(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_MAX(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

DarkHorse::SmArrayMath& SmArrayMath::mov_max(const int period, hmdf::StdDataFrame<hmdf::DateTime>& df, const std::string& col_name)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_MAX(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	df.load_column(col_name.c_str(), std::move(out));
	return *this;
}

SmArrayMath& SmArrayMath::mov_std(const int period, const double deviation)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_STDDEV(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		deviation,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;
	return *this;
}

SmArrayMath& SmArrayMath::mov_std(const int start_index, const int end_index, const int period, const double deviation, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_STDDEV(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		deviation,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::mov_sum(const int period)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_SUM(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;
	return *this;
}

SmArrayMath& SmArrayMath::mov_sum(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_SUM(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::tsf(const int period)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_TSF(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;
	return *this;
}

SmArrayMath& SmArrayMath::tsf(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_TSF(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::ht_sine()
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	std::vector<double> out2(_Data.size(), Chart::NoValue);
	int retCode = TA_HT_SINE(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		out.data() + 63,
		out2.data() + 63);

	//_Sine = out;
	//_LeadSine = out2;
	return *this;
}

SmArrayMath& SmArrayMath::ht_sine(const int start_index, const int end_index, std::vector<double>& sine, std::vector<double>& lead_sine)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	sine.resize(endIdx - start_index + 1, Chart::NoValue);
	lead_sine.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_HT_SINE(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		sine.data(),
		lead_sine.data());
	return *this;
}

SmArrayMath& SmArrayMath::ht_trend_line()
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_HT_TRENDLINE(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		out.data() + 63);

	_Data = out;
	return *this;
}

SmArrayMath& SmArrayMath::ht_trend_line(const int start_index, const int end_index, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_HT_TRENDLINE(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::ht_trendmode()
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<int> out(_Data.size(), 0);
	int retCode = TA_HT_TRENDMODE(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		out.data() + 63);

	//_TrendMode = out;
	return *this;
}

SmArrayMath& SmArrayMath::ht_trendmode(const int start_index, const int end_index, std::vector<int>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, 0);
	int retCode = TA_HT_TRENDMODE(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}


SmArrayMath& SmArrayMath::ht_phasor()
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	std::vector<double> out2(_Data.size(), Chart::NoValue);
	int retCode = TA_HT_PHASOR(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		out.data() + 32,
		out2.data() + 32);

	//_Phase = out;
	//_Quadrature = out2;
	return *this;
}


SmArrayMath& SmArrayMath::ht_phasor(const int start_index, const int end_index, std::vector<double>& phase, std::vector<double>& quadrature)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	phase.resize(endIdx - start_index + 1, Chart::NoValue);
	quadrature.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_HT_PHASOR(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		phase.data(),
		quadrature.data());
	return *this;
}


SmArrayMath& SmArrayMath::ht_dcphase()
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_HT_DCPHASE(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		out.data() + 63);

	_Data = out;
	return *this;
}

SmArrayMath& SmArrayMath::ht_dcphase(const int start_index, const int end_index, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_HT_DCPHASE(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::ht_dcperiod()
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_HT_DCPERIOD(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		out.data() + 32);

	_Data = out;
	return *this;
}

SmArrayMath& SmArrayMath::ht_dcperiod(const int start_index, const int end_index, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_HT_DCPERIOD(
		startIdx,
		endIdx,
		_Data.data(),
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::ht_linear_reg_angle(const int period)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_LINEARREG_ANGLE(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	_Data = out;
	return *this;
}

SmArrayMath& SmArrayMath::ht_linear_reg_angle(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_LINEARREG_ANGLE(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::atr(const int period)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

// 	std::vector<double> out(_Data.size(), Chart::NoValue);
// 	int retCode = TA_ATR(
// 		startIdx,
// 		endIdx,
// 		_High.data(),
// 		_Low.data(),
// 		_Data.data(),
// 		period,
// 		&outBegIdx,
// 		&outNbElement,
// 		out.data() + period);
// 
// 	_Data = out;
	return *this;
}


SmArrayMath& SmArrayMath::atr(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

// 	out.resize(endIdx - start_index + 1, Chart::NoValue);
// 	int retCode = TA_ATR(
// 		startIdx,
// 		endIdx,
// 		_High.data(),
// 		_Low.data(),
// 		_Data.data(),
// 		period,
// 		&outBegIdx,
// 		&outNbElement,
// 		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::avg_price(const std::vector<double>& open, const std::vector<double>& high, const std::vector<double>& low, const std::vector<double>& close)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_AVGPRICE(
		startIdx,
		endIdx,
		open.data(),
		high.data(),
		low.data(),
		close.data(),
		&outBegIdx,
		&outNbElement,
		out.data());

	_Data = out;
	return *this;
}

SmArrayMath& SmArrayMath::avg_price(const int start_index, const int end_index, const std::vector<double>& open, const std::vector<double>& high, const std::vector<double>& low, const std::vector<double>& close, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_AVGPRICE(
		startIdx,
		endIdx,
		open.data(),
		high.data(),
		low.data(),
		close.data(),
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::med_price(const std::vector<double>& high, const std::vector<double>& low)
{
	const int startIdx = 0, endIdx = (int)high.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	int retCode = TA_MEDPRICE(
		startIdx,
		endIdx,
		high.data(),
		low.data(),
		&outBegIdx,
		&outNbElement,
		_Data.data());
	return *this;
}

SmArrayMath& SmArrayMath::med_price(const int start_index, const int end_index, const std::vector<double>& high, const std::vector<double>& low, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_MEDPRICE(
		startIdx,
		endIdx,
		high.data(),
		low.data(),
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::max_index(const int period)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<int> out(_Data.size(), -1);
	int retCode = TA_MAXINDEX(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());

	std::transform(out.begin(), out.end(), _Data.begin(), [](int x) { return (double)x; });
	return *this;
}

int SmArrayMath::max_index(const std::vector<double> data, const int start, const int end)
{
	auto it = std::max_element(data.begin() + start, data.begin() + end);
	int index = std::distance(data.begin(), it);
	return index;
}

SmArrayMath& SmArrayMath::max_index(const int start_index, const int end_index, const int period, std::vector<int>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(_Data.size(), -1);
	int retCode = TA_MAXINDEX(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::min_index(const int period)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<int> out(_Data.size(), -1);
	int retCode = TA_MININDEX(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());

	std::transform(out.begin(), out.end(), _Data.begin(), [](int x) { return (double)x; });
	return *this;
}

SmArrayMath& SmArrayMath::min_index(const int start_index, const int end_index, const int period, std::vector<int>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(_Data.size(), -1);
	int retCode = TA_MININDEX(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

int SmArrayMath::min_index(const std::vector<double> data, const int start, const int end)
{
	auto it = std::min_element(data.begin() + start, data.begin() + end);
	int index = std::distance(data.begin(), it);
	return index;
}

int SmArrayMath::min_delta_index(const std::vector<double> data, const double source)
{
	int min_index = -1;
	if (data.size() == 0)
		return min_index;

	double delta = data[0] - source;
	for (size_t i = 1; i < data.size(); ++i) {
		if (std::abs(data[i] - source) < delta) {
			min_index = i;
		}
	}

	return min_index;
}


SmArrayMath& SmArrayMath::lowess1(const double& fraction, const int& step, hmdf::StdDataFrame<hmdf::DateTime>& df)
{
	std::vector<double> lowess_out(_Data.size(), Chart::NoValue);
	std::vector<double> origin(_Data.size(), Chart::NoValue);
	std::shared_ptr<XYChart> chart = std::make_shared<XYChart>(1, 1);
	DoubleArray closeData = DoubleArray(_Data.data(), _Data.size());
	SplineLayer* layer = chart->addSplineLayer(ArrayMath(closeData).lowess(fraction, step), 0xff0000);
	DataSet* dataset = layer->getDataSetByZ(0);
	for (size_t i = 0; i < _Data.size(); ++i) {
		origin[i] =dataset->getValue(i);
	}

	closeData = DoubleArray(origin.data(), origin.size());
	SplineLayer* layer2 = chart->addSplineLayer(ArrayMath(closeData).lowess(fraction, step), 0xff0000);
	dataset = layer2->getDataSetByZ(0);
	for (size_t i = 0; i < _Data.size(); ++i) {
		lowess_out[i] = dataset->getValue(i);
	}

	df.load_column("lowess1", std::move(lowess_out));

	return *this;
}

DarkHorse::SmArrayMath& SmArrayMath::lowess1(const double& fraction, const int& step, hmdf::StdDataFrame<hmdf::DateTime>& df, const std::string& col_name)
{
	std::vector<double> lowess_out(_Data.size(), Chart::NoValue);
	std::vector<double> origin(_Data.size(), Chart::NoValue);
	std::shared_ptr<XYChart> chart = std::make_shared<XYChart>(1, 1);
	DoubleArray closeData = DoubleArray(_Data.data(), _Data.size());
	SplineLayer* layer = chart->addSplineLayer(ArrayMath(closeData).lowess(fraction, step), 0xff0000);
	DataSet* dataset = layer->getDataSetByZ(0);
	for (size_t i = 0; i < _Data.size(); ++i) {
		origin[i] = dataset->getValue(i);
	}

	closeData = DoubleArray(origin.data(), origin.size());
	SplineLayer* layer2 = chart->addSplineLayer(ArrayMath(closeData).lowess(fraction, step), 0xff0000);
	dataset = layer2->getDataSetByZ(0);
	for (size_t i = 0; i < _Data.size(); ++i) {
		lowess_out[i] = dataset->getValue(i);
	}

	df.load_column(col_name.c_str(), std::move(lowess_out));

	return *this;
}

DarkHorse::SmArrayMath& SmArrayMath::lowess2(const double& fraction, const int& step, hmdf::StdDataFrame<hmdf::DateTime>& df)
{
	std::vector<double> lowess_out(_Data.size(), Chart::NoValue);
	std::vector<double> origin(_Data.size(), Chart::NoValue);

	hmdf::LowessVisitor<double>   l_v;

	df.single_act_visit<double, double>("cv_dt", "close", l_v);

	lowess_out = l_v.get_result();
	df.load_column("lowess2", std::move(lowess_out));

	return *this;
}

DarkHorse::SmArrayMath& SmArrayMath::lowess2(const double& fraction, const int& step, hmdf::StdDataFrame<hmdf::DateTime>& df, const std::string& col_name)
{
	std::vector<double> lowess_out(_Data.size(), Chart::NoValue);
	std::vector<double> origin(_Data.size(), Chart::NoValue);

	hmdf::LowessVisitor<double>   l_v;

	df.single_act_visit<double, double>("cv_dt", "close", l_v);

	lowess_out = l_v.get_result();
	df.load_column(col_name.c_str(), std::move(lowess_out));

	return *this;
}

SmArrayMath& SmArrayMath::curve_fit(const int type, const int poly_num)
{
// 	_CurveFit.clear();
// 
// 	DoubleArray closeData = DoubleArray(_Data.data(), _Data.size());
// 	std::shared_ptr<XYChart> chart = std::make_shared<XYChart>(1, 1);
// 	TrendLayer* layer = chart->addTrendLayer(DoubleArray(closeData), 0x00aa00);
// 	if (type == 0)
// 		layer->setRegressionType(Chart::LinearRegression);
// 	else if (type == 1)
// 		layer->setRegressionType(Chart::ConstrainedLinearRegression);
// 	else if (type == 2)
// 		layer->setRegressionType(Chart::ExponentialRegression);
// 	else if (type == 3)
// 		layer->setRegressionType(Chart::LogarithmicRegression);
// 	else
// 		layer->setRegressionType(Chart::PolynomialRegression(poly_num));
// 
// 	DataSet* dataset = layer->getDataSetByZ(0);
// 	for (size_t i = 0; i < _Data.size(); ++i) {
// 		_CurveFit.push_back(dataset->getValue(i));
// 	}

	return *this;
}

SmArrayMath& SmArrayMath::curve_fit(const int start_index, const int end_index, const int type, const int poly_num)
{
// 	_CurveFit.clear();
// 	const int duration = end_index - start_index + 1;
// 	DoubleArray closeData = DoubleArray(_Data.data() + start_index, duration);
// 	std::shared_ptr<XYChart> chart = std::make_shared<XYChart>(1, 1);
// 	TrendLayer* layer = chart->addTrendLayer(DoubleArray(closeData), 0x00aa00);
// 	if (type == 0)
// 		layer->setRegressionType(Chart::LinearRegression);
// 	else if (type == 1)
// 		layer->setRegressionType(Chart::ConstrainedLinearRegression);
// 	else if (type == 2)
// 		layer->setRegressionType(Chart::ExponentialRegression);
// 	else if (type == 3)
// 		layer->setRegressionType(Chart::LogarithmicRegression);
// 	else
// 		layer->setRegressionType(Chart::PolynomialRegression(poly_num));
// 
// 	DataSet* dataset = layer->getDataSetByZ(0);
// 	for (size_t i = 0; i < _Data.size(); ++i) {
// 		_CurveFit.push_back(dataset->getValue(i));
// 	}

	return *this;
}

SmArrayMath& SmArrayMath::curve_fit(const int type, const int poly_num, std::vector<double>& out)
{
	DoubleArray closeData = DoubleArray(_Data.data(), _Data.size());
	std::shared_ptr<XYChart> chart = std::make_shared<XYChart>(1, 1);
	TrendLayer* layer = chart->addTrendLayer(DoubleArray(closeData), 0x00aa00);
	if (type == 0)
		layer->setRegressionType(Chart::LinearRegression);
	else if (type == 1)
		layer->setRegressionType(Chart::ConstrainedLinearRegression);
	else if (type == 2)
		layer->setRegressionType(Chart::ExponentialRegression);
	else if (type == 3)
		layer->setRegressionType(Chart::LogarithmicRegression);
	else
		layer->setRegressionType(Chart::PolynomialRegression(poly_num));

	DataSet* dataset = layer->getDataSetByZ(0);
	for (size_t i = 0; i < _Data.size(); ++i) {
		out.push_back(dataset->getValue(i));
	}

	return *this;
}

SmArrayMath& SmArrayMath::curve_fit(const int start_index, const int end_index, const int type, const int poly_num, std::vector<double>& out)
{
	const int duration = end_index - start_index + 1;
	DoubleArray closeData = DoubleArray(_Data.data() + start_index, duration);
	std::shared_ptr<XYChart> chart = std::make_shared<XYChart>(1, 1);
	TrendLayer* layer = chart->addTrendLayer(DoubleArray(closeData), 0x00aa00);
	if (type == 0)
		layer->setRegressionType(Chart::LinearRegression);
	else if (type == 1)
		layer->setRegressionType(Chart::ConstrainedLinearRegression);
	else if (type == 2)
		layer->setRegressionType(Chart::ExponentialRegression);
	else if (type == 3)
		layer->setRegressionType(Chart::LogarithmicRegression);
	else
		layer->setRegressionType(Chart::PolynomialRegression(poly_num));

	DataSet* dataset = layer->getDataSetByZ(0);
	for (size_t i = 0; i < _Data.size(); ++i) {
		out.push_back(dataset->getValue(i));
	}

	return *this;
}

SmArrayMath& SmArrayMath::lin_reg(const double accuracy, const int half_len)
{
	//SmAlgorithm::SegmentedRegressionFast(_TimeStamp, _Data, accuracy, half_len, _LinearX, _LinearY);
	return *this;
}

SmArrayMath& SmArrayMath::lin_reg(const int period)
{
	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
	int outBegIdx = 0, outNbElement = 0;

	std::vector<double> out(_Data.size(), Chart::NoValue);
	int retCode = TA_LINEARREG(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data() + period - 1);

	//_Out1 = out;
	return *this;
}

SmArrayMath& SmArrayMath::lin_reg(const int start_index, const int end_index, const int period, std::vector<double>& out)
{
	const int startIdx = start_index, endIdx = end_index;
	int outBegIdx = 0, outNbElement = 0;

	out.resize(endIdx - start_index + 1, Chart::NoValue);
	int retCode = TA_LINEARREG(
		startIdx,
		endIdx,
		_Data.data(),
		period,
		&outBegIdx,
		&outNbElement,
		out.data());
	return *this;
}

SmArrayMath& SmArrayMath::trend_line(const int start_index, const int end_index, std::vector<double>& out)
{
	const size_t duration = end_index - start_index + 1;
	std::shared_ptr<XYChart> chart = std::make_shared<XYChart>(1, 1);
	DoubleArray closeData = DoubleArray(_Data.data() + start_index, duration);
	TrendLayer* const layer = chart->addTrendLayer(ArrayMath(closeData));
	DataSet* const dataset = layer->getDataSetByZ(0);
	for (size_t i = 0; i < duration; ++i) {
		out.push_back(dataset->getValue(i));
	}

	return *this;
}

// SmArrayMath& SmArrayMath::macd(const int fast_period, const int slow_period, const int signal_period)
// {
// 	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
// 	int outBegIdx = 0, outNbElement = 0;
// 	_Out1.resize(_Data.size());
// 	_Out2.resize(_Data.size());
// 	_Out3.resize(_Data.size());
// 
// 	int max_period = max(fast_period, slow_period);
// 
// 	int retCode = TA_MACD(
// 		startIdx,
// 		endIdx,
// 		_Data.data(),
// 		fast_period,
// 		slow_period,
// 		signal_period,
// 		&outBegIdx,
// 		&outNbElement,
// 		_Out1.data() + max_period - 1,
// 		_Out2.data() + max_period - 1,
// 		_Out3.data() + max_period - 1);
// 
// 	return *this;
// }

// void SmArrayMath::macd(const int fast_period, const int slow_period, const int signal_period, std::vector<double>& out_macd, std::vector<double>& out_macd_signal, std::vector<double>& out_macd_hist)
// {
// 	const int startIdx = 0, endIdx = (int)_Data.size() - 1;
// 	int outBegIdx = 0, outNbElement = 0;
// 	out_macd.resize(_Data.size(), Chart::NoValue);
// 	out_macd_signal.resize(_Data.size(), Chart::NoValue);
// 	out_macd_hist.resize(_Data.size(), Chart::NoValue);
// 
// 	int max_period = max(fast_period, slow_period);
// 
// 	int retCode = TA_MACD(
// 		startIdx,
// 		endIdx,
// 		_Data.data(),
// 		fast_period,
// 		slow_period,
// 		signal_period,
// 		&outBegIdx,
// 		&outNbElement,
// 		out_macd.data() + max_period - 1,
// 		out_macd_signal.data() + max_period - 1,
// 		out_macd_hist.data() + max_period - 1);
// }

// SmArrayMath& SmArrayMath::macd(const int start_index, const int end_index, const int fast_period, const int slow_period, const int signal_period, std::vector<double>& macd, std::vector<double>& signal, std::vector<double>& hist)
// {
// 	const int startIdx = start_index, endIdx = end_index;
// 	int outBegIdx = 0, outNbElement = 0;
// 	macd.resize(endIdx - start_index + 1, Chart::NoValue);
// 	signal.resize(endIdx - start_index + 1, Chart::NoValue);
// 	hist.resize(endIdx - start_index + 1, Chart::NoValue);
// 
// 	int max_period = max(fast_period, slow_period);
// 	int shift = max_period + signal_period - 2;
// 	int retCode = TA_MACD(
// 		startIdx,
// 		endIdx,
// 		_Data.data(),
// 		fast_period,
// 		slow_period,
// 		signal_period,
// 		&outBegIdx,
// 		&outNbElement,
// 		macd.data() + shift,
// 		signal.data() + shift,
// 		hist.data() + shift);
// 
// 	return *this;
// }

DarkHorse::SmArrayMath& SmArrayMath::heiken_ashi(hmdf::StdDataFrame<hmdf::DateTime>& df)
{
	std::vector<double>& high = df.get_column<double>("high");
	std::vector<double>& low = df.get_column<double>("low");
	std::vector<double>& open = df.get_column<double>("open");
	std::vector<double>& close = df.get_column<double>("close");
	size_t size = open.size();
	std::vector<double> ha_open(size, Chart::NoValue), ha_high(size, Chart::NoValue), ha_low(size, Chart::NoValue);
	int found = 0;
	for (size_t i = 0; i < open.size(); ++i) {
		if (open[i] != Chart::NoValue) { found = i;	break; }
	}
	std::vector<double> ha_close(open.begin(), open.end());
	 SmArrayMath(ha_close).add(high).add(low).add(close).mul(0.25).result();
	for (size_t i = found; i < open.size(); ++i) {
		double h_open = 0.0;
		if (i == found) h_open = (open[i] + close[i]) / 2.0;
		else h_open = (ha_open[i - 1] + ha_close[i - 1]) / 2.0;
		ha_open[i] = h_open;
		ha_high[i] = (std::max)({ h_open, ha_close[i], high[i] });
		ha_low[i] = (std::min)({ h_open, ha_close[i], low[i] });;
	}

	df.load_column("ha_open", std::move(ha_open));
	df.load_column("ha_high", std::move(ha_high));
	df.load_column("ha_low", std::move(ha_low));
	df.load_column("ha_close", std::move(ha_close));

	return *this;
}

DarkHorse::SmArrayMath& SmArrayMath::heiken_ashi(hmdf::StdDataFrame<hmdf::DateTime>& df, const std::string& prefix)
{
	std::string col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix);
	col_high.append("high");
	col_low.append("low");
	col_open.append("open");
	col_close.append("close");
	std::vector<double>& high = df.get_column<double>(col_high.c_str());
	std::vector<double>& low = df.get_column<double>(col_low.c_str());
	std::vector<double>& open = df.get_column<double>(col_open.c_str());
	std::vector<double>& close = df.get_column<double>(col_close.c_str());
	size_t size = open.size();
	std::vector<double> ha_open(size, Chart::NoValue), ha_high(size, Chart::NoValue), ha_low(size, Chart::NoValue);
	int found = 0;
	for (size_t i = 0; i < open.size(); ++i) {
		if (open[i] != Chart::NoValue) { found = i;	break; }
	}
	std::vector<double> ha_close(open.begin(), open.end());
	SmArrayMath(ha_close).add(high).add(low).add(close).mul(0.25).result();
	for (size_t i = found; i < open.size(); ++i) {
		double h_open = 0.0;
		if (i == found) h_open = (open[i] + close[i]) / 2.0;
		else h_open = (ha_open[i - 1] + ha_close[i - 1]) / 2.0;
		ha_open[i] = h_open;
		ha_high[i] = (std::max)({ h_open, ha_close[i], high[i] });
		ha_low[i] = (std::min)({ h_open, ha_close[i], low[i] });;
	}


	std::string col_ha_open(prefix), col_ha_high(prefix), col_ha_low(prefix), col_ha_close(prefix);
	col_ha_open.append("ha_open");
	col_ha_high.append("ha_high");
	col_ha_low.append("ha_low");
	col_ha_close.append("ha_close");

	df.load_column(col_ha_open.c_str(), std::move(ha_open));
	df.load_column(col_ha_high.c_str(), std::move(ha_high));
	df.load_column(col_ha_low.c_str(), std::move(ha_low));
	df.load_column(col_ha_close.c_str(), std::move(ha_close));

	return *this;
}

DarkHorse::SmArrayMath& SmArrayMath::heiken_ashi_smooth(const int& period, hmdf::StdDataFrame<hmdf::DateTime>& df)
{
	std::vector<double>& high = df.get_column<double>("high");
	std::vector<double>& low = df.get_column<double>("low");
	std::vector<double>& open = df.get_column<double>("open");
	std::vector<double>& close = df.get_column<double>("close");
	const size_t size = open.size();
	std::vector<double> ha_open(size, Chart::NoValue), ha_high(size, Chart::NoValue), ha_low(size, Chart::NoValue);
	SmArrayMath(open).mov_avg(period, "ma_open", df);
	SmArrayMath(high).mov_avg(period, "ma_high", df);
	SmArrayMath(low).mov_avg(period, "ma_low", df);
	SmArrayMath(close).mov_avg(period, "ma_close", df);

	std::vector<double>& ma_open = df.get_column<double>("ma_open");
	std::vector<double>& ma_high = df.get_column<double>("ma_high");
	std::vector<double>& ma_low = df.get_column<double>("ma_low");
	std::vector<double>& ma_close = df.get_column<double>("ma_close");

	std::vector<double> ha_close(ma_open.begin(), ma_open.end());
	SmArrayMath(ha_close).add(ma_high).add(ma_low).add(ma_close).mul(0.25);
	for (size_t i = period - 1; i < size; ++i) {
		double h_open = 0.0;

		if (i == period - 1) h_open = (ma_open[i] + ma_close[i]) / 2.0;
		else h_open = (ha_open[i - 1] + ha_close[i - 1]) / 2.0;

		ha_open[i] = h_open;

		ha_high[i] = (std::max)({ h_open, ha_close[i], ma_high[i] });
		ha_low[i] = (std::min)({ h_open, ha_close[i], ma_low[i] });
	}

	df.load_column("ha2_open", std::move(ha_open));
	df.load_column("ha2_high", std::move(ha_high));
	df.load_column("ha2_low", std::move(ha_low));
	df.load_column("ha2_close", std::move(ha_close));

	return *this;
}

DarkHorse::SmArrayMath& SmArrayMath::heiken_ashi_smooth(const int& period, hmdf::StdDataFrame<hmdf::DateTime>& df, const std::string& prefix)
{
	std::string col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix);
	col_high.append("high");
	col_low.append("low");
	col_open.append("open");
	col_close.append("close");
	std::vector<double>& high = df.get_column<double>(col_high.c_str());
	std::vector<double>& low = df.get_column<double>(col_low.c_str());
	std::vector<double>& open = df.get_column<double>(col_open.c_str());
	std::vector<double>& close = df.get_column<double>(col_close.c_str());


	const size_t size = open.size();
	std::vector<double> ha_open(size, Chart::NoValue), ha_high(size, Chart::NoValue), ha_low(size, Chart::NoValue);
	std::string col_ma_open(prefix), col_ma_high(prefix), col_ma_low(prefix), col_ma_close(prefix);
	col_ma_open.append("ma_open");
	col_ma_high.append("ma_high");
	col_ma_low.append("ma_low");
	col_ma_close.append("ma_close");

	std::vector<double> ma_open(open.begin(), open.end());
	std::vector<double> ma_high(high.begin(), high.end());
	std::vector<double> ma_low(low.begin(), low.end());
	std::vector<double> ma_close(close.begin(), close.end());

	SmArrayMath(ma_open).mov_avg(period, col_ma_open.c_str(), df);
	SmArrayMath(ma_high).mov_avg(period, col_ma_high.c_str(), df);
	SmArrayMath(ma_low).mov_avg(period, col_ma_low.c_str(), df);
	SmArrayMath(ma_close).mov_avg(period, col_ma_close.c_str(), df);

	//std::vector<double>& ma_open = df.get_column<double>(col_ma_open.c_str());
	//std::vector<double>& ma_high = df.get_column<double>(col_ma_high.c_str());
	//std::vector<double>& ma_low = df.get_column<double>(col_ma_low.c_str());
	//std::vector<double>& ma_close = df.get_column<double>(col_ma_close.c_str());

	std::vector<double> ha_close(ma_open.begin(), ma_open.end());
	SmArrayMath(ha_close).add(ma_high).add(ma_low).add(ma_close).mul(0.25);
	for (size_t i = period - 1; i < size; ++i) {
		double h_open = 0.0;

		if (i == period - 1) h_open = (ma_open[i] + ma_close[i]) / 2.0;
		else h_open = (ha_open[i - 1] + ha_close[i - 1]) / 2.0;

		ha_open[i] = h_open;

		ha_high[i] = (std::max)({ h_open, ha_close[i], ma_high[i] });
		ha_low[i] = (std::min)({ h_open, ha_close[i], ma_low[i] });
	}

	std::string col_ha2_open(prefix), col_ha2_high(prefix), col_ha2_low(prefix), col_ha2_close(prefix);
	col_ha2_open.append("ha2_open");
	col_ha2_high.append("ha2_high");
	col_ha2_low.append("ha2_low");
	col_ha2_close.append("ha2_close");

	df.load_column(col_ha2_open.c_str(), std::move(ha_open));
	df.load_column(col_ha2_high.c_str(), std::move(ha_high));
	df.load_column(col_ha2_low.c_str(), std::move(ha_low));
	df.load_column(col_ha2_close.c_str(), std::move(ha_close));

	return *this;
}

