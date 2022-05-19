set search_path = vk_audience;

-- Скрывающие таблицы
CREATE VIEW UserIds AS
SELECT user_id, can_access_closed, is_closed, deactivated, total_friends 
FROM Users
WITH CASCADED CHECK OPTION;

CREATE VIEW GroupIds AS
SELECT group_id 
FROM Groups
WITH CASCADED CHECK OPTION;

CREATE VIEW InfluencerIds AS
SELECT user_id
FROM Influencers
WITH CASCADED CHECK OPTION;

-- Раскрывающие таблицы

CREATE VIEW UserStatistics AS
SELECT
a.user_id,
a.total_friends,
(CASE WHEN b.local_friends IS NULL THEN 0 ELSE b.local_friends END),
(CASE WHEN b.ulikes IS NULL THEN 0 ELSE b.ulikes END)
FROM
Users a
JOIN
(
    SELECT DISTINCT
    u.user_id,
    COUNT(f.friend_id) OVER (PARTITION BY u.user_id) AS local_friends,
    SUM(c.likes) OVER (PARTITION BY u.user_id) AS ulikes
    FROM
    Users u
    LEFT JOIN
    Friendships f
    ON
    u.user_id = f.user_id
    LEFT JOIN
    Comments c
    ON u.user_id = c.author_id
) AS b
ON a.user_id = b.user_id;


CREATE VIEW CurrentFriendships AS
SELECT user_id, friend_id
FROM Friendships
WHERE current_timestamp >= valid_from_dtm
AND current_timestamp <= valid_to_dtm
WITH CASCADED CHECK OPTION;

CREATE VIEW InfluencerSubscribers AS
SELECT b.user_id, b.first_name, b.last_name, a.first_name inf_first_name, a.last_name inf_last_name
FROM
Influencers_x_Users ab
JOIN
Influencers a
ON
ab.influencer_id = a.user_id
JOIN
Users b
ON
ab.user_id = b.user_id;

CREATE VIEW GroupMembers AS
SELECT b.user_id, b.first_name, b.last_name, a.name
FROM
Groups_x_Users ab
JOIN
Groups a
ON
ab.group_id = a.group_id
JOIN
Users b
ON
ab.user_id = b.user_id;

