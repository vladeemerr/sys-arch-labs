#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/clients/http/client.hpp>

using namespace userver;

class CartStorage;

class CartHandler final : public server::handlers::HttpHandlerJsonBase {
public:
	static constexpr std::string_view kName = "cart-handler";

	CartHandler(const components::ComponentConfig &,
	            const components::ComponentContext &);

	formats::json::Value
	HandleRequestJsonThrow(const server::http::HttpRequest &,
	                       const formats::json::Value &,
	                       server::request::RequestContext &) const override;

private:
	formats::json::Value addToCart(server::http::HttpResponse &,
	                               const std::string &login,
	                               const formats::json::Value &cart_item) const;
	formats::json::Value getCart(const server::http::HttpResponse &,
	                             const std::string &login) const;
	formats::json::Value removeFromCart(server::http::HttpResponse &,
	                                    const std::string &login,
	                                    const formats::json::Value &cart_item) const;
	
	clients::http::Client &http_client_;
	CartStorage &storage_;
};

