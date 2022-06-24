## Users / Пользователи
| Название        | Описание             | Тип данных  |  Ограничение |
| --------------- |:--------------------:|:-----------:| ------------ |
| user_id       | id человека          | bigint      | pk, not null |
| first_name      | Имя человека         | text        | not null     |
| last_name     | Фамилия человека     | text      | not null     |
| can_access_closed     | Доступность страницы   | boolean      | not null |
| is_closed     | Закрытость аккаунта     | boolean      | not null     |
| deactivated     | Статус активности     | text      |      |
| total_friends     | Количество друзей     | bigint      | not null     |

---

## Groups / Группы
| Название        | Описание             | Тип данных  |  Ограничение |
| --------------- |:--------------------:|:-----------:| ------------ |
| group_id       | id группы          | bigint      | pk, not null |
| name      | Название         | text        | not null     |

---

## Groups_x_Users / Подписчики групп
| Название        | Описание             | Тип данных  |  Ограничение |
| --------------- |:--------------------:|:-----------:| ------------ |
| group_id       | id группы          | bigint      | not null |
| user_id      | id участника группы         | bigint        | not null     |

---

## Influencers / Инфлюенсеры
| Название        | Описание             | Тип данных  |  Ограничение |
| --------------- |:--------------------:|:-----------:| ------------ |
| user_id       | id человека          | bigint      | pk, not null |
| first_name      | Имя человека         | text        | not null     |
| last_name     | Фамилия человека     | text      | not null     |

---

## Influencers_x_Users / Подписчики инфлюенсеров
| Название        | Описание             | Тип данных  |  Ограничение |
| --------------- |:--------------------:|:-----------:| ------------ |
| influencer_id       | id инфлюенсера     | bigint      | not null |
| user_id      | id подписчика         | bigint        | not null     |

---

## Posts / Посты
| Название        | Описание             | Тип данных  |  Ограничение |
| --------------- |:--------------------:|:-----------:| ------------ |
| post_id       | id человека          | bigint      | pk, not null |
| owner_id      | id создателя поста     | bigint        | pk, not null     |
| text     | Текст публикации     | text      |      |
| date     | Дата публикации     | timestamp      |      |

---

## Comments / Комментарии
| Название        | Описание             | Тип данных  |  Ограничение |
| --------------- |:--------------------:|:-----------:| ------------ |
| сomment_id       | id комментария          | bigint      | pk, not null |
| owner_id     | id владельца поста | bigint | pk, not null |
| post_id      | id поста     | bigint        | pk, not null     |
| author_id      | id автора     | bigint        |  not null     |
| text     | Текст комментария     | text      |      |
| likes     | Количество лайков     | bigint      |      |

---

## Friendships / Дружеские отношения
| Название        | Описание             | Тип данных  |  Ограничение |
| --------------- |:--------------------:|:-----------:| ------------ |
| user_id       | id пользователя     | bigint      | not null |
| friend_id      | id друга         | bigint        | not null |
| valid_from_dtm | Начало интервала валидности | timestamp | not null |
| valid_to_dtm | Конец интервала валидности | timestamp | not null |

