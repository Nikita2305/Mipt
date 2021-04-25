Построение кучи за O(n). Пусть дан массив a[1..n]
Представим, что это и есть наш массив кучи. Тогда полагаем, что на последнем уровне(листья) куча построена верно.
Дальше рассматриваем предпоследний уровень. Если элемент маленький, то всё хорошо, но если большой,
то нужен сифт даун. Совершим, если нужно.
И так далее по индукции строим верное дерево.
На самом деле можно просто запустить сифт Даун от всех вершин дерева по убыванию(потому что от листов это = ничего)
Оценим асимпотитку:
n/2 на нижнем уровне. Там потребуется 1 операция
n/4 на втором уровне. Там потребуется 2 операции
n/8 на третьем уровне Там потребуется 3 операции
= n/2 * 1 + ... + n * k/2^k = n * (1/2 + ... + k/2^k).
Пусть S = 1/2 + ... + k/2^k
k/2^k = (k-1)/2^k + 1/2^k = (k-1)/2^(k-1) * 1/2 + 1/2^k
Тогда S = (1/2 + 1/4 + ... + 1/2^k) + 1/2 * (1/2 + ... + (k-1)/2^(k-1)) <= 1 + 1/2*S
S <= 2
Тогда операций <=2n
