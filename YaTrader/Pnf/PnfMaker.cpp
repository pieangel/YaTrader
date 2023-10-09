#include "stdafx.h"
#include "PnfMaker.h"
#include "pnfDefine.h"
#include <memory>
#include "../Chart/SmChartData.h"
using namespace DarkHorse;

PnfMaker::PnfMaker(const double& box_size, const int& reverse)
	:
	_BoxSize(box_size),
	_Reverse(reverse)
{
	TRACE("box_size = %.2f, reverse = %d\n", box_size, reverse);
}

void PnfMaker::OnNewData(const double& time, const double& high, const double& low, const double& close)
{
	double high_delta = 0, low_delta = 0;
	int count = 0;
	if (_Direction == 0) {
		if (_High == 0)
			_High = high;
		else
		{
			if (high > _High) {
				_High = high;
			}
		}
		if (_Low == 0)
			_Low = low;
		else
		{
			if (low < _Low)
			{
				_Low = low;
			}
		}

		if ((_High - _Low) >= _BoxSize) {
			_Direction = 1;
			const double delta = abs(_High - _Low);
			count = static_cast<int> (delta / _BoxSize);
			count = abs(count);
			_BaseValue = _Low + (_BoxSize * count);
			if (high > _High) {
				_High = high;
			}

			const std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
			pnf->box_size = _BoxSize;
			pnf->direction = 1;
			pnf->close_value = _BaseValue;
			pnf->open_value = _Low;
			pnf->high_value = _High;
			pnf->low_value = _Low;
			pnf->start_point = true;
			pnf->count = count;
			pnf->acc_count = count;
			pnf->reverse = _Reverse;
			pnf->time = time;
			_PnfMap[pnf->time] = pnf;

			_ChartTime = pnf->time;

			//dst_data->AddChartData(_ChartTime, pnf->open_value, pnf->close_value, pnf->open_value, pnf->close_value);

		}
	}
	else if (_Direction == 1) {
		high_delta = high - _BaseValue;
		// 고가 업데이트
		if (high > _High) {
			_High = high;
		}

		low_delta = _High - low;
		// 박스 크기보다 큰 경우
		if (high_delta >= _BoxSize) {
			count = static_cast<int>(high_delta / _BoxSize);
			count = abs(count);
			const std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
			pnf->box_size = _BoxSize;
			pnf->direction = 1;
			pnf->close_value = _BaseValue + _BoxSize * count;
			pnf->open_value = _BaseValue;
			pnf->count = count;
			pnf->reverse = _Reverse;
			pnf->time = time;


			// 기준점을 다시 정해 준다.
			_BaseValue += _BoxSize * count;

			const auto prev = std::prev(_PnfMap.end(), 1);
			//dst_data->SetChartData(_ChartTime, _BaseValue, _High, prev->second->low_value);
			pnf->low_value = prev->second->low_value;
			pnf->high_value = _High;
			pnf->acc_count += prev->second->acc_count + count;

			_PnfMap[pnf->time] = pnf;

		}
		// 반전이 일어 나는 경우
		else if (low_delta > static_cast<long>(_BoxSize * _Reverse)) {
			count = static_cast<int> (low_delta / _BoxSize);
			count = abs(count);
			// 기준점을 다시 정해 준다.
			_BaseValue = _High - (_BoxSize * count);
			const std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
			pnf->box_size = _BoxSize;
			pnf->direction = -1;
			pnf->close_value = _BaseValue;
			pnf->open_value = _High;
			pnf->high_value = _High;
			pnf->low_value = _Low;
			pnf->count = count;
			pnf->acc_count = count;
			pnf->start_point = true;
			pnf->reverse = _Reverse;
			pnf->time = time;
			_Direction = -1;



			_Low = low;

			const auto prev = std::prev(_PnfMap.end(), 1);
			//dst_data->SetChartData(_ChartTime, prev->second->close_value, high, prev->second->low_value);

			//dst_data->AddChartData(pnf->time, pnf->open_value, pnf->open_value, low, pnf->close_value);

			_ChartTime = pnf->time;

			_PnfMap[pnf->time] = pnf;

		}

	}
	else if (_Direction == -1) {
		low_delta = _BaseValue - low;
		// 저가 업데이트
		if (low < _Low) {
			_Low = low;
		}

		high_delta = high - _Low;
		// 박스 크기보다 큰 경우
		if (low_delta >= _BoxSize) {
			count = static_cast<int>(low_delta / _BoxSize);
			count = abs(count);
			const std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
			pnf->box_size = _BoxSize;
			pnf->direction = -1;
			pnf->open_value = _BaseValue;
			pnf->close_value = _BaseValue - _BoxSize * count;
			pnf->count = count;
			pnf->reverse = _Reverse;
			pnf->time = time;

			// 기준점을 다시 정해 준다.
			_BaseValue -= _BoxSize * count;

			const auto prev = std::prev(_PnfMap.end(), 1);
			//dst_data->SetChartData(_ChartTime, _BaseValue, prev->second->high_value, _Low);

			pnf->low_value = _Low;
			pnf->high_value = prev->second->high_value;
			pnf->acc_count += prev->second->acc_count + count;

			_PnfMap[pnf->time] = pnf;

		}
		// 반전이 일어 나는 경우
		else if (high_delta > static_cast<long>(_BoxSize * _Reverse)) {
			count = static_cast<int> (high_delta / _BoxSize);
			count = abs(count);
			// 기준점을 다시 정해 준다.
			_BaseValue = _Low + (_BoxSize * count);
			const std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
			pnf->box_size = _BoxSize;
			pnf->direction = 1;
			pnf->close_value = _BaseValue;
			pnf->open_value = _Low;
			pnf->high_value = _High;
			pnf->low_value = _Low;
			pnf->count = count;
			pnf->acc_count = count;
			pnf->start_point = true;
			pnf->reverse = _Reverse;
			pnf->time = time;
			_Direction = 1;

			_High = high;

			const auto prev = std::prev(_PnfMap.end(), 1);
			//dst_data->SetChartData(_ChartTime, prev->second->close_value, prev->second->high_value, _Low);

			//dst_data->AddChartData(pnf->time, pnf->open_value, pnf->close_value, pnf->open_value, pnf->close_value);

			_ChartTime = pnf->time;

			_PnfMap[pnf->time] = pnf;

		}

	}
}

void PnfMaker::OnNewDataForChartData(const double& time, const double& high, const double& low, const double& close, std::shared_ptr<SmChartData> dst_data)
{
	if (!dst_data) return;
	
	double high_delta = 0, low_delta = 0;
	int count = 0;
	if (_Direction == 0) {
		if (_High == 0) 
			_High = high;
		else {
			if (high > _High) {
				_High = high;
			}
		}
		if (_Low == 0) 
			_Low = low;
		else {
			if (low < _Low) {
				_Low = low;
			}
		}

		if ((_High - _Low) >= _BoxSize) {
			_Direction = 1;
			const double delta = abs(_High - _Low);
			count = static_cast<int> (delta / _BoxSize);
			count = abs(count);
			_BaseValue = _Low + (_BoxSize * count);
			if (high > _High) {
				_High = high;
			}

			const std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
			pnf->box_size = _BoxSize;
			pnf->direction = 1;
			pnf->close_value = _BaseValue;
			pnf->open_value = _Low;
			pnf->high_value = _High;
			pnf->low_value = _Low;
			pnf->start_point = true;
			pnf->count = count;
			pnf->acc_count = count;
			pnf->reverse = _Reverse;
			pnf->time = time;
			_PnfMap[pnf->time] = pnf;

			_ChartTime = pnf->time;

			dst_data->AddChartData(_ChartTime, pnf->open_value, pnf->close_value, pnf->open_value, pnf->close_value);

		}
	}
	else if (_Direction == 1) {
		high_delta = high - _BaseValue;
		// 고가 업데이트
		if (high > _High) {
			_High = high;
		}

		low_delta = _High - low;
		// 박스 크기보다 큰 경우
		if (high_delta >= _BoxSize) {
			count = static_cast<int>(high_delta / _BoxSize);
			count = abs(count);
			const std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
			pnf->box_size = _BoxSize;
			pnf->direction = 1;
			pnf->close_value = _BaseValue + _BoxSize * count;
			pnf->open_value = _BaseValue;
			pnf->count = count;
			pnf->reverse = _Reverse;
			pnf->time = time;
			

			// 기준점을 다시 정해 준다.
			_BaseValue += _BoxSize * count;

			const auto prev = std::prev(_PnfMap.end(), 1);
			dst_data->SetChartData(_ChartTime, _BaseValue, _High, prev->second->low_value);
			pnf->low_value = prev->second->low_value;
			pnf->high_value = _High;
			pnf->acc_count += prev->second->acc_count + count;

			_PnfMap[pnf->time] = pnf;

		}
		// 반전이 일어 나는 경우
		else if (low_delta > static_cast<long>(_BoxSize * _Reverse)) {
			count = static_cast<int> (low_delta / _BoxSize);
			count = abs(count);
			// 기준점을 다시 정해 준다.
			_BaseValue = _High - (_BoxSize * count);
			const std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
			pnf->box_size = _BoxSize;
			pnf->direction = -1;
			pnf->close_value = _BaseValue;
			pnf->open_value = _High;
			pnf->high_value = _High;
			pnf->low_value = _Low;
			pnf->count = count;
			pnf->acc_count = count;
			pnf->start_point = true;
			pnf->reverse = _Reverse;
			pnf->time = time;
			_Direction = -1;

			

			_Low = low;

			const auto prev = std::prev(_PnfMap.end(), 1);
			dst_data->SetChartData(_ChartTime, prev->second->close_value, high, prev->second->low_value);

			dst_data->AddChartData(pnf->time, pnf->open_value, pnf->open_value, low, pnf->close_value);

			_ChartTime = pnf->time;

			_PnfMap[pnf->time] = pnf;

		}

	}
	else if (_Direction == -1) {
		low_delta = _BaseValue - low;
		// 저가 업데이트
		if (low < _Low) {
			_Low = low;
		}

		high_delta = high - _Low;
		// 박스 크기보다 큰 경우
		if (low_delta >= _BoxSize) {
			count = static_cast<int>(low_delta / _BoxSize);
			count = abs(count);
			const std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
			pnf->box_size = _BoxSize;
			pnf->direction = -1;
			pnf->open_value = _BaseValue;
			pnf->close_value = _BaseValue - _BoxSize * count;
			pnf->count = count;
			pnf->reverse = _Reverse;
			pnf->time = time;
			
			// 기준점을 다시 정해 준다.
			_BaseValue -= _BoxSize * count;

			const auto prev = std::prev(_PnfMap.end(), 1);
			dst_data->SetChartData(_ChartTime, _BaseValue, prev->second->high_value, _Low);

			pnf->low_value = _Low;
			pnf->high_value = prev->second->high_value;
			pnf->acc_count += prev->second->acc_count + count;

			_PnfMap[pnf->time] = pnf;

		}
		// 반전이 일어 나는 경우
		else if (high_delta > static_cast<long>(_BoxSize * _Reverse)) {
			count = static_cast<int> (high_delta / _BoxSize);
			count = abs(count);
			// 기준점을 다시 정해 준다.
			_BaseValue = _Low + (_BoxSize * count);
			const std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
			pnf->box_size = _BoxSize;
			pnf->direction = 1;
			pnf->close_value = _BaseValue;
			pnf->open_value = _Low;
			pnf->high_value = _High;
			pnf->low_value = _Low;
			pnf->count = count;
			pnf->acc_count = count;
			pnf->start_point = true;
			pnf->reverse = _Reverse;
			pnf->time = time;
			_Direction = 1;

			_High = high;

			const auto prev = std::prev(_PnfMap.end(), 1);
			dst_data->SetChartData(_ChartTime, prev->second->close_value, prev->second->high_value, _Low);

			dst_data->AddChartData(pnf->time, pnf->open_value, pnf->close_value, pnf->open_value, pnf->close_value);

			_ChartTime = pnf->time;

			_PnfMap[pnf->time] = pnf;

		}

	}
}
