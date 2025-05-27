#include <userver/components/minimal_server_component_list.hpp>
#include <userver/utils/daemon_run.hpp>

#include "user.hpp"
#include "auth_handler.hpp"
#include "user_handler.hpp"
#include "middlewares/cors_middleware.hpp"
#include "middlewares/auth_middleware.hpp"

int main(int argc, char *argv[]) {
	auto list = userver::components::MinimalServerComponentList();
	list.Append<UserStorage>();
	list.Append<AuthHandler>();
	list.Append<UserHandler>();
	list.Append<CorsMiddlewareFactory>();
	list.Append<AuthMiddlewareFactory>();
	list.Append<AuthPipelineBuilder>("auth-pipeline-builder");
	return userver::utils::DaemonMain(argc, argv, list);
}
