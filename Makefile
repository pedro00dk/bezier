
build:
	gcc -l m -l GL -l glut -o bezier bezier.c

build-gdb:
	gcc -l m -l GL -l glut -g -Wall -o bezier bezier.c

build-opt:
	gcc -l m -l GL -l glut -O3 -o bezier bezier.c
	
run: build-opt
	./bezier
