/*
Итак. Заметим, что по факту алгоритм можно представить в виде дерева - блоксхемы.
Каждый узел - одно сравнение. Докажем, что высота алгоритма сортировки >= C * n * log(n)
Заметим количество листьев - не менее n!
Пусть h - высота дерева, тогда n! <= List <= 2^h
h > log(n!) >= 1/2 * n * log(n) //ч.т.д.
*/
