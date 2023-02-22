Примечание:
1) Кто спишет фулл, тот лох.(реально не списывайте плз)
2) Запускать скрипты в папке, где main.cpp

Код тестирования:
export CLASSPATH="`pwd`/test"
g++ -std=c++17 -Wl,--export-dynamic main.cpp -o file -ldl
./file

Ожидается вывод:
hello I'm dog from test test2
hello I'm dog from test test2
inter
hello I'm cat
hello I'm cat
inter
