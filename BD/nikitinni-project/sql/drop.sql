set search_path = vk_audience;

drop trigger if exists fix_friendships ON Friendships;

drop function if exists invalidate_friendships(bigint);
drop function if exists sync_friendships();
drop function if exists sync_one_friendship();

drop view if exists UserIds;
drop view if exists GroupIds;
drop view if exists InfluencerIds;
drop view if exists UserStatistics;
drop view if exists CurrentFriendships;
drop view if exists InfluencerSubscribers;
drop view if exists GroupMembers;

drop table if exists Users cascade;
drop table if exists Groups cascade;
drop table if exists Groups_x_Users cascade;
drop table if exists Influencers cascade;
drop table if exists Influencers_x_Users cascade;
drop table if exists Posts cascade;
drop table if exists Comments cascade;
drop table if exists Friendships cascade;

drop schema if exists vk_audience;
