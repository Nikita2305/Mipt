set search_path = vk_audience;

CREATE TRIGGER fix_friendships
AFTER INSERT ON Friendships
REFERENCING NEW TABLE AS NEW
FOR EACH STATEMENT
EXECUTE PROCEDURE sync_one_friendship();
