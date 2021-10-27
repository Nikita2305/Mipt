.PHONY: all clean

config=`cat compile.conf | sed "/\#.*/d" | tr '\n' '\$$'`

all:
	gcc -g solution.c -o program.exe -DCONFIG="\"$(config)\""

clean:
	rm -rf program.exe
