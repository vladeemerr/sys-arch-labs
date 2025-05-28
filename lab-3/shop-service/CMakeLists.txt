cmake_minimum_required(VERSION 3.12)
project(shop-service LANGUAGES CXX)

find_package(userver COMPONENTS core REQUIRED)
userver_setup_environment()

add_executable(${PROJECT_NAME} middlewares/cors_middleware.cpp
                               middlewares/auth_middleware.cpp
                               product.cpp
                               shop_handler.cpp
                               shop_service.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE userver::core)
target_include_directories(${PROJECT_NAME} PRIVATE jwt-cpp/include)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
