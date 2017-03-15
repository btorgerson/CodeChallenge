Server/Network.o: Server/Network.cpp Server/Network.h
	gcc -std=c++11 -c Server/Network.cpp -I./boost/include -I./ntl/include -I./ -o Server/Network.o

Server/Worker.o: Server/Worker.cpp Server/Worker.h
	gcc -std=c++11 -c Server/Worker.cpp -I./boost/include -I./ntl/include -I./ -o Server/Worker.o

Server/main.o: Server/main.cpp
	gcc -std=c++11 -c Server/main.cpp -I./boost/include -I./ntl/include -I./ -o Server/main.o

server: Server/Network.o Server/Worker.o Server/main.o
	gcc Server/Network.o Server/Worker.o Server/main.o -L./boost/lib -L./ntl/lib -L./gmp/lib -lntl -lstdc++ -lm -lgmp -lboost_system -lpthread -o server

Client/UserInput/Main.o: Client/UserInput/Main.cpp
	gcc -std=c++11 -c Client/UserInput/Main.cpp -I./boost/include -I./ntl/include -I./ -I./Client -o Client/UserInput/Main.o

Client/ClientNetwork.o: Client/ClientNetwork.cpp Client/ClientNetwork.h
	gcc -std=c++11 -c Client/ClientNetwork.cpp -I./boost/include -I./ntl/include -I./ -I./Client -o Client/ClientNetwork.o

client: Client/ClientNetwork.o Client/UserInput/Main.o
	gcc Client/ClientNetwork.o Client/UserInput/Main.o -L./boost/lib -L./ntl/lib -L./gmp/lib -lntl -lstdc++ -lm -lgmp -lboost_system -lpthread -o client

all: server client
