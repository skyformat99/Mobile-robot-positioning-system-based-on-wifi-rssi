
CC = arm-linux-gcc

rt73 : iwlist.o iwlib.o
	$(CC) -lm -o rt73 iwlist.o iwlib.o
iwlist.o : iwlist.c wireless.h iwlib.h
	$(CC) -c iwlist.c
iwlib.o : iwlib.c iwlib.h wireless.h
	$(CC) -c iwlib.c
clean :
	rm -f *.o






