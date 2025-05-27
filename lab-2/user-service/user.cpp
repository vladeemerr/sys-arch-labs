#include "user.hpp"

#include <userver/crypto/hash.hpp>

UserStorage::UserStorage(const components::ComponentConfig &config,
                         const components::ComponentContext &context)
: components::ComponentBase(config, context) {
	(void)addUser(User{
		.login = "admin",
		.password = "secret",
		.first_name = "Trader",
		.last_name = "Sidorovich",
	});
}

bool UserStorage::addUser(const User &info) {
	auto result = storage_.Emplace(info.login, User{
		.login = info.login,
		.password = User::encrypt(info.password),
		.first_name = info.first_name,
		.last_name = info.last_name,
	});

	return result.inserted;
}

std::optional<User> UserStorage::getUser(const std::string &login) const {
	const auto result = storage_.Get(login);

	if (result == nullptr)
		return std::nullopt;

	return std::make_optional(*result);
}

std::vector<User> UserStorage::getUsers(const std::string_view first_name,
                                        const std::string_view last_name) const {
	std::vector<User> result;

	bool has_first_name = !first_name.empty();
	bool has_last_name = !last_name.empty();

	if (!has_first_name && !has_last_name) {
		for (const auto &[k, v] : storage_)
			result.push_back(*v);
	} else if (has_first_name && has_last_name) {
		for (const auto &[k, v] : storage_) {
			if (v->first_name == first_name && v->last_name == last_name)
				result.push_back(*v);
		}
	} else {
		for (const auto &[k, v] : storage_) {
			if ((!first_name.empty() && v->first_name == first_name) ||
			    (!last_name.empty() && v->last_name == last_name)) {
				result.push_back(*v);
			}
		}
	}

	return result;
}

bool UserStorage::updateUser(const std::string &login, const User &info) {
	return deleteUser(login) && addUser(info);
}

bool UserStorage::deleteUser(const std::string &login) {
	return storage_.Erase(login);
}

User Parse(const formats::json::Value &value, formats::parse::To<User>) {
	return User{
		.login = value["login"].As<std::string>(),
		.password = value["password"].As<std::string>(),
		.first_name = value["first_name"].As<std::string>(),
		.last_name = value["last_name"].As<std::string>(),
	};
}

formats::json::Value Serialize(const User &user,
                               formats::serialize::To<formats::json::Value>) {
	formats::json::ValueBuilder result;
	result["login"] = user.login;
	result["password"] = user.password;
	result["first_name"] = user.first_name;
	result["last_name"] = user.last_name;
	return result.ExtractValue();
}

std::string User::encrypt(const std::string_view password) {
	return userver::crypto::hash::Sha512(password);
}
