#!/bin/sh
readonly url="http://localhost:8080"

readonly token="$(curl -s -X 'POST' \
  '${url}/auth' \
  -H 'accept: application/json' \
  -H 'Content-Type: application/json' \
  -d '{
  "login": "admin",
  "password": "secret"
}' | jq -r '.token')"

bench() {
	wrk -t${1:-1} -c10 -d15s -H "Authorization: Bearer ${token}" ${url}/user/$2
}

echo -e "\n*** Bench existing user (1 thread)"
bench 1 tomcat

echo -e "\n*** Bench non-existing user (1 thread)"
bench 1 dude

echo -e "\n*** Bench existing user (5 threads)"
bench 5 tomcat

echo -e "\n*** Bench non-existing user (5 threads)"
bench 5 dude

echo -e "\n*** Bench existing user (10 threads)"
bench 10 tomcat

echo -e "\n*** Bench non-existing user (10 threads)"
bench 10 dude
