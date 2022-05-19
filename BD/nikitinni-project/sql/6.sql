set search_path = vk_audience;

-- Получение друзей некоторого пользователя 
SELECT friend_id FROM Friendships WHERE user_id = 3656763;

-- Группы некоторого пользователя
SELECT group_id FROM Groups_x_Users WHERE user_id = 3656763;

-- Упорядочить пользователей по вовлечённости в сообщество 
SELECT
a.user_id,
a.total_friends,
(CASE WHEN b.local_friends IS NULL THEN 0 ELSE b.local_friends END),
(CASE WHEN b.ulikes IS NULL THEN 0 ELSE b.ulikes END),
(   (CASE WHEN b.local_friends IS NULL THEN 0 ELSE b.local_friends END) * 500 +
    (CASE WHEN b.ulikes IS NULL THEN 0 ELSE b.ulikes END)* 0.01 +
    total_friends
) score
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
ON a.user_id = b.user_id
ORDER BY score;

-- Комментарии, которые находятся у данного обладателя (группа/инфлюенсер)
SELECT comment_id, post_id FROM Comments WHERE owner_id = -207840928;

-- 3 самых популярных комментария к некоторому посту
SELECT comment_id, text
FROM Comments
WHERE post_id = 215 AND owner_id = -207840928
ORDER BY likes
LIMIT 3;

-- Найти всех пользователей из Users, написавших более одного комментария
SELECT user_id FROM
Users u JOIN Comments c ON u.user_id = c.author_id
GROUP BY u.user_id
HAVING COUNT(c.comment_id) > 1;

-- Найти квантили аккаунтов по количеству друзей относительно всей базы
SELECT user_id, CAST((ROW_NUMBER() OVER (ORDER BY total_friends)) AS FLOAT) / (SELECT COUNT(*) FROM Users) AS quantile
FROM Users;

-- Найти друзей данного пользователя в некоторый момент, например сейчас
SELECT friend_id FROM Friendships
WHERE current_timestamp >= valid_from_dtm
AND current_timestamp <= valid_to_dtm 
AND user_id = 3656763;

-- Проверка пересечения в версионности 
SELECT
user_id, friend_id,
prev_valid_from_dtm, prev_valid_to_dtm,
valid_from_dtm, valid_to_dtm
FROM (
    SELECT
    user_id,
    friend_id,
    LAG(valid_from_dtm) OVER (PARTITION BY user_id, friend_id
        ORDER BY valid_from_dtm) AS prev_valid_from_dtm,
    LAG(valid_to_dtm) OVER (PARTITION BY user_id, friend_id
        ORDER BY valid_from_dtm) AS prev_valid_to_dtm,
    valid_from_dtm,
    valid_to_dtm
    FROM Friendships
) t
WHERE valid_from_dtm <= prev_valid_to_dtm;

-- Получение данных из некоторой таблицы в формате json
-- SELECT array_to_json(array_agg(row_to_json(u))) FROM Users u;
