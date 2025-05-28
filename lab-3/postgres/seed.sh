#!/bin/sh
set -e

readonly flags="-U $POSTGRES_USER -d $POSTGRES_DB"

encrypt() {
	printf $1 | openssl sha512 | cut -f2 -d' '
}

add_user() {
	psql ${flags} -c "
		insert into users (login, password, first_name, last_name)
		values ('$1', '$(encrypt $2)', '$3', '$4')
	"
}

until pg_isready ${flags}; do sleep 1; done

add_user admin secret Trader Sidorovich
add_user tomcat patpatpat Kitty Enjoyer
add_user jorama password Joe Average
add_user sweetroll blu3m00n Alice Moon
