#include "cors_middleware.hpp"

#include "userver/http/predefined_header.hpp"

namespace {

using namespace userver::http::headers;

constexpr PredefinedHeader allow_origin_header("Access-Control-Allow-Origin");
constexpr PredefinedHeader allow_methods_header("Access-Control-Allow-Methods");
constexpr PredefinedHeader allow_headers_header("Access-Control-Allow-Headers");

} // namespace

CorsMiddleware::CorsMiddleware(const CorsValues &cors) 
: cors_(cors) {}

void CorsMiddleware::HandleRequest(userver::server::http::HttpRequest &request,
                                   userver::server::request::RequestContext &context) const {
	auto &response = request.GetHttpResponse();

	response.SetHeader(allow_origin_header, cors_.allow_origin);
	response.SetHeader(allow_methods_header, cors_.allow_methods);
	response.SetHeader(allow_headers_header, cors_.allow_headers);

	if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
		request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
		return;
	}

	Next(request, context);
}

CorsMiddlewareFactory::CorsMiddlewareFactory(const userver::components::ComponentConfig &config,
                                             const userver::components::ComponentContext &context)
: userver::server::middlewares::HttpMiddlewareFactoryBase(config, context), 
  cors_{
	.allow_origin = config["allow-origin"].As<std::string>(),
	.allow_methods = config["allow-methods"].As<std::string>(),
	.allow_headers = config["allow-headers"].As<std::string>(),
  } {}

std::unique_ptr<userver::server::middlewares::HttpMiddlewareBase>
CorsMiddlewareFactory::Create(const userver::server::handlers::HttpHandlerBase &,
                              userver::yaml_config::YamlConfig) const {
	return std::make_unique<CorsMiddleware>(cors_);
}

userver::yaml_config::Schema CorsMiddlewareFactory::GetStaticConfigSchema() {
	return userver::formats::yaml::FromString(R"(
type: object
description: CORS config
additionalProperties: false
properties:
  allow-origin:
    type: string
    description: Which origin to allow for CORS
  allow-methods:
    type: string
    description: Which methods to allow for CORS
  allow-headers:
     type: string
     description: Which headers to allow for CORS
)")
	.As<userver::yaml_config::Schema>();
}
