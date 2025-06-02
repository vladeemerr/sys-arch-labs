#!/bin/sh
readonly image=docker.swagger.io/swaggerapi/swagger-ui
readonly specfile=openapi-spec.yaml
readonly opts="-p 8000:8080 -v ./${specfile}:/${specfile} -e SWAGGER_JSON=/${specfile}"

if [ ! -f ${specfile} ]; then
	echo "Script must be executed from the root of a project"
	exit 1
fi

exec docker run --rm ${opts} $* ${image}
