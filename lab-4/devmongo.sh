#!/bin/sh
readonly image=mongo:5.0.31-focal
readonly opts="--name mongo-db --network dev -p 27017:27017 \
               -v ./mongo/init.js:/docker-entrypoint-initdb.d/00-init.js"

if ! docker network inspect dev >/dev/null 2>&1; then
	docker network create dev
fi

exec docker run --rm ${opts} $* ${image}
