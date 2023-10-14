#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <map>
#include <set>
#include <vector>
#include "MarketDefine.h"
// 국내 자산 - 국내 선물, 국내 옵션, 주식 선물, ETF
// 해외 자산 - 해외 선물 - 금리, 지수, 오일, 금속, 채권
namespace DarkHorse {
	class SmSymbol;
	class SmMarket;
	class SmProduct;
	class SmSymbolManager
	{
	public:
		SmSymbolManager();
		~SmSymbolManager();
		// Static Members
		static int _Id;
		static int GetId() { return ++_Id; }
	private:
		std::set<std::string> Ab_Market_Set_;
		/// <summary>
		/// Symbol Map.
		/// Key : Symbol Code, value : Symbol Object.
		/// </summary>
		std::map<std::string, std::shared_ptr<SmSymbol>> SymbolMap_;
		// key : unique symbol id, value : SmSymbol Object
		std::map<int, std::shared_ptr<SmSymbol>> _SymbolIdMap;
		// key : market name, value : Market Object
		std::map<std::string, std::shared_ptr<SmMarket>> _MarketMap;
		std::set<std::string> _RegisteredSymbolMap;
		// key : symbol unique id, value : SmSymbol Object
		std::map<int, std::shared_ptr<SmSymbol>> _FavoriteMap;
		std::set<std::string> _FavoriteProduct;
		// key : domestic product code, value : symbol code list
		std::map<std::string, std::vector<std::string>> _DomesticSymbolCodeMap;
		std::vector<std::string> _DomesticProductVec;
		std::vector<DmFuture> _DomesticFutureVec;
		std::vector<DmOption> _DomesticOptionVec;
		std::map<std::string, std::string> ya_market_code_map_;
	private:
		void market_code_map_init_dm_ya();
		void InitDomesticProducts();
		void add_to_yearmonth(std::shared_ptr<SmSymbol> symbol);
		void add_to_yearmonth_dm_ya(std::shared_ptr<SmSymbol> symbol);
		void set_product_info(std::shared_ptr<SmSymbol> symbol);
		void set_quote_preday_close(std::shared_ptr<SmSymbol> symbol, const std::string& pre_day_str);
	public:
		void set_domestic_symbol_info(std::shared_ptr<SmSymbol> symbol);
		void get_ab_recent_symbols(std::set<std::shared_ptr<SmSymbol>>& ab_symbol_set);
		const std::map<int, std::shared_ptr<SmSymbol>>& get_ab_favorite_map() {
			return _FavoriteMap;
		}
		void add_ab_market(const std::string& market_name) {
			Ab_Market_Set_.insert(market_name);
		}
		const std::set<std::string>& get_ab_market_set() {
			return Ab_Market_Set_;
		}
		const std::vector<DmFuture>& get_dm_future_vec()
		{
			return _DomesticFutureVec;
		}
		std::vector<DmOption>& get_dm_option_vec()
		{
			return _DomesticOptionVec;
		}
		const std::vector<std::string> GetDomesticProductVec() {
			return _DomesticProductVec;
		}
		std::shared_ptr<SmMarket> get_dm_market_by_product_code(const std::string& product_code);
		void sort_dm_option_symbol_vector();
		void read_dm_masterfile_si();
		void read_dm_masterfile_ya();
		void read_domestic_productfile();
		void AddDomesticSymbolCode(const std::string& product_code, const std::string& symbol_code);
		void MakeAbFavorite();
		void InitFavoriteProduct();
		void Clear() {
			
		}
		const std::map<int, std::shared_ptr<SmSymbol>>& GetFavoriteMap() {
			return _FavoriteMap;
		}
		void RegisterFavoriteSymbols();
		void AddFavorite(const int& symbol_id);
		void RemoveFavorite(const std::string& symbol_code);
		std::shared_ptr<SmSymbol> FindSymbolById(const int& id);
		void AddSymbol(std::shared_ptr<SmSymbol> symbol);
		std::shared_ptr<SmSymbol> FindSymbol(const std::string& symbol_code) const;
		int get_symbol_id(const std::string& symbol_code);
		void GetRecentSymbolVector(std::vector<std::shared_ptr<SmSymbol>>& symbol_list);
		std::shared_ptr<SmSymbol> GetRecentSymbol(const std::string& product_code);
		std::string GetRecentSymbolCode(const std::string& product_code);
		std::string GetNextSymbolCode(const std::string& product_code);
		const std::map<std::string, std::shared_ptr<SmMarket>>& GetMarketMap() const { return _MarketMap; }
		std::shared_ptr<SmProduct> FindProduct(const std::string& product_code);
		std::shared_ptr<SmMarket> AddMarket(const std::string& market_name);
		std::shared_ptr<SmMarket> FindMarket(const std::string& market_name);
		std::shared_ptr<SmMarket> get_market(const std::string& market_name);
		void ReadAbroadSymbols() const noexcept;
		void MakeDomesticMarket();
		void RegisterSymbolToServer(const std::string& symbol_code, const bool& reg);
		std::shared_ptr<SmProduct> find_product(
			const std::string& market_name, 
			const std::string& product_code);
	};
}

