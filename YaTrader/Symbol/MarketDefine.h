#pragma once
#include <string>
#include <memory>
namespace DarkHorse {

	const std::string DmFutureMarketName = "��������";
	const std::string DmOptionMarketName = "�����ɼ�";
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