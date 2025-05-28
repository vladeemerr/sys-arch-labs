from ghcr.io/userver-framework/ubuntu-22.04-userver:latest
arg host_uid
run useradd -u ${host_uid} -s /usr/sbin/nologin -m userver
user userver
copy --chown=userver user-service /home/userver
copy --chown=userver middlewares /home/userver/middlewares
copy --chown=userver jwt-cpp /home/userver/jwt-cpp
workdir /home/userver
run cmake --preset release && cmake --build build-release --parallel
entrypoint ./build-release/user-service -c config.yaml
