create schema if not exists vk_audience;

set search_path = vk_audience;

create table if not exists Users (
user_id bigint primary key,
first_name text not null,
last_name text not null,
can_access_closed boolean not null,
is_closed boolean not null,
deactivated text,
total_friends bigint
);

create table if not exists Groups (
group_id bigint primary key,
name text not null
);

create table if not exists Groups_x_Users (
group_id bigint not null,
user_id bigint not null
);

create table if not exists Influencers (
user_id bigint primary key,
first_name text not null,
last_name text not null
);

create table if not exists Influencers_x_Users (
influencer_id bigint not null,
user_id bigint not null
);

create table if not exists Posts (
post_id bigint,
owner_id bigint,
text text,
release_date timestamp,
primary key (post_id, owner_id)
);

create table if not exists Comments (
comment_id bigint,
owner_id bigint,
post_id bigint,
author_id bigint not null,
text text,
likes bigint,
primary key (comment_id, owner_id, post_id)
);

create table if not exists Friendships (
user_id bigint not null,
friend_id bigint not null,
valid_from_dtm timestamp not null,
valid_to_dtm timestamp not null
);
