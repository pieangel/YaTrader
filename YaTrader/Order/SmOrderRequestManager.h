#pragma once
#include <memory>
#include <map>
#include "../Common/common.h"
#include "../Common/BlockingCollection.h"
#include "SmOrderConst.h"
using namespace code_machina;
namespace DarkHorse {
	struct SmOrderRequest;
	class SmOrderRequestManager : public Runnable
	{
	public:
		SmOrderRequestManager();
		virtual ~SmOrderRequestManager();

		// Static Members
		static int _Id;
		static int GetId() { return _Id++; }

		std::shared_ptr<SmOrderRequest> FindOrderRequest(const int& req_id);
		void AddOrderRequest(const std::shared_ptr<SmOrderRequest>& order_req);
		void RemoveOrderRequest(const int& req_id);

		virtual unsigned int ThreadHandlerProc(void);


		static std::shared_ptr<SmOrderRequest> MakeOrderRequest(
			const int& order_price, 
			const int& order_amount, 
			const std::string& symbol_code, 
			const SmPositionType& position_type,
			const SmOrderType& order_type, 
			const SmPriceType& price_type, 
			const SmFilledCondition& fill_cond = SmFilledCondition::Day);

		static std::shared_ptr<SmOrderRequest> MakeOrderRequest(
			const std::string account_no,
			const std::string password,
			const int& order_price,
			const int& order_amount,
			const std::string& symbol_code,
			const SmPositionType& position_type,
			const SmOrderType& order_type,
			const SmPriceType& price_type,
			const SmFilledCondition& fill_cond = SmFilledCondition::Day);

		static std::shared_ptr<SmOrderRequest> MakeDefaultSellOrderRequest(
			const std::string& symbol_code,
			const int& order_price,
			const SmPositionType& position_type = SmPositionType::Sell,
			const int& order_amount = 1,
			const SmOrderType& order_type = SmOrderType::New,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_cond = SmFilledCondition::Day);

		static std::shared_ptr<SmOrderRequest> MakeDefaultSellOrderRequest(
			const std::string account_no,
			const std::string password,
			const std::string& symbol_code,
			const int& order_price,

			const int& order_amount = 1,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmPositionType& position_type = SmPositionType::Sell,
			
			const SmOrderType& order_type = SmOrderType::New,
			
			const SmFilledCondition& fill_cond = SmFilledCondition::Day);

		static std::shared_ptr<SmOrderRequest> MakeDefaultBuyOrderRequest(
			const std::string& symbol_code,
			const int& order_price,
			const SmPositionType& position_type = SmPositionType::Buy,
			const int& order_amount = 1,
			const SmOrderType& order_type = SmOrderType::New,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_cond = SmFilledCondition::Day);

		static std::shared_ptr<SmOrderRequest> MakeDefaultBuyOrderRequest(
			const std::string account_no,
			const std::string password,
			const std::string& symbol_code,
			const int& order_price,
			const int& order_amount = 1,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmPositionType& position_type = SmPositionType::Buy,
			
			const SmOrderType& order_type = SmOrderType::New,
			
			const SmFilledCondition& fill_cond = SmFilledCondition::Day);

		static std::shared_ptr<SmOrderRequest> MakeChangeOrderRequest(
			const std::string account_no,
			const std::string password,
			const std::string& symbol_code,
			const std::string& ori_order_no,
			const int& order_price,
			const SmPositionType& position_type = SmPositionType::Buy,
			const int& order_amount = 1,
			const SmOrderType& order_type = SmOrderType::Modify,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_cond = SmFilledCondition::Day);

		static std::shared_ptr<SmOrderRequest> MakeCancelOrderRequest(
			const std::string account_no,
			const std::string password,
			const std::string& symbol_code,
			const std::string& ori_order_no,
			const int& order_price,
			const SmPositionType& position_type = SmPositionType::Buy,
			const int& order_amount = 1,
			const SmOrderType& order_type = SmOrderType::Cancel,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_cond = SmFilledCondition::Day);
	private:
		// key : order request id, value : order request object
		std::map<int, std::shared_ptr<SmOrderRequest>> _OrderReqMap;
	};
}

