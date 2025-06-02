#pragma once

#include <optional>
#include <vector>

#include <userver/components/component_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/formats/json.hpp>
#include <userver/storages/redis/client.hpp>

using namespace userver;

struct User {
	std::string login;
	std::string password;
	std::string first_name, last_name;

	static std::string encrypt(const std::string_view password);
};

class UserStorage final : public components::ComponentBase {
public:
	static constexpr std::string_view kName = "user-storage";

	UserStorage(const components::ComponentConfig &,
	            const components::ComponentContext &);

	[[nodiscard]] bool addUser(const User &info);
	[[nodiscard]] std::optional<User> getUser(const std::string &login) const;
	[[nodiscard]] std::vector<User> getUsers(const std::string_view first_name,
	                                         const std::string_view last_name) const;
	[[nodiscard]] bool updateUser(const std::string &login, const User &info);
	[[nodiscard]] bool deleteUser(const std::string &login);

private:
	storages::postgres::ClusterPtr storage_;
	storages::redis::ClientPtr cache_;
	storages::redis::CommandControl redis_cc_;
};

User Parse(const formats::json::Value &, formats::parse::To<User>);
formats::json::Value Serialize(const User &, formats::serialize::To<formats::json::Value>);
