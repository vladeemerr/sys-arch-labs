#pragma once

#include <optional>
#include <vector>

#include <userver/components/component_base.hpp>
#include <userver/rcu/rcu_map.hpp>
#include <userver/formats/json.hpp>

using namespace userver;

struct ProductInfo {
	std::string name;
	std::string description;
	int32_t quantity;
	int32_t price;
};

struct Product {
	int64_t id;
	ProductInfo info;
};

class ProductStorage final : public components::ComponentBase {
public:
	static constexpr std::string_view kName = "product-storage";

	ProductStorage(const components::ComponentConfig &,
	               const components::ComponentContext &);

	[[nodiscard]] std::optional<Product> addProduct(const ProductInfo &info);
	[[nodiscard]] std::optional<Product> getProduct(int64_t id) const;
	[[nodiscard]] std::vector<Product> getProducts() const;
	[[nodiscard]] std::optional<Product> updateProduct(int64_t id,
	                                                   const ProductInfo &info);
	[[nodiscard]] bool removeProduct(int64_t id);

private:
	rcu::RcuMap<int64_t, ProductInfo> storage_;
};

ProductInfo Parse(const formats::json::Value &, formats::parse::To<ProductInfo>);
formats::json::Value Serialize(const Product &, formats::serialize::To<formats::json::Value>);
