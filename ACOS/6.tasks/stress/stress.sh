if test -f solve.sh
then
  echo "Найдено решение solve.sh"
else
  echo "Не найдено решение solve.sh"
  exit
fi

if test -d tmp
then
  echo -n "Перекомпилировать чекер и генератор?(y/n): "
  read line
  if [[ "$line" = "y" ]]
  then
    echo "Удаление tmp"
    rm -r tmp
  fi
fi

if ! test -d tmp
then
  echo "Создание tmp"
  mkdir tmp
  echo "Компиляция checker"
  g++ checker.cpp -o tmp/checker
  echo "Компиляция generator"
  g++ generator.cpp -o tmp/generator
fi

cp solve.sh tmp/
cd tmp

if test -d tests
then
  echo -n "Пересоздать тесты?(y/n): "
  read line
  if [[ "$line" = "y" ]]
  then
    rm -r tests
    echo "Удаление tests"
  fi
fi

tests=100
if ! test -d tests
then
  mkdir tests

  for ((i = 0; i < $tests; i++))
  do
    ./generator 2 tests/test$i tests/res$i
  done

  # for ((i = 10; i < 20; i++))
  # do
  #   ./generator 2 tests/test$i tests/res$i
  # done

  # for ((i = 20; i < 30; i++))
  # do
  #   ./generator 2 tests/test$i tests/res$i
  # done

  echo "Тесты были созданы"
fi

if test -d output
then
  rm -r output
fi
mkdir output

for ((i = 0; i < $tests; i++))
do
  echo "Решение $i-го СЛУ"
  ./solve.sh tests/test$i > output/out$i
done

for ((i = 0; i < $tests; i++))
do
  echo "$i-ый тест: "`./checker tests/res$i output/out$i`
done
