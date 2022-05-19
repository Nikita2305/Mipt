set search_path = vk_audience;

-- Фильтрация аккаунтов, основанная на свойствах аккаунта
DELETE FROM Users WHERE deactivated = 'banned';
DELETE FROM Users WHERE deactivated = 'deleted';
DELETE FROM Users WHERE can_access_closed = False;

-- Часть из загрузки данных
UPDATE Users SET total_friends = 1004 WHERE user_id = 1000912;
UPDATE Users SET total_friends = 552 WHERE user_id = 1151988;
UPDATE Users SET total_friends = 2274 WHERE user_id = 2153814;
UPDATE Users SET total_friends = 218 WHERE user_id = 2300155;
UPDATE Users SET total_friends = 499 WHERE user_id = 2483126;
UPDATE Users SET total_friends = 1851 WHERE user_id = 3656763;

-- Фильтрация аккаунтов, основанная на дружеских свойствах
DELETE FROM Users WHERE total_friends < 50;

DELETE FROM Friendships f USING (SELECT friend_id FROM Friendships a LEFT JOIN Users b ON a.friend_id = b.user_id WHERE b.user_id IS NULL) d WHERE f.friend_id = d.friend_id;

-- Опциональная часть фильтрации - удаляет недружелюбные аккаунты
/*
DELETE FROM Users p USING (SELECT a.user_id FROM Users a LEFT JOIN Friendships b ON a.user_id = b.user_id WHERE b.user_id IS NULL) d WHERE p.user_id = d.user_id;
*/

-- Некоторые обновления, как это и бывает в жизни
UPDATE Comments SET likes = Comments.likes + 1
WHERE Comments.comment_id = 216
AND Comments.owner_id = -207840928
AND Comments.post_id = 215;

UPDATE Comments SET likes = 7
WHERE Comments.comment_id = 219
AND Comments.owner_id = -207840928
AND Comments.post_id = 215;

-- Пример фильтрации обновлений
DELETE FROM Comments WHERE text SIMILAR TO '%[a-zA-Z]%';
SELECT FROM Comments WHERE text SIMILAR TO '[а-яА-Я]*';

-- Пример полной выкачки данных (подробнее в 6.sql)
-- SELECT array_to_json(array_agg(row_to_json(u))) FROM Users u;
