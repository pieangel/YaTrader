#include "stdafx.h"
#include "PnfManager.h"
#include "../Chart/SmChartData.h"
#include "PnfMaker.h"
using namespace DarkHorse;

void PnfManager::MakePnf(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& box_size, const int& reverse)
{
	if (!chart_data)
		return;

	/*
	When the current column is an X-Column (rising):

	Use the high when another X can be drawn and then ignore the low.
	Use the low when another X cannot be drawn and the low triggers a 3-box reversal.
	Ignore both when the high does not warrant another X and the low does not trigger a 3-box reversal.
	
	
	When the current column is an O-Column (falling):

	Use the low when another O can be drawn and then ignore the high.
	Use the high when another O cannot be drawn and the high triggers a 3-box reversal.
	Ignore both when the low does not warrant another O and the high does not trigger a 3-box reversal.
	
	*/
	_PnfMap.clear();
	double base_value = 0;
	int direction = 0;
	hmdf::StdDataFrame<hmdf::DateTime>& df = chart_data->GetDataFrame();

	std::string prefix = chart_data->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");

	auto& dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high = df.get_column<double>(col_high.c_str());
	auto& low = df.get_column<double>(col_low.c_str());
	auto& open = df.get_column<double>(col_open.c_str());
	auto& close = df.get_column<double>(col_close.c_str());

	double start_high = high[0];
	double start_low = low[0];
	double f_remain = 0, high_remain = 0, low_remain = 0;
	f_remain = fmod(start_low, box_size);
	base_value = start_low - f_remain;
	direction = 1;
	int count = 0;
	double high_delta = 0, low_delta = 0;
	for (size_t i = 0; i < dt.size(); ++i) {
		high_delta = high[i] - base_value;
		high_remain = fmod(high_delta, box_size);
		
		low_delta = base_value - low[i];
		low_remain = fmod(low_delta, box_size);
		if (direction == 1) {
			// 박스 크기보다 큰 경우
			if (high_delta >= box_size) {
				count = (int)(high_delta / box_size);
				//if (high_remain > 0)
				//	count++;
				PnfItem pnf;
				pnf.box_size = box_size;
				pnf.direction = 1;
				pnf.close_value = base_value;
				pnf.count = count;
				pnf.reverse = reverse;
				pnf.time = dt[i];
				// 기준점을 다시 정해 준다.
				base_value += box_size * count;

				_PnfMap[pnf.time] = pnf;
			}
			// 박스 크기보다 작으면서 반전이 일어 나는 경우
			else if (low_delta > static_cast<long>(box_size * reverse)) {
				count = (int)(low_delta / box_size);
				//if (low_remain > 0)
				//	count++;
				PnfItem pnf;
				pnf.box_size = box_size;
				pnf.direction = -1;
				pnf.close_value = base_value;
				pnf.open_value = base_value;
				pnf.count = count;
				pnf.start_point = true;
				pnf.reverse = reverse;
				pnf.time = dt[i];
				// 기준점을 다시 정해 준다.
				base_value -= box_size * count;
				direction = -1;
				
				_PnfMap[pnf.time] = pnf;
			}
		}
		else {
			if (low_delta >= box_size) {
				count = (int)(low_delta / box_size);
				//if (low_remain > 0)
				//	count++;
				PnfItem pnf;
				pnf.box_size = box_size;
				pnf.direction = -1;
				pnf.close_value = base_value;
				pnf.count = count;
				pnf.reverse = reverse;
				pnf.time = dt[i];
				// 기준점을 다시 정해 준다.
				base_value -= box_size * count;
				_PnfMap[pnf.time] = pnf;
			}
			else if (high_delta > static_cast<long>(box_size * reverse)) {
				count = (int)(high_delta / box_size);
				//if (high_remain > 0)
				//	count++;
				PnfItem pnf;
				pnf.box_size = box_size;
				pnf.direction = 1;
				pnf.close_value = base_value;
				pnf.open_value = base_value;
				pnf.count = count;
				pnf.start_point = true;
				pnf.reverse = reverse;
				pnf.time = dt[i];
				// 기준점을 다시 정해 준다.
				base_value += box_size * count;
				direction = 1;
				_PnfMap[pnf.time] = pnf;
			}
		}
	}
}



void PnfManager::MakePnfByEdge(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& box_size, const int& reverse, std::map<double, std::shared_ptr<PnfItem>>& pnf_map)
{
	if (!chart_data || chart_data->GetChartDataCount() < 1)
		return;

	/*
	When the current column is an X-Column (rising):

	Use the high when another X can be drawn and then ignore the low.
	Use the low when another X cannot be drawn and the low triggers a 3-box reversal.
	Ignore both when the high does not warrant another X and the low does not trigger a 3-box reversal.


	When the current column is an O-Column (falling):

	Use the low when another O can be drawn and then ignore the high.
	Use the high when another O cannot be drawn and the high triggers a 3-box reversal.
	Ignore both when the low does not warrant another O and the high does not trigger a 3-box reversal.

	*/
	pnf_map.clear();

	hmdf::StdDataFrame<hmdf::DateTime>& df = chart_data->GetDataFrame();

	std::string prefix = chart_data->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");

	auto& dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high_v = df.get_column<double>(col_high.c_str());
	auto& low_v = df.get_column<double>(col_low.c_str());
	auto& open_v = df.get_column<double>(col_open.c_str());
	auto& close_v = df.get_column<double>(col_close.c_str());

	double base_value = 0;
	int direction = 0;
	double high = high_v[0];
	double low = low_v[0];
	//double f_remain = 0, high_remain = 0, low_remain = 0;
	int count = 0;
	double high_delta = 0, low_delta = 0, delta = 0;
	for (size_t i = 0; i < dt.size(); ++i) {
		if (direction == 0) {
			if ((high - low) < box_size) {
				if (high_v[i] > high) {
					high = high_v[i];
				}
				if (low_v[i] < low)
				{
					low = low_v[i];
				}
			}
			else
			{
				direction = 1;
				delta = abs(high - low);
				count = (int)(delta / box_size);
				count = abs(count);
				base_value = low + (box_size * count);
				if (high_v[i] > high) {
					high = high_v[i];
				}

				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = 1;
				pnf->close_value = base_value;
				pnf->open_value = low;
				pnf->start_point = true;
				pnf->count = count;
				pnf->reverse = reverse;
				pnf->time = dt[i];
				pnf_map[pnf->time] = pnf;


				TRACE("start_value = %.2f, close_value = %.2f, base_value = %.2f\n", pnf->open_value, pnf->close_value, base_value);
			}
		}
		else if (direction == 1) {
			high_delta = high_v[i] - base_value;
			// 고가 업데이트
			if (high_v[i] > high) {
				high = high_v[i];
			}
// 			if (chart_data->Low()[i] < low) {
// 				low = chart_data->Low()[i];
// 			}
			low_delta = high - low_v[i];
			// 박스 크기보다 큰 경우
			if (high_delta >= box_size) {
				count = (int)(high_delta / box_size);
				count = abs(count);
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = 1;
				pnf->close_value = base_value + box_size * count;
				pnf->open_value = base_value;
				pnf->count = count;
				pnf->reverse = reverse;
				pnf->time = dt[i];
				pnf_map[pnf->time] = pnf;

				

				// 기준점을 다시 정해 준다.
				base_value += box_size * count;

				TRACE("start_value = %.2f, close_value = %.2f, base_value = %.2f\n", pnf->open_value, pnf->close_value, base_value);
			}
			// 반전이 일어 나는 경우
			else if (low_delta > static_cast<long>(box_size * reverse)) {
				count = (int)(low_delta / box_size);
				count = abs(count);
				// 기준점을 다시 정해 준다.
				base_value = high - (box_size * count);
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = -1;
				pnf->close_value = base_value;
				pnf->open_value = high;
				pnf->count = count;
				pnf->start_point = true;
				pnf->reverse = reverse;
				pnf->time = dt[i];
				direction = -1;

				pnf_map[pnf->time] = pnf;

				low = low_v[i];

				TRACE("<<<<<<<<start_value = %.2f, close_value = %.2f, base_value = %.2f\n", pnf->open_value, pnf->close_value, base_value);
			}
			
		}
		else if (direction == -1) {
			low_delta = base_value - low_v[i];
			// 저가 업데이트
			if (low_v[i] < low) {
				low = low_v[i];
			}
// 			if (chart_data->High()[i] > high) {
// 				high = chart_data->High()[i];
// 			}
			high_delta = high_v[i] - low;
			// 박스 크기보다 큰 경우
			if (low_delta >= box_size) {
				count = (int)(low_delta / box_size);
				count = abs(count);
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = -1;
				pnf->open_value = base_value;
				pnf->close_value = base_value - box_size * count;
				pnf->count = count;
				pnf->reverse = reverse;
				pnf->time = dt[i];
				pnf_map[pnf->time] = pnf;
				// 기준점을 다시 정해 준다.
				base_value -= box_size * count;

				TRACE("start_value = %.2f, close_value = %.2f, base_value = %.2f\n", pnf->open_value, pnf->close_value, base_value);
			}
			// 반전이 일어 나는 경우
			else if (high_delta > static_cast<long>(box_size * reverse)) {
				count = (int)(high_delta / box_size);
				count = abs(count);
				// 기준점을 다시 정해 준다.
				base_value = low + (box_size * count);
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = 1;
				pnf->close_value = base_value;
				pnf->open_value = low;
				pnf->count = count;
				pnf->start_point = true;
				pnf->reverse = reverse;
				pnf->time = dt[i];
				direction = 1;
				pnf_map[pnf->time] = pnf;

				high = high_v[i];

				TRACE(">>>>>>>>>start_value = %.2f, close_value = %.2f, base_value = %.2f\n", pnf->open_value, pnf->close_value, base_value);
			}
			
		}
	}
}

void PnfManager::MakePnfBar(std::shared_ptr<SmChartData> chart_data, const int& box_size, const int& reverse, std::map<double, std::shared_ptr<PnfItem>>& pnf_map)
{
	if (!chart_data)
		return;

	/*
	When the current column is an X-Column (rising):

	Use the high when another X can be drawn and then ignore the low.
	Use the low when another X cannot be drawn and the low triggers a 3-box reversal.
	Ignore both when the high does not warrant another X and the low does not trigger a 3-box reversal.


	When the current column is an O-Column (falling):

	Use the low when another O can be drawn and then ignore the high.
	Use the high when another O cannot be drawn and the high triggers a 3-box reversal.
	Ignore both when the low does not warrant another O and the high does not trigger a 3-box reversal.

	*/
	pnf_map.clear();
	hmdf::StdDataFrame<hmdf::DateTime>& df = chart_data->GetDataFrame();


	std::string prefix = chart_data->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");

	auto& dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high_v = df.get_column<double>(col_high.c_str());
	auto& low_v = df.get_column<double>(col_low.c_str());
	auto& open_v = df.get_column<double>(col_open.c_str());
	auto& close_v = df.get_column<double>(col_close.c_str());

	double base_value = 0;
	int direction = 0;
	double start_high = high_v[0];
	double start_low = low_v[0];
	double f_remain = 0, high_remain = 0, low_remain = 0;
	f_remain = fmod(start_low, box_size);
	base_value = start_low - f_remain;
	direction = 1;
	int count = 0;
	double high_delta = 0, low_delta = 0;
	for (size_t i = 0; i < dt.size(); ++i) {
		high_delta = high_v[i] - base_value;
		high_remain = fmod(high_delta, box_size);
		low_delta = base_value - low_v[i];
		low_remain = fmod(low_delta, box_size);
		if (direction == 1) {
			// 박스 크기보다 큰 경우
			if (high_delta >= box_size) {
				count = (int)(high_delta / box_size);
				
				// 기준점을 다시 정해 준다.
				base_value += box_size * count;
				if (pnf_map.size() > 0) {
					auto prev = std::prev(pnf_map.end(), 1);
					prev->second->close_value = base_value;
					prev->second->count += count;
					
				}
			}
			// 박스 크기보다 작으면서 반전이 일어 나는 경우
			else if (low_delta > static_cast<long>(box_size * reverse)) {
				count = (int)(low_delta / box_size);
				//if (low_remain > 0)
				//	count++;
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = -1;
				pnf->close_value = base_value;
				pnf->open_value = base_value;
				pnf->count = count;
				pnf->reverse = reverse;
				pnf->start_point = true;
				pnf->time = dt[i];
				// 기준점을 다시 정해 준다.
				base_value -= box_size * count;
				direction = -1;

				pnf_map[pnf->time] = pnf;
			}
		}
		else {
			if (low_delta >= box_size) {
				count = (int)(low_delta / box_size);
				
				// 기준점을 다시 정해 준다.
				base_value -= box_size * count;

				if (pnf_map.size() > 0) {
					auto prev = std::prev(pnf_map.end(), 1);
					prev->second->close_value = base_value;
					prev->second->count += count;
				}
			}
			else if (high_delta > static_cast<long>(box_size * reverse)) {
				count = (int)(high_delta / box_size);
				//if (high_remain > 0)
				//	count++;
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = 1;
				pnf->close_value = base_value;
				pnf->open_value = base_value;
				pnf->count = count;
				pnf->reverse = reverse;
				pnf->start_point = true;
				pnf->time = dt[i];
				// 기준점을 다시 정해 준다.
				base_value += box_size * count;
				direction = 1;
				pnf_map[pnf->time] = pnf;
			}
		}
	}
}

void PnfManager::MakePnfBar(std::shared_ptr<SmChartData> chart_data, const int& box_size, const int& reverse, std::map<double, std::shared_ptr<PnfItem>>& pnf_map, std::shared_ptr<DarkHorse::SmChartData> dst_data)
{
	if (!chart_data)
		return;

	/*
	When the current column is an X-Column (rising):

	Use the high when another X can be drawn and then ignore the low.
	Use the low when another X cannot be drawn and the low triggers a 3-box reversal.
	Ignore both when the high does not warrant another X and the low does not trigger a 3-box reversal.


	When the current column is an O-Column (falling):

	Use the low when another O can be drawn and then ignore the high.
	Use the high when another O cannot be drawn and the high triggers a 3-box reversal.
	Ignore both when the low does not warrant another O and the high does not trigger a 3-box reversal.

	*/
	pnf_map.clear();
	hmdf::StdDataFrame<hmdf::DateTime>& df = chart_data->GetDataFrame();
	std::string prefix = chart_data->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");

	auto& dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high_v = df.get_column<double>(col_high.c_str());
	auto& low_v = df.get_column<double>(col_low.c_str());
	auto& open_v = df.get_column<double>(col_open.c_str());
	auto& close_v = df.get_column<double>(col_close.c_str());

	double base_value = 0;
	int direction = 0;
	double start_high = high_v[0];
	double start_low = low_v[0];
	double f_remain = 0, high_remain = 0, low_remain = 0;
	f_remain = fmod(start_low, box_size);
	base_value = start_low - f_remain;
	direction = 1;
	int count = 0;
	double high_delta = 0, low_delta = 0;
	for (size_t i = 0; i < dt.size(); ++i) {
		high_delta = high_v[i] - base_value;
		high_remain = fmod(high_delta, box_size);
		low_delta = base_value - low_v[i];
		low_remain = fmod(low_delta, box_size);
		if (direction == 1) {
			// 박스 크기보다 큰 경우
			if (high_delta >= box_size) {
				count = (int)(high_delta / box_size);

				// 기준점을 다시 정해 준다.
				base_value += box_size * count;
				if (!pnf_map.empty()) {
					auto prev = std::prev(pnf_map.end(), 1);
					prev->second->close_value = base_value;
					prev->second->count += count;
					if (high_v[i] > prev->second->high_value)
						prev->second->high_value = high_v[i];
					if (low_v[i] < prev->second->low_value)
						prev->second->low_value = low_v[i];
					dst_data->SetChartData(prev->second->time, base_value, prev->second->high_value, prev->second->low_value);
				}
			}
			// 박스 크기보다 작으면서 반전이 일어 나는 경우
			else if (low_delta > static_cast<long>(box_size * reverse)) {
				count = (int)(low_delta / box_size);
				//if (low_remain > 0)
				//	count++;
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = -1;
				pnf->close_value = base_value;
				pnf->open_value = base_value;
				pnf->low_value = low_v[i];
				pnf->count = count;
				pnf->start_point = true;
				pnf->reverse = reverse;
				pnf->time = dt[i];
				// 기준점을 다시 정해 준다.
				base_value -= box_size * count;
				direction = -1;

				if (!pnf_map.empty()) {
					const auto prev = std::prev(pnf_map.end(), 1);
					pnf->high_value = prev->second->high_value;
				}
				else
				{
					pnf->high_value = high_v[i];
				}

				pnf_map[pnf->time] = pnf;

				dst_data->AddChartData(pnf->time, pnf->open_value, pnf->high_value, base_value, base_value);
			}
		}
		else {
			if (low_delta >= box_size) {
				count = (int)(low_delta / box_size);

				// 기준점을 다시 정해 준다.
				base_value -= box_size * count;

				if (!pnf_map.empty()) {
					auto prev = std::prev(pnf_map.end(), 1);
					prev->second->close_value = base_value;
					prev->second->count += count;
					if (low_v[i] < prev->second->low_value)
						prev->second->low_value = low_v[i];
					if (high_v[i] > prev->second->high_value)
						prev->second->high_value = high_v[i];
					dst_data->SetChartData(prev->second->time, base_value, prev->second->high_value, prev->second->low_value);
				}
			}
			else if (high_delta > static_cast<long>(box_size * reverse)) {
				count = (int)(high_delta / box_size);
				//if (high_remain > 0)
				//	count++;
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = 1;
				pnf->close_value = base_value;
				pnf->open_value = base_value;
				pnf->high_value = high_v[i];
				pnf->count = count;
				pnf->start_point = true;
				pnf->reverse = reverse;
				pnf->time = dt[i];
				// 기준점을 다시 정해 준다.
				base_value += box_size * count;
				direction = 1;
				if (!pnf_map.empty()) {
					const auto prev = std::prev(pnf_map.end(), 1);
					pnf->low_value = prev->second->low_value;
				}
				else
				{
					pnf->low_value = low_v[i];
				}
				pnf_map[pnf->time] = pnf;
				dst_data->AddChartData(pnf->time, pnf->open_value, base_value, low_v[i], base_value);
			}
		}
	}
}

void PnfManager::MakePnfBar(std::shared_ptr<SmChartData> chart_data, const double& box_size, const int& reverse)
{
	if (!chart_data)
		return;

	hmdf::StdDataFrame<hmdf::DateTime>& df = chart_data->GetDataFrame();
	std::string prefix = chart_data->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");

	auto& dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high_v = df.get_column<double>(col_high.c_str());
	auto& low_v = df.get_column<double>(col_low.c_str());
	auto& open_v = df.get_column<double>(col_open.c_str());
	auto& close_v = df.get_column<double>(col_close.c_str());

	std::shared_ptr<PnfMaker> pnf_maker = std::make_shared<PnfMaker>(box_size, reverse);
	for (size_t i = 0; i < dt.size(); ++i) {
		pnf_maker->OnNewData(dt[i], high_v[i], low_v[i], close_v[i]);
	}
	_PnfMakerMap[chart_data->GetChartDataKey()] = pnf_maker;
}

void PnfManager::MakePnfBar(std::shared_ptr<SmChartData> chart_data, std::shared_ptr<SmChartData> dst_data, const double& box_size, const int& reverse)
{
	if (!chart_data || !dst_data)
		return;
	hmdf::StdDataFrame<hmdf::DateTime>& df = chart_data->GetDataFrame();
	std::string prefix = chart_data->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");

	const auto& dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	const auto& high_v = df.get_column<double>(col_high.c_str());
	const auto& low_v = df.get_column<double>(col_low.c_str());
	const auto& open_v = df.get_column<double>(col_open.c_str());
	const auto& close_v = df.get_column<double>(col_close.c_str());
	std::shared_ptr<PnfMaker> pnf_maker = std::make_shared<PnfMaker>(box_size, reverse);
	for (size_t i = 0; i < dt.size(); ++i) {
		pnf_maker->OnNewDataForChartData(dt[i], high_v[i], low_v[i], close_v[i], dst_data);
	}
	_PnfMakerMap[chart_data->GetChartDataKey()] = pnf_maker;

	CString msg;
	msg.Format("MakePnfBar %s \n", chart_data->GetChartDataKey().c_str());
	TRACE(msg);
}

void PnfManager::MakePnfBarByEdge(std::shared_ptr<DarkHorse::SmChartData> chart_data, const int& box_size, const int& reverse, std::map<double, std::shared_ptr<PnfItem>>& pnf_map, std::shared_ptr<SmChartData> dst_data)
{
	if (!chart_data)
		return;

	/*
	When the current column is an X-Column (rising):

	Use the high when another X can be drawn and then ignore the low.
	Use the low when another X cannot be drawn and the low triggers a 3-box reversal.
	Ignore both when the high does not warrant another X and the low does not trigger a 3-box reversal.


	When the current column is an O-Column (falling):

	Use the low when another O can be drawn and then ignore the high.
	Use the high when another O cannot be drawn and the high triggers a 3-box reversal.
	Ignore both when the low does not warrant another O and the high does not trigger a 3-box reversal.

	*/
	pnf_map.clear();

	hmdf::StdDataFrame<hmdf::DateTime>& df = chart_data->GetDataFrame();
	std::string prefix = chart_data->MakePrefix();
	std::string col_dt(prefix), col_high(prefix), col_low(prefix), col_open(prefix), col_close(prefix), col_volume(prefix);
	col_dt.append("dt");
	col_open.append("open");
	col_high.append("high");
	col_low.append("low");
	col_close.append("close");

	auto& dt = df.get_column<double>(col_dt.c_str()); // Converted To Chart Director DateTime
	auto& high_v = df.get_column<double>(col_high.c_str());
	auto& low_v = df.get_column<double>(col_low.c_str());
	auto& open_v = df.get_column<double>(col_open.c_str());
	auto& close_v = df.get_column<double>(col_close.c_str());

	double base_value = 0;
	int direction = 0;
	double high = high_v[0];
	double low = low_v[0];
	double f_remain = 0, high_remain = 0, low_remain = 0;
	double chart_time = 0;
	int count = 0;
	double high_delta = 0, low_delta = 0, delta = 0;
	for (size_t i = 0; i < dt.size(); ++i) {
		if (direction == 0) {
			if ((high - low) < box_size) {
				if (high_v[i] > high) {
					high = high_v[i];
				}
				if (low_v[i] < low)
				{
					low = low_v[i];
				}
			}
			else
			{
				direction = 1;
				delta = abs(high - low);
				count = (int)(delta / box_size);
				count = abs(count);
				base_value = low + (box_size * count);
				if (high_v[i] > high) {
					high = high_v[i];
				}

				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = 1;
				pnf->close_value = base_value;
				pnf->open_value = low;
				pnf->high_value = high;
				pnf->low_value = low;
				pnf->start_point = true;
				pnf->count = count;
				pnf->acc_count = count;
				pnf->reverse = reverse;
				pnf->time = dt[i];
				pnf_map[pnf->time] = pnf;

				chart_time = pnf->time;

				dst_data->AddChartData(chart_time, pnf->open_value, pnf->close_value, pnf->open_value, pnf->close_value);
				TRACE("start_value = %.2f, close_value = %.2f, base_value = %.2f\n", pnf->open_value, pnf->close_value, base_value);
			}
		}
		else if (direction == 1) {
			high_delta = high_v[i] - base_value;
			// 고가 업데이트
			if (high_v[i] > high) {
				high = high_v[i];
			}
			low_delta = high - low_v[i];
			// 박스 크기보다 큰 경우
			if (high_delta >= box_size) {
				count = (int)(high_delta / box_size);
				count = abs(count);
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = 1;
				pnf->close_value = base_value + box_size * count;
				pnf->open_value = base_value;
				pnf->count = count;
				pnf->reverse = reverse;
				pnf->time = dt[i];


				// 기준점을 다시 정해 준다.
				base_value += box_size * count;


				const auto prev = std::prev(pnf_map.end(), 1);
				dst_data->SetChartData(chart_time, base_value, high, prev->second->low_value);
				pnf->low_value = prev->second->low_value;
				pnf->high_value = high;
				pnf->acc_count += prev->second->acc_count + count;

				pnf_map[pnf->time] = pnf;


				TRACE("pnf->acc_count = %d, start_value = %.2f, close_value = %.2f, base_value = %.2f\n", pnf->acc_count, pnf->open_value, pnf->close_value, base_value);
			}
			// 반전이 일어 나는 경우
			else if (low_delta > static_cast<long>(box_size * reverse)) {
				count = (int)(low_delta / box_size);
				count = abs(count);
				// 기준점을 다시 정해 준다.
				base_value = high - (box_size * count);
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = -1;
				pnf->close_value = base_value;
				pnf->open_value = high;
				pnf->high_value = high;
				pnf->low_value = low;
				pnf->count = count;
				pnf->acc_count = count;
				pnf->start_point = true;
				pnf->reverse = reverse;
				pnf->time = dt[i];
				direction = -1;


				low = low_v[i];

				const auto prev = std::prev(pnf_map.end(), 1);
				dst_data->SetChartData(chart_time, prev->second->close_value, high, prev->second->low_value);
				
				dst_data->AddChartData(pnf->time, pnf->open_value, pnf->open_value, low, pnf->close_value);

				chart_time = pnf->time;

				pnf_map[pnf->time] = pnf;

				TRACE("<<<<<<<<start_value = %.2f, close_value = %.2f, base_value = %.2f\n", pnf->open_value, pnf->close_value, base_value);
			}

		}
		else if (direction == -1) {
			low_delta = base_value - low_v[i];
			// 저가 업데이트
			if (low_v[i] < low) {
				low = low_v[i];
			}
		
			high_delta = high_v[i] - low;
			// 박스 크기보다 큰 경우
			if (low_delta >= box_size) {
				count = (int)(low_delta / box_size);
				count = abs(count);
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = -1;
				pnf->open_value = base_value;
				pnf->close_value = base_value - box_size * count;
				pnf->count = count;
				pnf->reverse = reverse;
				pnf->time = dt[i];
				// 기준점을 다시 정해 준다.
				base_value -= box_size * count;

				const auto prev = std::prev(pnf_map.end(), 1);
				dst_data->SetChartData(chart_time, base_value, prev->second->high_value, low);

				pnf->low_value = low;
				pnf->high_value = prev->second->high_value;
				pnf->acc_count += prev->second->acc_count + count;
				pnf_map[pnf->time] = pnf;


				TRACE("pnf->acc_count = %d, start_value = %.2f, close_value = %.2f, base_value = %.2f\n", pnf->acc_count, pnf->open_value, pnf->close_value, base_value);
			}
			// 반전이 일어 나는 경우
			else if (high_delta > static_cast<long>(box_size * reverse)) {
				count = (int)(high_delta / box_size);
				count = abs(count);
				// 기준점을 다시 정해 준다.
				base_value = low + (box_size * count);
				std::shared_ptr<PnfItem> pnf = std::make_shared<PnfItem>();
				pnf->box_size = box_size;
				pnf->direction = 1;
				pnf->close_value = base_value;
				pnf->open_value = low;
				pnf->high_value = high;
				pnf->low_value = low;
				pnf->count = count;
				pnf->acc_count = count;
				pnf->start_point = true;
				pnf->reverse = reverse;
				pnf->time = dt[i];
				direction = 1;

				high = high_v[i];


				const auto prev = std::prev(pnf_map.end(), 1);
				dst_data->SetChartData(chart_time, prev->second->close_value, prev->second->high_value, low);

				dst_data->AddChartData(pnf->time, pnf->open_value, pnf->close_value, pnf->open_value, pnf->close_value);

				chart_time = pnf->time;

				pnf_map[pnf->time] = pnf;


				TRACE(">>>>>>>>>start_value = %.2f, close_value = %.2f, base_value = %.2f\n", pnf->open_value, pnf->close_value, base_value);
			}

		}
	}
}

std::shared_ptr<PnfMaker> PnfManager::FindPnfMaker(const std::string data_key)
{
	auto found = _PnfMakerMap.find(data_key);
	if (found != _PnfMakerMap.end())
		return found->second;

	return nullptr;
}
