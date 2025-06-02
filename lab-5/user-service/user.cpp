#include "user.hpp"

#include <userver/crypto/hash.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/redis/component.hpp>

UserStorage::UserStorage(const components::ComponentConfig &config,
                         const components::ComponentContext &context)
: components::ComponentBase(config, context),
  storage_(context.FindComponent<components::Postgres>("user-database").GetCluster()),
  cache_(context.FindComponent<components::Redis>("user-cache").GetClient("users")),
  redis_cc_(std::chrono::seconds{15}, std::chrono::seconds{60}, 4) {}

bool UserStorage::addUser(const User &info) {
	User user = {
		.login = info.login,
		.password = User::encrypt(info.password),
		.first_name = info.first_name,
		.last_name = info.last_name,
	};
	
	auto query = storage_->Execute(storages::postgres::ClusterHostType::kMaster,
		"insert into users (login, password, first_name, last_name) \
		 values ($1, $2, $3, $4) on conflict do nothing",
		user.login, user.password, user.first_name, user.last_name);

	if (query.RowsAffected() == 0) {
		return false;
	}

#ifndef BENCH_DISABLE_VALKEY
	const std::string key = "user:" + user.login;
	const auto value = formats::json::ValueBuilder(user).ExtractValue();
	(void)cache_->Set(key, formats::json::ToStableString(value), redis_cc_);
#endif // BENCH_DISABLE_VALKEY

	return true;
}

std::optional<User> UserStorage::getUser(const std::string &login) const {
	const std::string key = "user:" + login;

#ifndef BENCH_DISABLE_VALKEY
	if (auto reply = cache_->Get(key, redis_cc_).Get(); reply) {
		const User user = formats::json::FromString(reply.value()).As<User>();
		return std::make_optional(user);
	}
#endif // BENCH_DISABLE_VALKEY

	auto query = storage_->Execute(storages::postgres::ClusterHostType::kMaster,
		"select login, password, first_name, last_name from users where login=$1",
		login);

	auto result = query.AsOptionalSingleRow<User>(storages::postgres::kRowTag);
	if (!result.has_value()) {
		return std::nullopt;
	}

#ifndef BENCH_DISABLE_VALKEY
	const auto value = formats::json::ValueBuilder(result.value()).ExtractValue();
	(void)cache_->Set(key, formats::json::ToStableString(value), redis_cc_);
#endif // BENCH_DISABLE_VALKEY

	return result;
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
	User user = {
		.login = info.login,
		.password = User::encrypt(info.password),
		.first_name = info.first_name,
		.last_name = info.last_name,
	};
	
	if (login == info.login) {
		auto query = storage_->Execute(storages::postgres::ClusterHostType::kMaster,
			"update users set (login, password, first_name, last_name) = ($2, $3, $4, $5) \
			 where login=$1",
			login, user.login, user.password, user.first_name, user.last_name);

		if (query.RowsAffected() == 0) {
			return false;
		}
	} else {
		auto rem_query = storage_->Execute(storages::postgres::ClusterHostType::kMaster,
		                                   "delete from users where login=$1", login);

		auto add_query = storage_->Execute(storages::postgres::ClusterHostType::kMaster,
			"insert into users (login, password, first_name, last_name) \
			 values ($1, $2, $3, $4) on conflict do nothing",
			user.login, user.password, user.first_name, user.last_name);

		if (rem_query.RowsAffected() == 0 || add_query.RowsAffected() == 0) {
			return false;
		}

#ifndef BENCH_DISABLE_VALKEY
		const std::string key = "user:" + login;
		(void)cache_->Del(key, redis_cc_).Get();
#endif // BENCH_DISABLE_VALKEY
	}

#ifndef BENCH_DISABLE_VALKEY
	const std::string key = "user:" + user.login;
	const auto value = formats::json::ValueBuilder(user).ExtractValue();
	(void)cache_->Set(key, formats::json::ToStableString(value), redis_cc_);
#endif // BENCH_DISABLE_VALKEY

	return true;
}

bool UserStorage::deleteUser(const std::string &login) {
	const std::string key = "user:" + login;

#ifndef BENCH_DISABLE_VALKEY
	(void)cache_->Del(key, redis_cc_).Get();
#endif // BENCH_DISABLE_VALKEY

	auto query = storage_->Execute(storages::postgres::ClusterHostType::kMaster,
	                               "delete from users where login=$1", login);

	return query.RowsAffected() > 0;
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
