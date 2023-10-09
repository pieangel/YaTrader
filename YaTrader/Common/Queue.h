#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#include <chrono>
#include <atomic>
#include <cstddef>
#include <ctime>
#include <string>
#include <time.h>
#include <assert.h>
namespace DarkHorse {
	template <typename T>
	class Queue
	{
	public:

		T pop()
		{
			std::unique_lock<std::mutex> mlock(mutex_);
			while (queue_.empty())
			{
				cond_.wait(mlock);
			}
			auto item = queue_.front();
			queue_.pop();
			return item;
		}

		void pop(T& item)
		{
			std::unique_lock<std::mutex> mlock(mutex_);
			while (queue_.empty())
			{
				cond_.wait(mlock);
			}
			item = queue_.front();
			queue_.pop();
		}

		void push(const T& item)
		{
			std::unique_lock<std::mutex> mlock(mutex_);
			queue_.push(item);
			mlock.unlock();
			cond_.notify_one();
		}

		void push(T&& item)
		{
			std::unique_lock<std::mutex> mlock(mutex_);
			queue_.push(std::move(item));
			mlock.unlock();
			cond_.notify_one();
		}
		int getCount()
		{
			return (int)queue_.size();
		}
		bool empty()
		{
			return queue_.empty();
		}
	private:
		std::queue<T> queue_;
		std::mutex mutex_;
		std::condition_variable cond_;
	};
	class  ListnerParam;

	using ListnerPtr = std::shared_ptr<ListnerParam>;
	class  ListnerParam
	{
	public:
		ListnerParam(int size, std::string addr = "", unsigned int port = 0)
		{
			nSize = size;
			base_string = "";
			if (nSize > 0)
			{
				readData = new char[size + 1];
				memset(readData, 0, size + 1);
			}
			else
			{
				nSize = 0;
				readData = NULL;
			}
			Address = addr;
			recvPort = port;
			m_EventTime = 0;

		}
		~ListnerParam()
		{
			if (readData != NULL) delete[] readData;
		}
		ListnerParam& operator=(const ListnerParam& src)
		{
			nSize = src.nSize;
			base_string = src.base_string;
			readData = new char[nSize + 1];
			memset(readData, 0x00, nSize + 1);
			memcpy(readData, src.readData, nSize);
			Address = src.Address;
			recvPort = src.recvPort;
			m_EventTime = src.m_EventTime;
		}
		int nSize;								// data size
		char* readData;
		std::string Address;					// addr
		unsigned int	recvPort;				// port
		unsigned long m_EventTime;
		std::string base_string;
	};
	//#define threadsafe_queue  Queue
	template<typename T>
	class threadsafe_queue
	{
	private:
		struct node
		{
			std::shared_ptr<T> data;
			std::unique_ptr<node> next;
		};
		std::mutex head_mutex;
		std::unique_ptr<node> head;
		std::mutex tail_mutex;
		node* tail;
		std::condition_variable data_cond;
		unsigned int elementCount;
	public:
		threadsafe_queue() :
			head(new node), tail(head.get())
		{
			elementCount = 0;
		}
		threadsafe_queue(const threadsafe_queue& other) = delete;
		threadsafe_queue& operator=(const threadsafe_queue& other) = delete;
		void push(T new_value)
		{
			elementCount++;
			std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
			std::unique_ptr<node> p(new node);
			{
				std::lock_guard<std::mutex> tail_lock(tail_mutex);
				tail->data = new_data;
				node* const new_tail = p.get();
				tail->next = std::move(p);
				tail = new_tail;
			}
			data_cond.notify_one();
		}

	private:
		node* get_tail()
		{
			std::lock_guard<std::mutex> tail_lock(tail_mutex);
			return tail;
		}
		std::unique_ptr<node> pop_head()
		{
			std::unique_ptr<node> old_head = std::move(head);
			head = std::move(old_head->next);
			return old_head;
		}
		std::unique_lock<std::mutex> wait_for_data()
		{
			std::unique_lock<std::mutex> head_lock(head_mutex);
			data_cond.wait(head_lock, [&] {return head.get() != get_tail(); });
			return std::move(head_lock);
		}
		std::unique_ptr<node> wait_pop_head()
		{
			std::unique_lock<std::mutex> head_lock(wait_for_data());
			return pop_head();
		}
		std::unique_ptr<node> wait_pop_head(T& value)
		{
			std::unique_lock<std::mutex> head_lock(wait_for_data());
			value = std::move(*head->data);
			return pop_head();
		}
	public:
		std::shared_ptr<T> wait_and_pop()
		{
			std::unique_ptr<node> const old_head = wait_pop_head();
			return old_head->data;
		}
		void wait_and_pop(T& value)
		{
			std::unique_ptr<node> const old_head = wait_pop_head(value);
		}

	private:
		std::unique_ptr<node> try_pop_head()
		{
			std::lock_guard<std::mutex> head_lock(head_mutex);
			if (head.get() == get_tail())
			{
				return std::unique_ptr<node>();
			}
			return pop_head();
		}
		/// Remark Begin [2018-3-8  Debug by mnOh]
		//	std::unique_ptr<node> try_pop_head(T& value)
		bool try_pop_head(T& value)
		{
			std::lock_guard<std::mutex> head_lock(head_mutex);
			if (head.get() == get_tail())
			{
				return false;
			}
			value = std::move(*head->data);
			pop_head();
			return true;
		}
		T auto_pop_head()
		{
			std::lock_guard<std::mutex> head_lock(head_mutex);
			if (head.get() == get_tail())
			{
				return NULL;
			}
			T value = std::move(*head->data);
			pop_head();
			return value;
		}
	public:
		std::shared_ptr<T> try_pop()
		{
			std::unique_ptr<node> old_head = try_pop_head();
			if (elementCount != 0)	elementCount--;
			return old_head ? old_head->data : std::shared_ptr<T>();
		}
		//bool try_pop(T& value)
		//{
		//	std::unique_ptr<node> const old_head = try_pop_head(value);
		//	return old_head;
		//}
		bool try_pop(T& value)
		{
			if (elementCount != 0)	elementCount--;
			return try_pop_head(value);
		}
		bool pop(T& value)
		{
			if (elementCount != 0)	elementCount--;
			return try_pop_head(value);
		}
		auto pop()
		{
			if (elementCount != 0)	elementCount--;
			return auto_pop_head();
		}
		bool empty()
		{
			std::lock_guard<std::mutex> head_lock(head_mutex);
			return (head.get() == get_tail());
		}
		unsigned int getSize()
		{
			return elementCount;
		}
	};


	template <class T, size_t N>
	class LockFreeQueue {
	public:
		LockFreeQueue()
			: size_{ 0 }
			, read_pos_{ 0 }
			, write_pos_{ 0 } {
			assert(size_.is_lock_free());
		}

		auto size() const {
			return size_.load();
		}

		// Writer thread
		auto push(const T& t) {
			if (size_.load() >= N) {
				throw std::overflow_error("Queue is full");
			}
			buffer_[write_pos_] = t;
			write_pos_ = (write_pos_ + 1) % N;
			size_.fetch_add(1);
		}

		// Reader thread
		auto& front() const {
			auto s = size_.load();
			if (s == 0) {
				throw std::underflow_error("Queue is empty");
			}
			return buffer_[read_pos_];
		}

		// Reader thread
		auto pop() {
			if (size_.load() == 0) {
				throw std::underflow_error("Queue is empty");
			}
			read_pos_ = (read_pos_ + 1) % N;
			size_.fetch_sub(1);
		}

	private:
		std::array<T, N> buffer_{};  // Used by both threads
		std::atomic<size_t> size_{}; // Used by both threads
		size_t read_pos_ = 0;    // Used by reader thread
		size_t write_pos_ = 0;   // Used by writer thread
	};
}