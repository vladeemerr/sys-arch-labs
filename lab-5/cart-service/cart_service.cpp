#include <userver/components/minimal_server_component_list.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/utils/daemon_run.hpp>

#include "cart.hpp"
#include "cart_handler.hpp"
#include "middlewares/cors_middleware.hpp"
#include "middlewares/auth_middleware.hpp"

int main(int argc, char *argv[]) {
	auto list = userver::components::MinimalServerComponentList();
	list.Append<userver::components::HttpClient>();
	list.Append<userver::clients::dns::Component>();
	list.Append<CartStorage>();
	list.Append<CartHandler>();
	list.Append<CorsMiddlewareFactory>();
	list.Append<AuthMiddlewareFactory>();
	list.Append<AuthPipelineBuilder>("auth-pipeline-builder");
	return userver::utils::DaemonMain(argc, argv, list);
}
