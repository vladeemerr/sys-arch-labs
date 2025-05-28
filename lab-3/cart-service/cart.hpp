#pragma once

#include <optional>
#include <vector>

#include <userver/components/component_base.hpp>
#include <userver/rcu/rcu_map.hpp>
#include <userver/formats/json.hpp>

using namespace userver;

struct CartItem {
	int64_t id;
	int32_t quantity;
};

using Cart = std::vector<CartItem>;

class CartStorage final : public components::ComponentBase {
public:
	static constexpr std::string_view kName = "cart-storage";

	CartStorage(const components::ComponentConfig &,
	            const components::ComponentContext &);

	[[nodiscard]] Cart add(const std::string &login,
	                       const CartItem &item);
	[[nodiscard]] Cart get(const std::string &login) const;
	[[nodiscard]] std::optional<Cart> remove(const std::string &login,
	                                         const CartItem &item);

private:
	rcu::RcuMap<std::string, Cart> storage_;
};

CartItem Parse(const formats::json::Value &, formats::parse::To<CartItem>);
formats::json::Value Serialize(const CartItem &, formats::serialize::To<formats::json::Value>);
