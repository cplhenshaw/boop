PROJECT = obj3do
OBJ = main.o read3do.o modl.o write3do.o writeObj.o checkedMem.o objStructs.o readObj.o update3do.o

C99 = gcc -std=c99
CFLAGS = -Wall -Werror -pedantic

$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ)

main.o : modl.h read3do.h main.c
	$(C99) $(CFLAGS) -c -o main.o main.c

read3do.o : modl.h checkedMem.h read3do.h read3do.c
	$(C99) $(CFLAGS) -c -o read3do.o read3do.c 

modl.o : modl.h modl.c
	$(C99) $(CFLAGS) -c -o modl.o modl.c

write3do.o : modl.h write3do.h write3do.c
	$(C99) $(CFLAGS) -c -o write3do.o write3do.c

writeObj.o : modl.h writeObj.h writeObj.c
	$(C99) $(CFLAGS) -c -o writeObj.o writeObj.c

checkedMem.o : checkedMem.h checkedMem.c
	$(C99) $(CFLAGS) -c -o checkedMem.o checkedMem.c

objStructs.o : checkedMem.h objStructs.h objStructs.c
	$(C99) $(CFLAGS) -c -o objStructs.o objStructs.c

readObj.o : objStructs.h checkedMem.h readObj.h readObj.c
	$(C99) $(CFLAGS) -c -o readObj.o readObj.c

update3do.o : objStructs.h modl.h checkedMem.h update3do.h update3do.c
	$(C99) $(CFLAGS) -c -o update3do.o update3do.c

clean:
	rm -f $(OBJ) $(PROJECT)
