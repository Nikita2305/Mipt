set search_path = vk_audience;

-- Поможет в WHERE по id и в JOIN по id
CREATE INDEX ON Users(user_id);
CREATE INDEX ON Groups(group_id);
CREATE INDEX ON Influencers(user_id);
CREATE INDEX ON Posts(owner_id, post_id);
CREATE INDEX ON Comments(owner_id, post_id, comment_id);
