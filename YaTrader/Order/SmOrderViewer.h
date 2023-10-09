#pragma once
#include <memory>
#include <map>
#include <vector>
#include <mutex>
namespace DarkHorse {
	class SmOrder;
	class SmOrderViewer
	{
	public:
		void Clear();
		void AddOrder(std::shared_ptr<SmOrder> order);
		const std::map<int, std::map<std::string, std::shared_ptr<SmOrder>>>& GetOrderMap() {
			return _OrderMap;
		}
		void RemoveOrder(std::shared_ptr<SmOrder> order);
		void RemoveOrder(const int& price);
		void UpdateOrder(std::map<int, int>& order_map);
		void GetOrderVec(const int& price, std::map<std::string, std::shared_ptr<SmOrder>>& order_vec);
		size_t GetOrderCount(const int& price);
		int GetTotalCount();
	private:
		/// <summary>
		/// Key : order price, order object map
		/// </summary>
		std::map<int, std::map<std::string, std::shared_ptr<SmOrder>>> _OrderMap;
		std::mutex _m;
	};
}

