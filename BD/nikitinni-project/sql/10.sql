set search_path = vk_audience;

CREATE FUNCTION invalidate_friendships(id bigint)
RETURNS void
AS $$
DECLARE
BEGIN
    UPDATE Friendships SET valid_to_dtm = current_timestamp WHERE user_id = id;
    UPDATE Friendships SET valid_to_dtm = current_timestamp WHERE friend_id = id;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION sync_friendships()
RETURNS void
AS $$
DECLARE
   rec   record;
BEGIN
    FOR rec IN SELECT * FROM CurrentFriendships
    LOOP
        IF (rec.friend_id, rec.user_id) not in (SELECT * FROM CurrentFriendships) THEN
            INSERT INTO Friendships
            VALUES (rec.friend_id, rec.user_id, current_timestamp, to_timestamp('2022-12-31 12:00:00', 'YYYY-MM-DD HH:MI:SS'));
        END IF;
   END LOOP;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION sync_one_friendship()
RETURNS trigger
AS $$
DECLARE
BEGIN
    IF (NEW.friend_id, NEW.user_id) not in (SELECT * FROM CurrentFriendships) THEN
        INSERT INTO Friendships
        VALUES (NEW.friend_id, NEW.user_id, current_timestamp, to_timestamp('2022-12-31 12:00:00', 'YYYY-MM-DD HH:MI:SS'));
    END IF;
END
$$ LANGUAGE plpgsql;
