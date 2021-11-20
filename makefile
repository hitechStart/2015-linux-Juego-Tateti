all: tateti servidor

tateti: tateti.o extras.o grafica.o
	gcc -o tateti tateti.o extras.o grafica.o -lpthread -lSDL

servidor: servidor.o
	gcc -o servidor servidor.o -lpthread

tateti.o: tateti.c extras.h grafica.h
	gcc -c tateti.c

servidor.o: servidor.c 
	gcc -c servidor.c

extras.o: extras.c 
	gcc -c extras.c

grafica.o: grafica.c 
	gcc -c grafica.c
