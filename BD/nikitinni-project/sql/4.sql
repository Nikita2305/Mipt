set search_path = vk_audience;

-- TAG: Groups

INSERT INTO Groups
VALUES (198081409,'ФПМИ 2 курс (набор 2020)');

INSERT INTO Groups
VALUES (207840928,'ФПМИ.Cтудентам');

-- TAG: Users

INSERT INTO Users
(user_id,first_name,last_name,can_access_closed,is_closed)
VALUES
(1000912,'Александр','Рубцов',True,False) 
ON CONFLICT (user_id) DO NOTHING;

INSERT INTO Users
(user_id,first_name,last_name,can_access_closed,is_closed)
VALUES
(1151988,'Виктор','Яковлев',True,False)
ON CONFLICT (user_id) DO NOTHING;

INSERT INTO Users
(user_id,first_name,last_name,can_access_closed,is_closed)
VALUES
(1373062,'Евгений','Шевченко',False,True)
ON CONFLICT (user_id) DO NOTHING;

INSERT INTO Users
(user_id,first_name,last_name,can_access_closed,is_closed)
VALUES
(1648578,'Александр','Орлов',False,True)
ON CONFLICT (user_id) DO NOTHING;

INSERT INTO Users
(user_id,first_name,last_name,can_access_closed,is_closed)
VALUES
(2153814,'Евгений','Молчанов',True,False)
ON CONFLICT (user_id) DO NOTHING;

INSERT INTO Users
(user_id,first_name,last_name,can_access_closed,is_closed)
VALUES
(2300155,'Алина','Кузнецова',True,False)
ON CONFLICT (user_id) DO NOTHING;

INSERT INTO Users
(user_id,first_name,last_name,can_access_closed,is_closed)
VALUES
(2483126,'Саша','Сенькова',True,False)
ON CONFLICT (user_id) DO NOTHING;

INSERT INTO Users
(user_id,first_name,last_name,can_access_closed,is_closed)
VALUES
(3162776,'Мария','Уткина',False,True)
ON CONFLICT (user_id) DO NOTHING;

INSERT INTO Users
(user_id,first_name,last_name,can_access_closed,is_closed)
VALUES
(3656763,'Иван','Эрлих',True,False)
ON CONFLICT (user_id) DO NOTHING;

INSERT INTO Users
(user_id,first_name,last_name,can_access_closed,is_closed)
VALUES
(4044482,'Наталья','Мастинен',False,True)
ON CONFLICT (user_id) DO NOTHING;

-- TAG: Groups_x_Users

INSERT INTO Groups_x_Users
VALUES (198081409, 1000912);

INSERT INTO Groups_x_Users
VALUES (198081409, 1151988);

INSERT INTO Groups_x_Users
VALUES (198081409, 1373062);

INSERT INTO Groups_x_Users
VALUES (198081409, 1648578);

INSERT INTO Groups_x_Users
VALUES (198081409, 2153814);

INSERT INTO Groups_x_Users
VALUES (198081409, 2300155);

INSERT INTO Groups_x_Users
VALUES (198081409, 2483126);

INSERT INTO Groups_x_Users
VALUES (198081409, 3162776);

INSERT INTO Groups_x_Users
VALUES (198081409, 3656763);

INSERT INTO Groups_x_Users
VALUES (198081409, 4044482);

INSERT INTO Groups_x_Users
VALUES (207840928, 2153814);

INSERT INTO Groups_x_Users
VALUES (207840928, 2300155);

INSERT INTO Groups_x_Users
VALUES (207840928, 3656763);

-- TAG: Friendships

INSERT INTO Friendships
VALUES (1000912, 2153814, current_timestamp, to_timestamp('2022-12-31 12:00:00', 'YYYY-MM-DD HH:MI:SS'));

INSERT INTO Friendships
VALUES (2153814, 1000912, current_timestamp, to_timestamp('2022-12-31 12:00:00', 'YYYY-MM-DD HH:MI:SS'));

INSERT INTO Friendships
VALUES (2153814, 3656763, current_timestamp, to_timestamp('2022-12-31 12:00:00', 'YYYY-MM-DD HH:MI:SS'));

INSERT INTO Friendships
VALUES (3656763, 2153814, current_timestamp, to_timestamp('2022-12-31 12:00:00', 'YYYY-MM-DD HH:MI:SS'));

-- TAG: Posts

INSERT INTO Posts
VALUES
(216, -198081409, 'Ребята, наши студенты традиционно показывали хорошие результаты на этой олимпиаде. Последние две олимпиады мы просто выигрывали (правда в старшей группе).', to_timestamp(1649759495));

INSERT INTO Posts
VALUES
(215, -198081409, 'Для студентов ФПМИ. \n\nМы продолжаем развивать взаимодействие ФПМИ со школами, как региональными, так и столичными. Уже два года действует проект [https:\/\/vk.com\/miptfpmi?w=wall-166330483_1656|“Наставник будущих физтехов”], в сентябре состоится первый набор на образовательную программу [https:\/\/vk.com\/miptfpmi?w=wall-166330483_3386|“Искусственный интеллект в математическом и IT-образовании”].', to_timestamp(1645013538));

INSERT INTO Posts
VALUES
(214, -198081409, 'МОДАЛЬНАЯ ЛОГИКА\n\nВесной-2022 можно прослушать, освоить и сдать факультативный курс «Модальная логика» (в рамках факультатива «Современные приложения дискретной математики и функционального анализа»). Курс читает ассистент кафедры МОУ Степанов Григорий.\nВся информация доступна на t.me\/kripke_frame\n\nЗанятия проводятся по пятницам с 18:35 до 20:00 в 428 гк.\nПервое занятие состоится 11 февраля.', to_timestamp(1643888203));

INSERT INTO Posts
VALUES
(215, -207840928, 'Идея для Маскота от [id441636937|Пластининой Валентины], Б05-924в — «Замечательная сова» 🦉\n\nСимволизирует ум и смекалку ФПМИшников — это раз! Отражает нашу тягу к знаниям — это два!\n\n#маскотФПМИ', to_timestamp(1652530359));

INSERT INTO Posts
VALUES
(214, -207840928, 'Кафедра концептуального анализа и проектирования ФПМИ объявляет набор студентов для поступления в бакалавриат\/магистратуру на совместную программу с Концерном «Алмаз-Антей»\n\nПараллельно с обучением на кафедре планируется устройство в лабораторию стратегического планирования и цифровой трансформации. Работы в лаборатории ведутся по направлениям, связанным с и стратегическим планированием и управлением, цифровой трансформации деятельности и кадровой политикой. Работы имеют прикладной характер, их результаты могут быть внедрены в Концерне и стать основой для дипломной работы.\n\n📅 Собеседования состоятся: 19 мая, 26 мая и 2 июня с 16.00 до 19.00 очно, в офисе базовой организации кафедры Центра «Концепт» или удаленно в Skype. Каждое из них займет около 15 минут.\n\n⚡Для регистрации на собеседование заполните форму: https:\/\/clck.ru\/h35PC\nВремя собеседования и ссылка на Skype (при выборе удаленного собеседования) будут отправлены на почту, указанную в форме. К собеседованию подготовьте собственные вопросы, а также ответ на вопрос: «Почему Вы хотите учиться на нашей кафедре?».\n\nБольше информации о кафедре можно найти здесь:\n• Страница кафедры на сайте МФТИ: https:\/\/vk.cc\/82NtJh\n• Запись презентации кафедры в 2021 году: https:\/\/clck.ru\/h35Pz\n\n❓ По всем вопросам можно обращаться к секретарю кафедры [id174564408|Константину Серкову]: на почту kserkov@acconcept.ru или Vk.\n\nБудем рады видеть Вас на собеседовании! Офис Центра «Концепт» расположен по адресу: м. Китай-город, ул. Солянка, д. 3, стр. 1, каб.46', to_timestamp(1652457938));

INSERT INTO Posts
VALUES
(211, -207840928, '11 мая в 19:00 подключайтесь к онлайн-презентации магистерской программы X5 Group — «Промышленный анализ данных в ритейле»\n\nМихаил Хозин, заместитель заведующего кафедрой расскажет:\n‣ Почему data science в ритейле это не про аналитику \"в стол\";\n‣ Что тебя ждет в процессе обучения (список курсов и практические кейсы);\n‣ Что нужно, чтобы пройти отбор;\n‣ Кто такой аналитик от бизнес-кейса до post-production и почему круто им быть.\n\n+ bonus от HR X5Tech: Мастер-класс «Как сделать сильное резюме и пройти собеседование молодому дата-саентисту».\n\n ⚡Подключиться: https:\/\/clck.ru\/guzYB\n\n🔗 Подробнее о кафедре:\nЗапись презентации прошлого года: https:\/\/clck.ru\/guzdp\nСайт кафедры: https:\/\/mipt.x5.ru\/\n\n📹 Ссылка на трансляцию:\nVk: https:\/\/clck.ru\/guzhd\nYouTube: https:\/\/clck.ru\/guzh5\n\n#презентации_кафедр_фпми #магистратураФПМИ', to_timestamp(1651933738));

-- TAG: Comments

INSERT INTO Comments
VALUES
(216, -207840928, 215, 156692231, 'Конечно же 2ой вариант. Лол. Gold on my wrist', 0);

INSERT INTO Comments
VALUES
(219, -207840928, 215, 328862944, 'Заявка на победу!', 0);

INSERT INTO Comments
VALUES
(220, -207840928, 215, 453008630, 'А почему сова желто-синяя?🤡', 1);

INSERT INTO Comments
VALUES
(221, -207840928, 215, 212943330, 'немного банально', 0);
