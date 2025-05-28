#!/bin/sh
readonly image=docker.swagger.io/swaggerapi/swagger-ui
readonly specfile=openapi-spec.yaml
readonly opts="-p 8000:8080 -v ./${specfile}:/${specfile} -e SWAGGER_JSON=/${specfile}"
exec docker run --rm ${opts} $* ${image}
