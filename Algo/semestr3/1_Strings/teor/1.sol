Теорки 1. Базовые строковые алгоритмы.

Задача 1.
Дана префикс функция - необх найти лексикограф. минимальную строку с такой префикс. функцией.
Чтоже, будем действовать жадно: В каждый момент будем ставить минимально возможную букву из алфавита.
Очевидно такой подход выдаёт минимальную лексикографически строку,
    ведь там идёт такое же "жадное" поразрядное сравнение
Касательно "минимально возможной буквы", в частности [ будем считать s - строка, p - преф. функц. в 0-индексации]:
    1) s[0] положим 'а'.
    2) Далее если p[1] = 1, то это значит, что s[1] = s[0], откуда единственная возможная буква = 'a'.
        если p[1] = 0, то положим s[1] = 'b'.
    3) Алгоритмически эти рассуждения можно продлить(невзирая на ужасную асимптотику) так:
        for (int i = 0; i < s.length(); i++) {
            for (char x = 'a'; x <= 'z'; x++) {
                s[i] = x;
                if (same(prefix_function(s), p, i + 1))
                    break;
            }
        }
        Где same(vector, vector, int k) возвращает true <=> векторы совпадают на первых k элементах

Задача 2.
Стоит взглянуть на код в конце файла и удостовериться, что он решает данную задачу за данную асимптотику
(Задача Е первого контеста)
В чём собственно состоит идея асимптотики:
1) Вершин в боре O(sum(|s_i|))
2) Для каждой вершины по разу будет ВЫЧИСЛЕНЫ:
    to[1], ...to[|Alphabet|],
    link
    forwardLink
Здесь получаем асимптотику из Бора.
3) Далее начинаем саму обработку текста в котором |t| букв.
Для каждой буквы мы путешествуем по forwardLink-ам
Суммарно путешествий не больше ans, т.к. каждый прыжок добавляет хотя бы 1 к ответу
А значит из обработки текста получаем асимптотику O(|t| + ans)
4) Суммарно то что нужно.

Задача 3.
Сразу заметим, что крайним случаем будет aaaaa // при n = 5: a,aa,aaa,aaaa,aaaaa
abcde // при n = 5: a,b,c,d,e
abacaba // при n = 7: a,b,c,aba,aca,bacab,abacaba
ababa // при n = 5: a,b,aba,bab,ababa

Маленькие случаи:
а: L = 1, P = {a}
aa: L = 2, P = {a, aa}
ab: L = 2, P = {a, b}
aab: L = 3, P = {a, aa, b}
aaa: L = 3, P = {a, aa, aaa}
abc: L = 3, P = {a, b, c}
abb: L = 3, P = {a, b, bb}
aba: L = 3, P = {a, b, aba}

Утверждение доказывается по индукции.
для n = 1 - доказано выше.
Пусть имеем строку s, |s| = n
Утверждение для префикса длиной n-1 строки s уже доказано, а значит там не более n-1 подпалиндрома.
Добавляя букву в конце могут возникнуть подпалиндромы, в которых участвует последняя буква.
Если возник всего один, то утв. доказано (более того могло быть так, что этот палиндром уже был учтён в тех n-1)
Если возникло несколько, то возьмём самый длиный - X и докажем, что все другие уже были учтены.
Ну в целом очевидно, ведь любой другой возникнувший есть собственный суффикс X, а значит у него есть копия
и она полностью лежит в строке без последней буквы, а значит был учтён ранее.
Ч.т.д. количество увеличилось не более чем на 1.

Задача 4.
Предподсчёт.
Возьмём и посчитаем префикс функции для всех строк.
На этом этапе имеем O(summ{|s_i|})

Затем, как мы помним в алгоритме префикс функции при каждом i происходит итерация по всем суперефиксам.
При i = n - 1 просто запомним все супрефиксы, который эта часть алгоритма нам выдаст.
Теперь у нас для каждого слова есть набор индексов, которые соотв. супрефиксам, которые вообще сущесвтуют.
На этом этапе также не вышли за O(summ{|s_i|}), т.к. для каждой строки таких индексов O(|s_i|)

Запихнём все слова в бор и пометим терминальными вершины(вообще term сделаем int, т.е. счётчик),
    которые в соотв. словах соотвтетсвуют одному из найденных индексов.
Тут также не вышли за O(summ{|s_i|}).

Ну а теперь когда приходит запрос - просто в боре найдём соотв. вершину и выдадим её term-счётчик.

Какая-то сайд-мысль: 
    Решение опирается на мысль, что всего супрефиксов немного (а вот q может нестрого говоря -> inf) и
    основную часть вычислений стоит сложить на их полное вычисление, 


-----------------------------------
Приложение к задаче 2.
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

struct Node {
    std::vector<Node*> to;
    Node* link = nullptr;
    Node* forwardLink = nullptr;
    Node* parent = nullptr;
    int lastChar = -1;
    std::vector<int> term;
    Node* operator[](int i) {
        return to[i];
    } 
};

class Bor {
public:
    Bor(int nodes) { 
        arr = new Node[nodes];
        root = getNew();
    }
    void add(const std::string& s, int index) {
        Node* node = root;
        for (int i = 0; i < s.length(); i++) {
            int j = s[i] - 'a';
            if (!(*node)[j]) {
                node->to[j] = getNew();
                (*node)[j]->lastChar = j;
                (*node)[j]->parent = node;
            }
            node = (*node)[j]; 
        }
        node->term.push_back(index);
    }
    Node* getLink(Node* node) {
        if (!node->link) {
            if (node == root or node->parent == root) {
                node->link = root;
            } else {
                node->link = getTo(getLink(node->parent), node->lastChar);
            }
        }
        return node->link;
    }
    Node* getTo(Node* node, int c) {
        if (!node->to[c]) {
            if (node == root) {
                node->to[c] = root;
            } else {
                node->to[c] = getTo(getLink(node), c);
            }
        }
        return node->to[c];
    }
    Node* getForwardLink(Node* node) {
        if (!node->forwardLink) {
            if (getLink(node)->term.size() > 0) {
                node->forwardLink = getLink(node);
            } else if (getLink(node) == root) {
                node->forwardLink = root;
            } else {
                node->forwardLink = getForwardLink(getLink(node));
            }
        }
        return node->forwardLink; 
    } 
    void countMatches(const std::string& text, std::vector<std::vector<int>>& indexes) {
        Node* current = root;
        for (int i = 0; i < text.size(); i++) {
            int c = text[i] - 'a';
            current = getTo(current, c);
            Node* now = current;
            while (now != root) {
                for (auto& t: now->term) {
                    indexes[t].push_back(i);
                }
                now = getForwardLink(now);
            }
        } 
    }
    ~Bor() {
        delete[] arr;
    } 
private:
    Node* root;
    Node* arr;
    int ptr = 0;
    Node* getNew() {
        ptr++;
        arr[ptr - 1].to.resize(26, nullptr);
        return &arr[ptr - 1];
    }
};

main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::string text;
    std::cin >> text;
    int n = 0;
    std::cin >> n;
    Bor b(1000000 + 10);
    std::vector<int> len(n);
    for (int i = 0; i < n; i++) {
        std::string word;
        std::cin >> word;
        b.add(word, i);
        len[i] = word.length();
    }
    std::vector<std::vector<int>> indexes(n, std::vector<int>());
    b.countMatches(text, indexes);
    for (int i = 0; i < n; i++) {
        std::cout << indexes[i].size() << " ";
        for (auto& t : indexes[i]) { 
            std::cout << t - len[i] + 2 << " ";
        }
        std::cout << std::endl;
    }    
}
