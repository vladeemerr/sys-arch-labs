#include "cart_handler.hpp"

#include <exception>
#include <string>

#include <userver/http/status_code.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/clients/http/component.hpp>

#include "cart.hpp"
#include "userver/clients/http/error.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/logging/log.hpp"

CartHandler::CartHandler(const components::ComponentConfig &config,
                         const components::ComponentContext &context)
: server::handlers::HttpHandlerJsonBase(config, context),
  http_client_(context.FindComponent<components::HttpClient>().GetHttpClient()),
  storage_(context.FindComponent<CartStorage>()) {}

formats::json::Value
CartHandler::HandleRequestJsonThrow(const server::http::HttpRequest &request,
                                    const formats::json::Value &body,
                                    server::request::RequestContext &context) const {
	auto &response = request.GetHttpResponse();
	
	const std::string login = context.GetData<std::string>("login");

	switch (request.GetMethod()) {
	case server::http::HttpMethod::kGet:
		return getCart(response, login);

	case server::http::HttpMethod::kPost:
		return addToCart(response, login, body);

	case server::http::HttpMethod::kDelete:
		return removeFromCart(response, login, body);

	default: {
		throw server::handlers::ClientError();
	} break;
	}
}

formats::json::Value CartHandler::addToCart(server::http::HttpResponse &,
	                                        const std::string &login,
	                                        const formats::json::Value &cart_item) const {
	if (login.empty()) {
		throw server::handlers::ClientError();
	}

	CartItem item = cart_item.As<CartItem>();

	{
		auto query = http_client_.CreateRequest()
		             .get("http://shop-service:8080/shop/" + item.id)
		             .timeout(1000)
		             .perform();

		if (!query->IsOk()) {
			throw server::handlers::ResourceNotFound();
		}
	}

	if (item.quantity <= 0) {
		throw server::handlers::ClientError();
	}

	Cart cart = storage_.add(login, item);

	formats::json::ValueBuilder result;

	for (const auto &i : cart)
		result.PushBack(i);

	return result.ExtractValue();
}

formats::json::Value CartHandler::getCart(const server::http::HttpResponse &,
                                          const std::string &login) const {
	if (login.empty()) {
		throw server::handlers::ClientError();
	}

	formats::json::ValueBuilder result;

	const auto items = storage_.get(login);
	for (const auto &it : items)
		result.PushBack(it);

	return result.ExtractValue();
}


formats::json::Value CartHandler::removeFromCart(server::http::HttpResponse &,
	                                             const std::string &login,
	                                             const formats::json::Value &cart_item) const {
	CartItem item = cart_item.As<CartItem>();

	if (item.quantity <= 0) {
		throw server::handlers::ClientError();
	}

	const auto result = storage_.remove(login, item);

	if (!result.has_value()) {
		throw server::handlers::ResourceNotFound();
	}

	formats::json::ValueBuilder builder;

	for (const auto &i : result.value())
		builder.PushBack(i);

	return builder.ExtractValue();
}

