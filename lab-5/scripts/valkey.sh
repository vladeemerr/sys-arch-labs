#!/bin/sh
readonly image=valkey/valkey:8.1.1-alpine
readonly opts="--name valkey-cache --network dev -p 6379:6379"

if ! docker network inspect dev >/dev/null 2>&1; then
	docker network create dev
fi

exec docker run --rm ${opts} $* ${image}
