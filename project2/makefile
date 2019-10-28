all: Sim02

Sim02: Sim02.o Meta.o
	g++ -g -pthread -o Sim02 Sim02.o Meta.o

clean:
	rm Sim02 Sim02.o Meta.o *.lgf

Meta.o: Meta.cpp Meta.h
