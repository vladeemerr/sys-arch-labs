#!/bin/sh
readonly image=mongo:5.0.31-focal
readonly opts="--name mongo-db --network dev -p 27017:27017 \
               -v ./mongo/init.js:/docker-entrypoint-initdb.d/00-init.js"

if [ ! -d mongo ]; then
	echo "Script must be executed from the root of a project"
	exit 1
fi

if ! docker network inspect dev >/dev/null 2>&1; then
	docker network create dev
fi

exec docker run --rm ${opts} $* ${image}
