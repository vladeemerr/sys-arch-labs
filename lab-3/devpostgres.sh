#!/bin/sh
readonly image=postgres:14
readonly opts="--name postgres-db --network dev -p 5432:5432 \
               -e POSTGRES_USER=admin \
               -e POSTGRES_PASSWORD=assword \
               -e POSTGRES_DB=user-db \
               -v ./postgres/init.sql:/docker-entrypoint-initdb.d/00-init.sql \
               -v ./postgres/seed.sh:/docker-entrypoint-initdb.d/10-seed.sh"

if ! docker network inspect dev >/dev/null 2>&1; then
	docker network create dev
fi

exec docker run --rm ${opts} $* ${image}
