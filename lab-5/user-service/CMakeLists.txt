cmake_minimum_required(VERSION 3.12)
project(user-service LANGUAGES CXX)

find_package(userver COMPONENTS core postgresql redis REQUIRED)
userver_setup_environment()

add_executable(${PROJECT_NAME} middlewares/cors_middleware.cpp
                               middlewares/auth_middleware.cpp
                               user.cpp
                               auth_handler.cpp
                               user_handler.cpp
                               user_service.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE userver::core userver::postgresql userver::redis)
target_include_directories(${PROJECT_NAME} PRIVATE jwt-cpp/include)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
