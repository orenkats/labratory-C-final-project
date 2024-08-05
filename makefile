assembler: main.o translate_macro.o utils.o first_path.o second_path.o entry_extern.o path_utils.o
	gcc -g -Wall -ansi -pedantic main.o translate_macro.o utils.o first_path.o second_path.o entry_extern.o path_utils.o -o assembler -lm

main.o: main.c functions.h libary_h.h macro.h global_var.h
	gcc -ansi -Wall -pedantic -c main.c -o main.o

translate_macro.o: translate_macro.c macro.h functions.h libary_h.h 
	gcc -ansi -Wall -pedantic -c translate_macro.c -o translate_macro.o

first_path.o: first_path.c functions.h libary_h.h global_var.h macro.h
	gcc -ansi -Wall -pedantic -c first_path.c -o first_path.o

second_path.o: second_path.c functions.h libary_h.h global_var.h macro.h
	gcc -ansi -Wall -pedantic -c second_path.c -o second_path.o

utils.o: utils.c libary_h.h global_var.h macro.h
	gcc -ansi -Wall -pedantic -c utils.c -o utils.o

entry_extern.o: entry_extern.c libary_h.h global_var.h macro.h
	gcc -ansi -Wall -pedantic -c entry_extern.c -o entry_extern.o

path_utils.o: path_utils.c libary_h.h global_var.h macro.h
	gcc -ansi -Wall -pedantic -c path_utils.c -o path_utils.o







