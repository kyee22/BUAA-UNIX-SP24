
all: main

main: main.c
	gcc -o main main.c

run:
	./main

test: test.c
	gcc -o test test.c

testcase:
	