#include "auth_middleware.hpp"

#include <userver/formats/yaml.hpp>
#include <userver/yaml_config/yaml_config.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/request/request_context.hpp>

#include <jwt-cpp/jwt.h>

AuthMiddleware::AuthMiddleware(yaml_config::YamlConfig config)
: public_methods_(config["public-methods"].As<std::vector<std::string>>()) {}

void AuthMiddleware::HandleRequest(server::http::HttpRequest &request,
                                   server::request::RequestContext &context) const {
	const auto &method = request.GetMethodStr();
	for (const auto &m : public_methods_) {
		if (method == m) {
			Next(request, context);
			return;
		}
	}

	const auto &header = request.GetHeader("Authorization");
	if (header.empty() || !header.starts_with("Bearer ")) {
		throw server::handlers::Unauthorized();
	}

	std::string token = header.substr(7);

	try {
		auto decoded = jwt::decode(token);
		auto verifier = jwt::verify().allow_algorithm(jwt::algorithm::hs256(token_secret))
		                             .with_issuer(token_issuer);

		verifier.verify(decoded);

		context.SetData("login", decoded.get_subject());

		Next(request, context);
	} catch (const jwt::error::token_verification_exception &) {
		throw server::handlers::Unauthorized();
	} catch (const std::exception &) {
		throw server::handlers::RequestParseError();
	}
}

std::unique_ptr<server::middlewares::HttpMiddlewareBase>
AuthMiddlewareFactory::Create(const server::handlers::HttpHandlerBase &,
                              yaml_config::YamlConfig config) const {
	return std::make_unique<AuthMiddleware>(config);
}

yaml_config::Schema AuthMiddlewareFactory::GetMiddlewareConfigSchema() const {
	return formats::yaml::FromString(R"(
type: object
description: Authentication middleware config
additionalProperties: false
properties:
  public-methods:
    type: array
    description: Which methods are not protected
    items:
      type: string
      description: Method in caps (e.g. GET)
)")
	.As<yaml_config::Schema>();
}

server::middlewares::MiddlewaresList
AuthPipelineBuilder::BuildPipeline(server::middlewares::MiddlewaresList list) const {
	auto &pipeline = list;
	pipeline.emplace_back(AuthMiddlewareFactory::kName);
	return pipeline;
}
