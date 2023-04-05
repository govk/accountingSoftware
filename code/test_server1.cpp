#include "server1.h"
int main(int argc,char *argv[]){

    if(argc!=3){
        cout<<"用法：./test_server1 port ip"<<endl;
        cout<<"示例：/home/lijialin/accountingSoftware/bin/test_server1 8888 127.0.0.1"<<endl;
        return -1;
    }
    server serv(stoi(argv[1]),argv[2]);//创建实例，传入端口号和ip作为构造函数参数
    serv.run();//启动服务
}
