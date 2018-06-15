all:
	# gcc -fPIC -c -o libfaketime.o libfaketime.c -ldl -lm -lrt
	g++ -shared -o liboppsync.so -fPIC client.cpp -ldl # libfaketime.o -lrt

#usage: LD_PRELOAD=./liboppsync.so xclock -digital -update 1

