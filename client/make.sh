
g++ -c -g client.cc -I .. -I /home/zhangjun/tools/protobuf-3.7.1/include/ -std=c++11

g++ -g -o client_proto client.o ../echo_service.pb.o -L /home/zhangjun/tools/protobuf-3.7.1/lib/ -lprotobuf -ljsoncpp -lpthread
