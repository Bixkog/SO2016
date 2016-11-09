all: modules library ldl rdl

modules:
	gcc strdrop.c -g -c -o strdrop.o -fPIC -Wall -Wextra
	gcc strcnt.c -g -c -o strcnt.o -fPIC -Wall -Wextra
library:
	gcc -shared -o libstr.so -g strcnt.o strdrop.o
ldl:
	gcc ldl.c -o ldl.out -Wl,-rpath,. -Wall -Wextra -g libstr.so 
rdl:
	gcc rdl.c -o rdl.out -Wl,-rpath,. -Wall -Wextra -g -ldl
clean:
	rm -f *.o *.out *.so 
