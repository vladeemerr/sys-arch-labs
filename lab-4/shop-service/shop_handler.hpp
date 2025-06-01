#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>

using namespace userver;

class ProductStorage;

class ShopHandler final : public server::handlers::HttpHandlerJsonBase {
public:
	static constexpr std::string_view kName = "shop-handler";

	ShopHandler(const components::ComponentConfig &,
	            const components::ComponentContext &);

	formats::json::Value
	HandleRequestJsonThrow(const server::http::HttpRequest &,
	                       const formats::json::Value &,
	                       server::request::RequestContext &) const override;

private:
	formats::json::Value addProduct(server::http::HttpResponse &,
	                                const formats::json::Value &) const;
	formats::json::Value getProducts(server::http::HttpResponse &,
	                                 const std::string &filter) const;
	formats::json::Value getProduct(server::http::HttpResponse &,
	                                const std::string &id) const;
	formats::json::Value updateProduct(server::http::HttpResponse &,
	                                   const std::string &id,
	                                   const formats::json::Value &) const;
	formats::json::Value removeProduct(server::http::HttpResponse &,
	                                   const std::string &id) const;
	
	ProductStorage &storage_;
};

