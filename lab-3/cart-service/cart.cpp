#include "cart.hpp"
#include "userver/formats/json/value_builder.hpp"

CartStorage::CartStorage(const components::ComponentConfig &config,
                         const components::ComponentContext &context)
: components::ComponentBase(config, context) {}

Cart CartStorage::add(const std::string &login,
                      const CartItem &item) {
	auto result = storage_.Get(login);

	if (result == nullptr) {
		auto cart = std::vector<CartItem>{item};
		storage_.Emplace(login, cart);
		return cart;
	}

	for (auto &i : *result) {
		if (i.id == item.id) {
			i.quantity += item.quantity;
			return *result;
		}
	}

	result->push_back(item);

	return *result;
}

Cart CartStorage::get(const std::string &login) const {
	const auto result = storage_.Get(login);

	if (result == nullptr)
		return {};

	return *result;
}

std::optional<Cart> CartStorage::remove(const std::string &login,
                                        const CartItem &item) {
	const auto result = storage_.Get(login);

	if (result == nullptr)
		return std::nullopt;

	for (auto &it : *result) {
		if (it.id == item.id) {
			it.quantity -= item.quantity;

			if (it.quantity <= 0) {
				storage_.Erase(login);
				return {};
			}

			return *result;
		}
	}
	
	return std::nullopt;
}

CartItem Parse(const formats::json::Value &value, formats::parse::To<CartItem>) {
	return CartItem{
		.id = value["id"].As<int64_t>(),
		.quantity = value["quantity"].As<int32_t>(),
	};
}

formats::json::Value Serialize(const CartItem &item, formats::serialize::To<formats::json::Value>) {
	formats::json::ValueBuilder builder;
	builder["id"] = item.id;
	builder["quantity"] = item.quantity;
	return builder.ExtractValue();
}
