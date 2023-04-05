all: test_server1 test_client1

test_server1: test_server1.cpp server1.cpp server1.h global1.h
        g++ -std=c++14 -o test_server1 test_server1.cpp server1.cpp -lpthread -lmysqlclient
        cp test_server1 ../bin

test_client1: test_client1.cpp client1.cpp client1.h global1.h
        g++ -std=c++14 -o test_client1 test_client1.cpp client1.cpp -lpthread
        cp test_client1 ../bin

clean:
        rm -f test_server1 test_client1