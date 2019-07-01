library_test.exe: 
	make create_folders

	gcc -c main.c -o main.o
	gcc -c my_math.c -fPIC -o dynamic/my_math.o	
	gcc -c names.c -o static/names.o 	
	#making static library
	ar rcs static/libhwnames.a static/names.o 
	#making dynamic library
	gcc -shared dynamic/my_math.o -o dynamic/libhwmath.so
	#linking	
	gcc main.o -Lstatic/ -lhwnames -Ldynamic/ -lhwmath -o library_test.exe	

	#moving shared library to /usr/lib
	
	sudo mv dynamic/libhwmath.so /usr/lib
	sudo chmod 755 /usr/lib/libhwmath.so

	#testing main
	./library_test.exe
	
create_folders:
	mkdir static
	mkdir dynamic
