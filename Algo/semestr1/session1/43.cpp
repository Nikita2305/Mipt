AVL - дерево
Определение - бинарное дерево, у которого |h(L) - h(R)| <= 1.

Пусть S(h) - мин. кол-во вершин в AVL дереве глубины h, тогда S(h) = F(h+1) - 1
S(h) = 1 + S(h-1) + S(h-2)

База:
S(1) = 1 = 2 - 1 = F(2) - 1
S(2) = 2 = 3 - 1 = F(3) - 1
Переход:
S(h) = 1 + S(h-1) + S(h-2) = 1 + F(h) - 1 + F(h-1) - 1 = F(h+1) - 1, ч.т.д.

А значит глубина - O(logn)

----
Научимся исправлять дизбаланс:
h(a.l) - h(a.r) = -2
Рассматривая 2 случая на a.l.l:
a.r.l = h-2 -> левый поворот
a.r.l = h-3 -> a.r.r = h-2. -> a.r.правый поворот -> левый поворот
----

Операции:
insert x - пройдём вниз до нек листа и вставим его туда. Очев. высота каждого поддерева на пути изменилась не более чем на единицу, так что win-win, это мы умеем чинить, будем это делать снизу вверх.

erase x - пройдём вниз до этого числа. Если нет правого поддерева - победили и вернули левое.
Если есть правое поддерево, нашли там минимум и удалили. На обратном пути починили высоты.
Вставили минимум вместо нашего х. Починили высоты на обратном пути.
