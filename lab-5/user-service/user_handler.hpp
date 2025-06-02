#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>

using namespace userver;

class UserStorage;

class UserHandler final : public server::handlers::HttpHandlerJsonBase {
public:
	static constexpr std::string_view kName = "user-handler";

	UserHandler(const components::ComponentConfig &,
	            const components::ComponentContext &);

	formats::json::Value
	HandleRequestJsonThrow(const server::http::HttpRequest &,
	                       const formats::json::Value &,
	                       server::request::RequestContext &) const override;

private:
	formats::json::Value registerUser(server::http::HttpResponse &,
	                                  const formats::json::Value &) const;
	formats::json::Value getUsers(server::http::HttpResponse &,
	                              const std::string_view first_name,
	                              const std::string_view last_name) const;
	formats::json::Value getUser(server::http::HttpResponse &,
	                             const std::string &login) const;
	formats::json::Value updateUser(server::http::HttpResponse &, 
	                                const std::string &login,
	                                const formats::json::Value &) const;
	formats::json::Value deleteUser(server::http::HttpResponse &,
	                                const std::string &login) const;
	
	UserStorage &storage_;
};

