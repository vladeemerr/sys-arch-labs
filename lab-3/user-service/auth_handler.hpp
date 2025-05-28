#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>

using namespace userver;

class UserStorage;

class AuthHandler final : public server::handlers::HttpHandlerJsonBase {
public:
	static constexpr std::string_view kName = "auth-handler";

	AuthHandler(const components::ComponentConfig &,
	            const components::ComponentContext &);

	formats::json::Value
	HandleRequestJsonThrow(const server::http::HttpRequest &,
	                       const formats::json::Value &,
	                       server::request::RequestContext &) const override;

private:
	const UserStorage &storage_;
};
