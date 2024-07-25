objects = intcontr.o checksum.o read_file.o input_handler.o fileconf_handler.o
flags = -c -g

intcontr : $(objects)
	gcc -g -o intcontr $(objects) -lssl -lcrypto -static-libgcc


intcontr.o : intcontr.c
	gcc $(flags) intcontr.c intcontr.h

checksum.o : checksum.c
	gcc $(flags) checksum.c intcontr.h

read_file.o : read_file.c
	gcc $(flags) read_file.c intcontr.h

input_handler.o : input_handler.c 
	gcc $(flags) input_handler.c intcontr.h

fileconf_handler.o : fileconf_handler.c
	gcc $(flags) fileconf_handler.c intcontr.h

.PHONY : clean
clean:
	rm -f intcontr *.o intcontr.h.gch