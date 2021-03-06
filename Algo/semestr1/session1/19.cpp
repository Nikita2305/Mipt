#include <iostream>
#include <vector>

/*
Детерменированная сортировка:
Выбор медианы в качестве пивота.
Алгоритм be like: T(n) = 2*T(n/2) + P(n)
Всилу того, что уже доказано, что P(n) - поиск k-й статистики = O(n)
Тогда T(n) = 2*T(n/2) + c*n, а значит T(n) = O(nlogn)
*/
