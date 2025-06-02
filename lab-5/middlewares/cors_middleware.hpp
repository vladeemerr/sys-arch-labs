#pragma once

#include <userver/components/component.hpp>
#include <userver/server/middlewares/http_middleware_base.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/middlewares/configuration.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/formats/yaml.hpp>

struct CorsValues {
	std::string allow_origin;
	std::string allow_methods;
	std::string allow_headers;
};

class CorsMiddleware final : public userver::server::middlewares::HttpMiddlewareBase {
public:
	static constexpr std::string_view kName = "cors-middleware";

	explicit CorsMiddleware(const CorsValues &cors);

private:
	void HandleRequest(userver::server::http::HttpRequest &,
	                   userver::server::request::RequestContext &) const override;

	const CorsValues cors_;
};

class CorsMiddlewareFactory final : public userver::server::middlewares::HttpMiddlewareFactoryBase {
public:
	static constexpr std::string_view kName = "cors-middleware";

	using userver::server::middlewares::HttpMiddlewareFactoryBase::HttpMiddlewareFactoryBase;

	CorsMiddlewareFactory(const userver::components::ComponentConfig &,
	                      const userver::components::ComponentContext &);

	static userver::yaml_config::Schema GetStaticConfigSchema();

private:
	std::unique_ptr<userver::server::middlewares::HttpMiddlewareBase>
	Create(const userver::server::handlers::HttpHandlerBase &,
	       userver::yaml_config::YamlConfig) const override;

	const CorsValues cors_;
};
