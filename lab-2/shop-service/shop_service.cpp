#include <userver/components/minimal_server_component_list.hpp>
#include <userver/utils/daemon_run.hpp>

#include "product.hpp"
#include "shop_handler.hpp"
#include "middlewares/cors_middleware.hpp"
#include "middlewares/auth_middleware.hpp"

int main(int argc, char *argv[]) {
	auto list = userver::components::MinimalServerComponentList();
	list.Append<ProductStorage>();
	list.Append<ShopHandler>();
	list.Append<CorsMiddlewareFactory>();
	list.Append<AuthMiddlewareFactory>();
	list.Append<AuthPipelineBuilder>("auth-pipeline-builder");
	return userver::utils::DaemonMain(argc, argv, list);
}
