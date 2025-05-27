#include "user_handler.hpp"

#include <userver/crypto/hash.hpp>
#include <userver/http/status_code.hpp>
#include <userver/components/component_context.hpp>

#include "user.hpp"
#include "userver/server/handlers/exceptions.hpp"

UserHandler::UserHandler(const components::ComponentConfig &config,
                         const components::ComponentContext &context)
: server::handlers::HttpHandlerJsonBase(config, context),
  storage_(context.FindComponent<UserStorage>()) {}

formats::json::Value
UserHandler::HandleRequestJsonThrow(const server::http::HttpRequest &request,
                                    const formats::json::Value &body,
                                    server::request::RequestContext &) const {
	auto &response = request.GetHttpResponse();

	switch (request.GetMethod()) {
	case server::http::HttpMethod::kGet: {
		if (request.HasPathArg(0)) {
			const std::string login = request.GetPathArg(0);
			return getUser(response, login);
		} else {
			const std::string first_name = request.GetArg("first_name");
			const std::string last_name = request.GetArg("last_name");
			return getUsers(response, first_name, last_name);
		}
	} break;

	case server::http::HttpMethod::kPost:
		return registerUser(response, body);

	case server::http::HttpMethod::kPut: {
		const std::string login = request.GetPathArg(0);
		return updateUser(response, login, body);
	} break;

	case server::http::HttpMethod::kDelete: {
		const std::string login = request.GetPathArg(0);
		return deleteUser(response, login);
	} break;

	default: {
		throw server::handlers::ClientError();
	} break;
	}
}

formats::json::Value UserHandler::registerUser(server::http::HttpResponse &response,
                                               const formats::json::Value &body) const {
	User user = body.As<User>();

	if (user.login.empty() || user.password.empty() ||
	    user.first_name.empty() || user.last_name.empty()) {
		throw server::handlers::ClientError();
	}

	if (!storage_.addUser(user)) {
		throw server::handlers::ResourceNotFound();
	}

	response.SetStatus(http::kCreated);
	return formats::json::ValueBuilder("Successfuly registered new user").ExtractValue();
}

formats::json::Value UserHandler::getUsers(server::http::HttpResponse &,
                                           const std::string_view first_name,
                                           const std::string_view last_name) const {
	formats::json::ValueBuilder result;

	const auto users = storage_.getUsers(first_name, last_name);
	for (const auto &user : users)
		result.PushBack(user);

	return result.ExtractValue();
}

formats::json::Value UserHandler::getUser(server::http::HttpResponse &,
                                          const std::string &login) const {
	if (login.empty()) {
		throw server::handlers::ClientError();
	}

	const auto result = storage_.getUser(login);

	if (!result.has_value()) {
		throw server::handlers::ResourceNotFound();
	}

	return formats::json::ValueBuilder(result.value()).ExtractValue();
}

formats::json::Value UserHandler::updateUser(server::http::HttpResponse &,
                                             const std::string &login,
                                             const formats::json::Value &body) const {
	User user = body.As<User>();

	if (user.login.empty() || user.password.empty() ||
	    user.first_name.empty() || user.last_name.empty()) {
		throw server::handlers::ClientError();
	}

	if (!storage_.updateUser(login, user)) {
		throw server::handlers::ResourceNotFound();
	}

	return formats::json::ValueBuilder("Successfuly updated user information").ExtractValue();
}

formats::json::Value UserHandler::deleteUser(server::http::HttpResponse &,
                                             const std::string &login) const {
	if (login.empty()) {
		throw server::handlers::ClientError();
	}

	if (!storage_.deleteUser(login)) {
		throw server::handlers::ResourceNotFound();
	}

	return formats::json::ValueBuilder("User was successfuly removed").ExtractValue();
}

