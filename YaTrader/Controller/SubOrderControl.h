#pragma once
#include <map>
#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include "../ViewModel/VmOrder.h"
#include "../Order/SmOrderConst.h"
#include "../Order/Order.h"
#include <algorithm>
namespace DarkHorse {
	class PriceOrderMap {
	private:
		int price_{ 0 };
		size_t count_{ 0 };
		// key : order no, value : accepted order object.
		std::map<std::string, std::shared_ptr<Order>> order_map_;
		std::mutex mutex_; // Mutex for thread synchronization

		std::vector<std::shared_ptr<Order>> test;
	public:
		void clear() {
			std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex

			order_map_.clear();
			count_ = 0;
		}
		int get_price() const { return price_; }
		void set_price(const int price) {
			price_ = price;
		}
		size_t count() const { return count_; }

		void get_order(std::vector<std::shared_ptr<Order>>& order_vector) {
			std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex

			order_vector.clear();
			for (auto it : order_map_) {
				order_vector.push_back(it.second);
			}
		}
		void add_order(std::shared_ptr<Order> order) {
			std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex

			if (order->order_price != price_) return;
			order_map_[order->order_no] = order;
			count_ = order_map_.size();
		}
		void remove_order(const std::string& order_no) {
			std::lock_guard<std::mutex> lock(mutex_); // Lock the mutex

			auto it = order_map_.find(order_no);
			if (it == order_map_.end()) return;
			order_map_.erase(it);
			count_ = order_map_.size();
		}
	};
	/// <summary>
	/// 메인계좌 일 때는 메인 계좌와 서브 계좌의 모든 주문을 저장해야 한다.
	/// 펀드 일 때는 펀드에 속한 모든 계좌의 주문을 저장해야 한다. 
	/// 컨트롤에 각 계좌의 id 맵을 가지고 들어오는 주문을 필터링 해야 한다. 
	/// </summary>
	class SubOrderControl
	{
	private:
		SubOrderControlType control_type_{ CT_NONE };
		size_t total_count_{ 0 };
		// key : price as integer, value : order list on the price. 
		std::map<int, std::shared_ptr<PriceOrderMap>> order_map_;
	public:
		void clear() {
			for (auto& it : order_map_) {
				it.second->clear();
			}
			order_map_.clear();
			total_count_ = 0;
		};

		void get_order(const int& order_price, std::vector<std::shared_ptr<Order>>& order_vector);
		void clear_order(const int& order_price);
		void set_control_type(const SubOrderControlType control_type) {
			control_type_ = control_type;
		}
		int total_count() const { return total_count_; }
		const std::map<int, std::shared_ptr<PriceOrderMap>>& get_order_map() {
			return order_map_;
		}
		void add_order(const int order_price, std::shared_ptr<Order> order) {
			auto it = order_map_.find(order_price);
			if (it == order_map_.end()) {
				std::shared_ptr<PriceOrderMap> price_order_map_p = std::make_shared<PriceOrderMap>();;
				price_order_map_p->set_price(order_price);
				price_order_map_p->add_order(order);
				order_map_[order_price] = price_order_map_p;
			}
			else {
				std::shared_ptr<PriceOrderMap> price_order_map = it->second;
				price_order_map->add_order(order);
			}
			calculate_total_count();
		}

		void remove_order(const int order_price, const std::string& order_no) {
			auto it = order_map_.find(order_price);
			if (it == order_map_.end()) return;
			std::shared_ptr<PriceOrderMap> price_order_map = it->second;
			price_order_map->remove_order(order_no);
			calculate_total_count();
		}

		void calculate_total_count() {
			total_count_ = 0;
			for (auto it : order_map_)
				total_count_ += it.second->count();
		}
	};
}

