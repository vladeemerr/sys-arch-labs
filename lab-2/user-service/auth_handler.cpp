#include "auth_handler.hpp"

#include <userver/components/component_context.hpp>
#include "userver/server/handlers/exceptions.hpp"
#include <jwt-cpp/jwt.h>

#include "user.hpp"
#include "middlewares/auth_middleware.hpp"

namespace {

constexpr uint64_t token_lifespan = 3600;

} // namespace

struct AuthCredentials {
	std::string login;
	std::string password;
};

AuthHandler::AuthHandler(const components::ComponentConfig &config,
                         const components::ComponentContext &context)
: server::handlers::HttpHandlerJsonBase(config, context),
  storage_(context.FindComponent<UserStorage>()) {}

formats::json::Value
AuthHandler::HandleRequestJsonThrow(const server::http::HttpRequest &,
                                    const formats::json::Value &body,
                                    server::request::RequestContext &) const {
	if (!body.HasMember("login") || !body.HasMember("password")) {
		throw server::handlers::RequestParseError();
	}

	AuthCredentials creds = {
		.login = body["login"].As<std::string>(),
		.password = body["password"].As<std::string>(),
	};

	if (creds.login.empty() || creds.password.empty()) {
		throw server::handlers::ClientError();
	}

	creds.password = User::encrypt(creds.password);

	const auto query = storage_.getUser(creds.login);

	if (!query.has_value()) {
		throw server::handlers::ResourceNotFound();
	}

	if (query->password != creds.password) {
		throw server::handlers::Unauthorized();
	}

	const auto expires_on = std::chrono::system_clock::now() + 
	                        std::chrono::seconds(token_lifespan);

	auto token = jwt::create().set_issuer(token_issuer)
	                          .set_type("JWT")
	                          .set_subject(query->login)
	                          .set_issued_now()
	                          .set_expires_at(expires_on)
	                          .sign(jwt::algorithm::hs256(token_secret));

	formats::json::ValueBuilder result;

	result["token"] = token;
	result["type"] = "Bearer";
	result["expires_on"] = std::to_string(std::chrono::system_clock::to_time_t(expires_on));

	return result.ExtractValue();
}
