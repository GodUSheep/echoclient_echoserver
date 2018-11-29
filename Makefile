all : echoclient echoserver

echoclient: client.c
	gcc -g -o echoclient client.c -lpcap

server: server.cpp
	g++ -g -o echoserver server.cpp -lpcap

clean:
	rm -f echoclient
	rm -f echoserver
	rm -f *.o