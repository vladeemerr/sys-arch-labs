#pragma once

#include "userver/formats/bson/value.hpp"
#include <optional>
#include <vector>

#include <userver/components/component_base.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/formats/json.hpp>

using namespace userver;

struct ProductInfo {
	std::string name;
	std::string description;
	int32_t quantity;
	int32_t price;
};

struct Product {
	std::string id;
	ProductInfo info;
};

class ProductStorage final : public components::ComponentBase {
public:
	static constexpr std::string_view kName = "product-storage";

	ProductStorage(const components::ComponentConfig &,
	               const components::ComponentContext &);

	[[nodiscard]] std::string addProduct(const ProductInfo &info);
	[[nodiscard]] std::optional<Product> getProduct(const std::string &id) const;
	[[nodiscard]] std::vector<Product> getProducts(const std::string &filter) const;
	[[nodiscard]] std::optional<Product> updateProduct(const std::string &id,
	                                                   const ProductInfo &info);
	[[nodiscard]] bool removeProduct(const std::string &id);

private:
	storages::mongo::PoolPtr storage_;
};

ProductInfo Parse(const formats::json::Value &, formats::parse::To<ProductInfo>);
formats::json::Value Serialize(const Product &, formats::serialize::To<formats::json::Value>);

formats::bson::Document Serialize(const Product &, formats::serialize::To<formats::bson::Document>);
