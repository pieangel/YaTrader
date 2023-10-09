#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace DarkHorse {
	class SmProduct;
	class SmMarket
	{
	private:
		std::string _MarketName;
		std::map<std::string, std::shared_ptr<SmProduct>> _ProductMap;
	public:
		int GetValidProductCount();
		void GetProductVector(const int& start_index, const int& count, std::vector<std::shared_ptr<SmProduct>>& product_vec);
		const std::map<std::string, std::shared_ptr<SmProduct>>& GetProductMap() const { return _ProductMap; }
		std::shared_ptr<SmProduct> FindProduct(const std::string& product_code);
		std::shared_ptr<SmProduct> AddProduct(const std::string& product_code);
		SmMarket(const std::string& market_name) : _MarketName(market_name) {}

		std::string MarketName() const { return _MarketName; }
		void MarketName(std::string val) { _MarketName = val; }
	};
}

