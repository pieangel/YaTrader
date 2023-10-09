#include "stdafx.h"
#include "SmMarket.h"
#include "SmProduct.h"
using namespace DarkHorse;

std::shared_ptr<SmProduct> SmMarket::AddProduct(const std::string& product_code)
{
	if (product_code.empty()) return nullptr;
	const auto found = _ProductMap.find(product_code);
	if (found != _ProductMap.end())
		return found->second;
	auto product = std::make_shared<SmProduct>(product_code);
	_ProductMap[product_code] = product;
	return product;
}

std::shared_ptr<SmProduct> SmMarket::FindProduct(const std::string& product_code)
{
	const auto found = _ProductMap.find(product_code);
	if (found != _ProductMap.end())
		return found->second;
	else
		return nullptr;
}

int DarkHorse::SmMarket::GetValidProductCount()
{
	int row = 0;
	for (auto it2 = _ProductMap.begin(); it2 != _ProductMap.end(); ++it2) {
		const auto product = it2->second;
		const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec = product->GetSymbolVec();
		if (symbol_vec.empty()) continue;
		row++;
	}

	return row;
}

void DarkHorse::SmMarket::GetProductVector(const int& start_index, const int& count, std::vector<std::shared_ptr<SmProduct>>& product_vec)
{
	const int product_count = GetValidProductCount();
	if (start_index >= product_count) return;
	
	std::vector<std::shared_ptr<SmProduct>> product_valid_vec;
	for (auto it2 = _ProductMap.begin(); it2 != _ProductMap.end(); ++it2) {
		const auto product = it2->second;
		const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec = product->GetSymbolVec();
		if (symbol_vec.empty()) continue;
		product_valid_vec.push_back(product);
	}
	auto it = std::next(product_valid_vec.begin(), start_index);
	int row = 0;
	for (auto it3 = it; it3 != product_valid_vec.end(); it3++) {
		product_vec.push_back(*it3);
		row++;
		if (row == count) break;
	}
}
