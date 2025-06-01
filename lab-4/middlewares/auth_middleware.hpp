#pragma once

#include <userver/server/middlewares/http_middleware_base.hpp>
#include <userver/server/middlewares/configuration.hpp>

using namespace userver;

constexpr inline char token_issuer[] = "shop";
constexpr inline char token_secret[] = "shadow";

class AuthMiddleware final : public server::middlewares::HttpMiddlewareBase {
public:
	explicit AuthMiddleware(yaml_config::YamlConfig);

private:
	void HandleRequest(server::http::HttpRequest &,
	                   server::request::RequestContext &) const override;

	const std::vector<std::string> public_methods_;
};

class AuthMiddlewareFactory final : public server::middlewares::HttpMiddlewareFactoryBase {
public:
	static constexpr std::string_view kName = "auth-middleware";

	using HttpMiddlewareFactoryBase::HttpMiddlewareFactoryBase;

private:
	std::unique_ptr<server::middlewares::HttpMiddlewareBase>
	Create(const server::handlers::HttpHandlerBase &,
	       yaml_config::YamlConfig) const override;

	yaml_config::Schema GetMiddlewareConfigSchema() const override;
};

class AuthPipelineBuilder final : public server::middlewares::HandlerPipelineBuilder {
public:
	using HandlerPipelineBuilder::HandlerPipelineBuilder;

	server::middlewares::MiddlewaresList
	BuildPipeline(server::middlewares::MiddlewaresList) const override;
};
