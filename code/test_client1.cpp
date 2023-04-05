#include "client1.h"

int main(int argc,char *argv[]){
  if(argc!=3){
        cout<<"用法：./test_client port ip"<<endl;
        cout<<"示例：/home/lijialin/accountingSoftware/bin/test_client 8888 127.0.0.1"<<endl;
        return -1;
    }
  client cli(stoi(argv[1]),argv[2]);
  cli.run();
}
