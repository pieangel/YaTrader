#pragma once
#include <string>
#include <memory>
namespace DarkHorse {

	const std::string DmFutureMarketName = "국내선물";
	const std::string DmOptionMarketName = "국내옵션";
	class SmProduct;
	enum class ValueType {
		None,
		Future,
		KospiOption,
		MiniKospiOption,
		Kosdaq,
		KospiWeekly
	};
struct DmFuture {
	bool registered{ false };
	std::string future_name;
	std::string product_code;
	std::shared_ptr<SmProduct> product;
};
struct DmOption {
	bool registered{ false };
	std::string option_name;
	std::shared_ptr<SmProduct> call_product;
	std::shared_ptr<SmProduct> put_product;
};
}