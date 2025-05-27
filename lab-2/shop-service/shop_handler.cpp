#include "shop_handler.hpp"

#include <string>

#include <userver/crypto/hash.hpp>
#include <userver/http/status_code.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/exceptions.hpp>

#include "product.hpp"

ShopHandler::ShopHandler(const components::ComponentConfig &config,
                         const components::ComponentContext &context)
: server::handlers::HttpHandlerJsonBase(config, context),
  storage_(context.FindComponent<ProductStorage>()) {}

formats::json::Value
ShopHandler::HandleRequestJsonThrow(const server::http::HttpRequest &request,
                                    const formats::json::Value &body,
                                    server::request::RequestContext &) const {
	auto &response = request.GetHttpResponse();

	switch (request.GetMethod()) {
	case server::http::HttpMethod::kGet: {
		if (request.HasPathArg(0)) {
			const std::string id = request.GetPathArg(0);
			return getProduct(response, id);
		} else {
			return getProducts(response);
		}
	} break;

	case server::http::HttpMethod::kPost:
		return addProduct(response, body);

	case server::http::HttpMethod::kPut: {
		const std::string id = request.GetPathArg(0);
		return updateProduct(response, id, body);
	} break;

	case server::http::HttpMethod::kDelete: {
		const std::string id = request.GetPathArg(0);
		return removeProduct(response, id);
	} break;

	default: {
		throw server::handlers::ClientError();
	} break;
	}
}

formats::json::Value ShopHandler::addProduct(server::http::HttpResponse &response,
                                             const formats::json::Value &body) const {
	ProductInfo info = body.As<ProductInfo>();
	
	if (info.name.empty()) {
		throw server::handlers::ClientError();
	}

	const auto result = storage_.addProduct(info);

	if (!result.has_value()) {
		throw server::handlers::ConflictError();
	}

	response.SetStatus(http::kCreated);
	return formats::json::ValueBuilder(result.value()).ExtractValue();
}

formats::json::Value ShopHandler::getProducts(server::http::HttpResponse &) const {
	formats::json::ValueBuilder result;

	const auto products = storage_.getProducts();
	for (const auto &product : products)
		result.PushBack(product);

	return result.ExtractValue();
}

formats::json::Value ShopHandler::getProduct(server::http::HttpResponse &,
                                             const std::string &id) const {
	if (id.empty()) {
		throw server::handlers::ClientError();
	}

	int64_t nid = std::stol(id);
	const auto result = storage_.getProduct(nid);

	if (!result.has_value()) {
		throw server::handlers::ResourceNotFound();
	}

	return formats::json::ValueBuilder(result.value()).ExtractValue();
}

formats::json::Value ShopHandler::updateProduct(server::http::HttpResponse &,
                                                const std::string &id,
                                                const formats::json::Value &body) const {
	ProductInfo info = body.As<ProductInfo>();

	if (info.name.empty() || id.empty()) {
		throw server::handlers::ClientError();
	}

	int64_t nid = std::stol(id);

	const auto result = storage_.updateProduct(nid, info);
	if (!result.has_value()) {
		throw server::handlers::ResourceNotFound();
	}

	return formats::json::ValueBuilder(result.value()).ExtractValue();
}

formats::json::Value ShopHandler::removeProduct(server::http::HttpResponse &,
                                                const std::string &id) const {
	if (id.empty()) {
		throw server::handlers::ClientError();
	}

	int64_t nid = std::stol(id);

	if (!storage_.removeProduct(nid)) {
		throw server::handlers::ResourceNotFound();
	}

	return formats::json::ValueBuilder("Successfuly removed product listing").ExtractValue();
}

