host.o : main.o
	gcc -L /usr/local/cuda/lib64 -l OpenCL main.o -o host

main.o : main.c
	gcc -c -I /usr/local/cuda/include main.c -o main.o

clean :
	rm main.o host
