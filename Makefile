
all: main

main: main.c
	gcc -o main main.c

run:
	./main

test:
	@echo "Copying files from test directory to the current directory..."
	cp -r case/* .



