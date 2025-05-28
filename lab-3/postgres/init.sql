create table if not exists users(
	id serial primary key,
	login text not null unique,
	password text not null,
	first_name text not null,
	last_name text not null
);

create unique index user_login_index on users(login);
create index user_name_index on users(first_name, last_name);
