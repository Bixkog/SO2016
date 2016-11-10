CC=gcc
CFLAGS=-Wall -Wextra -std=gnu99 -g

all: strcnt.o strdrop.o libstr.so ldl rdl

strcnt.o: CFLAGS+=-fPIC

strdrop.o: CFLAGS+=-fPIC

libstr.so: CFLAGS+= -fPIC -shared
libstr.so: strcnt.o strdrop.o
	$(CC) $(CFLAGS) -o $@ $^

ldl: 
	$(CC) $(CFLAGS) ldl.c -o $@ -Wl,-rpath,. libstr.so

rdl:
	$(CC) $(CFLAGS) rdl.c -o $@ -Wl,-rpath,. -ldl 

clean:
	$(RM) *.o *.out *.so ldl rdl 
