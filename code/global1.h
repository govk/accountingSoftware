#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <regex>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <time.h>
#include <thread>
#include <vector>
#include <mysql/mysql.h>    //myql
#include <unordered_map>    //map容器
#include <pthread.h>        //互斥锁
#include <set>              //将处于同一群聊的套接字描述符放入同一个 set 中
using namespace std;

struct user {
        char tmp[100];

        //user表
        char UserID[20];
        char UserName[20];
        char UserAge[10];
        char UserPassword[16];
        char BankAccount[18];

        //income表
        char IncomeMoney[10];
        char IncomeType[50];
        char Remark[255];

        //expense表
        char ExpensesMoney[10];
        char ExpensesType[50];
        char ExpensesLocation[255];
};

#endif
