#include "product.hpp"
#include <optional>

ProductStorage::ProductStorage(const components::ComponentConfig &config,
                               const components::ComponentContext &context)
: components::ComponentBase(config, context) {}

std::optional<Product> ProductStorage::addProduct(const ProductInfo &info) {
	int64_t id = storage_.SizeApprox();

	auto result = storage_.Emplace(id, info);

	return result.inserted ? std::make_optional(Product{id, *result.value})
	                       : std::nullopt;
}

std::optional<Product> ProductStorage::getProduct(int64_t id) const {
	const auto result = storage_.Get(id);

	if (result == nullptr)
		return std::nullopt;

	return std::make_optional(Product{id, *result});
}

std::vector<Product> ProductStorage::getProducts() const {
	std::vector<Product> result;

	for (const auto &[k, v] : storage_)
		result.push_back(Product{k, *v});

	return result;
}

std::optional<Product> ProductStorage::updateProduct(int64_t id,
                                                     const ProductInfo &info) {
	const auto result = storage_.Get(id);

	if (result == nullptr)
		return std::nullopt;

	*result = info;

	return std::make_optional(Product{id, *result});
}

bool ProductStorage::removeProduct(int64_t id) {
	return storage_.Erase(id);
}

ProductInfo Parse(const formats::json::Value &value, formats::parse::To<ProductInfo>) {
	return ProductInfo{
		.name = value["name"].As<std::string>(),
		.description = value["description"].As<std::string>(),
		.quantity = value["quantity"].As<int32_t>(),
		.price = value["price"].As<int32_t>(),
	};
}

formats::json::Value Serialize(const Product &product,
                               formats::serialize::To<formats::json::Value>) {
	formats::json::ValueBuilder result;
	result["id"] = product.id;
	result["name"] = product.info.name;
	result["description"] = product.info.description;
	result["quantity"] = product.info.quantity;
	result["price"] = product.info.price;
	return result.ExtractValue();
}
