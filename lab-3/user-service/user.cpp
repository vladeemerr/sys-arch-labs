#include "user.hpp"
#include "userver/logging/log.hpp"

#include <userver/crypto/hash.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>

UserStorage::UserStorage(const components::ComponentConfig &config,
                         const components::ComponentContext &context)
: components::ComponentBase(config, context),
  storage_(context.FindComponent<components::Postgres>("user-database").GetCluster()) {}

bool UserStorage::addUser(const User &info) {
	auto result = storage_->Execute(storages::postgres::ClusterHostType::kMaster,
		"insert into users (login, password, first_name, last_name) \
		 values ($1, $2, $3, $4) on conflict do nothing",
		info.login, User::encrypt(info.password), info.first_name, info.last_name);
	return result.RowsAffected() > 0;
}

std::optional<User> UserStorage::getUser(const std::string &login) const {
	auto result = storage_->Execute(storages::postgres::ClusterHostType::kMaster,
		"select login, password, first_name, last_name from users where login=$1",
		login);
	return result.AsOptionalSingleRow<User>(storages::postgres::kRowTag);
}

std::vector<User> UserStorage::getUsers(const std::string_view first_name,
                                        const std::string_view last_name) const {
	auto result = storage_->Execute(storages::postgres::ClusterHostType::kMaster,
		"select login, password, first_name, last_name from users \
		 where users.first_name = $1 and users.last_name=$2",
		first_name, last_name);
	return result.AsContainer<std::vector<User>>(storages::postgres::kRowTag);
}

bool UserStorage::updateUser(const std::string &login, const User &info) {
	auto result = storage_->Execute(storages::postgres::ClusterHostType::kMaster,
		"update users set (login, password, first_name, last_name) = ($2, $3, $4, $5) \
		 where login=$1",
		login, info.login, User::encrypt(info.password), info.first_name, info.last_name);
	return result.RowsAffected() > 0;
}

bool UserStorage::deleteUser(const std::string &login) {
	auto result = storage_->Execute(storages::postgres::ClusterHostType::kMaster,
		"delete from users where login=$1", login);
	return result.RowsAffected() > 0;
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
