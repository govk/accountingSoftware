#include "client1.h"

client::client(int port,string ip):server_port(port),server_ip(ip){}

client::~client(){
    close(sock);
}

void client::run(){

    //定义sockfd
    sock = socket(AF_INET,SOCK_STREAM, 0);

    //定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server_port);  //服务器端口
    servaddr.sin_addr.s_addr = inet_addr(server_ip.c_str());  //服务器ip

    //连接服务器，成功返回0，错误返回-1
    if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    HandleClient(sock);
    return;
}

void client::HandleClient(int conn){
    char choice;                //储存选项
    string name,pass,pass1;     //储存姓名和密码值

    struct user li;
        memset(&li,0,sizeof(struct user));

    bool if_login=false;//记录是否登录成功
    string login_name;//记录成功登录的用户名

    system("clear");
        cout << " ------------------\n";
        cout << "|                  |\n";
        cout << "|  记账堡:         |\n";
        cout << "|    1:登录        |\n";
        cout << "|    2:注册        |\n";
        cout << "|    0:退出应用    |\n";
        cout << "|                  |\n";
        cout << " ------------------ \n\n";
    //开始处理各种事务
    while(1){
        if(if_login)  break;
        cout<<"输入选项:";
        cin>>choice;
        if(choice=='0')
            break;
        //注册
        else if(choice=='2'){
                strcpy(li.tmp,"注册");
                //用户名
                regex r_name("\\d{10}");
                while(1){
                        cout<<"注册的用户名(十位数字):";
                        cin>>name;
                        if(regex_match(name, r_name)) break;
                        cout<<"用户名不合法！"<<endl;
                }
                strcpy(li.UserID,name.c_str());
                //密码
            while(1){
                cout<<"密码:";
                cin>>pass;
                cout<<"确认密码:";
                cin>>pass1;
                if(pass==pass1)
                    break;
                else
                    cout<<"两次密码不一致!\n\n";
            }
            strcpy(li.UserPassword,pass.c_str());
            //用户信息
                        cout<<"姓名:";
            cin>>li.UserName;
            regex r_age("((1[0-1])|[1-9])?\\d");
                while(1){
                        cout<<"年龄:";
                        cin>>li.UserAge;
                        if(regex_match(li.UserAge,r_age)) break;
                        cout<<"年龄不合法！"<<endl;
                }

            send(conn,&li,sizeof(li),0);
            cout<<"注册成功！\n";
                }
        //登录
        else if(choice=='1'&&!if_login){
            while(1){
                strcpy(li.tmp,"登录");
                cout<<"用户名:";
                cin>>li.UserID;
                cout<<"密码:";
                cin>>li.UserPassword;
                send(conn,&li,sizeof(li),0);//发送登录信息
                char buffer[1000];
                memset(buffer,0,sizeof(buffer));
                recv(sock,buffer,sizeof(buffer),0);//接收响应
                string recv_str(buffer);
                //登录成功
                if(recv_str.substr(0,2)=="ok"){
                    if_login=true;
                    cout<<"登录成功\n\n";
                    break;
                }
                //登录失败
                else  cout<<"密码或用户名错误！\n\n";
            }
        }
        else cout<<"请按提示输入选项进行操作！"<<endl;
    }
    //登录成功
    if(if_login){
        system("clear");
        cout << " -------------------------------------------\n";
        cout << "                                           \n";
        cout << "             记账堡\n";
        cout << "              1:记录支出                  \n";
        cout << "              2:记录收入                  \n";
        cout << "              3:支出查询                   \n";
        cout << "              4:收入查询                 \n";
        cout << "              5:个人信息查询            \n";
        cout << "              0:退出应用                \n";
        cout << "                                        \n";
        cout << " ------------------------------------------- \n\n";
    }
    while(if_login&&1){
        cout<<"\n输入选项:";
        cin>>choice;
        if(choice=='0')  break;

        //记录支出
        if(choice=='1'){
            strcpy(li.tmp,"记录支出");
            
            time_t tnow;
  	    tnow=time(0);
	    sprintf(li.ExpensesType, "%ld", tnow);  			

            cout<<"消费金额:";
            cin>>li.ExpensesMoney;
            cout<<"消费地点:";
            cin>>li.ExpensesLocation;
            cout<<"备注:";
            cin>>li.Remark;
            send(conn,&li,sizeof(li),0);//发送登录信息
            cout<<"记录成功！"<<endl;
        }

        //记录收入
        else if(choice=='2'){
            strcpy(li.tmp,"记录收入");
            
            time_t tnow;
  	    tnow=time(0);
	    sprintf(li.IncomeType, "%ld", tnow);  			
  			
            cout<<"收入金额:";
            cin>>li.IncomeMoney;
            cout<<"备注:";
            cin>>li.Remark;
            send(conn,&li,sizeof(li),0);//发送登录信息
            cout<<"记录成功！"<<endl;
        }
        //支出查询
        else if(choice=='3'){
            strcpy(li.tmp,"支出查询");
	    send(conn,&li,sizeof(li),0);
	    char buffer[1000];
	    while(1){
           	memset(buffer,0,sizeof(buffer));
          	recv(sock,buffer,sizeof(buffer),0);//接收响应
                string recv_str(buffer);
		if(recv_str=="over"||recv_str=="wrong") break;
	        cout<<recv_str<<endl;
	    }
        }

        //收入查询
        else if(choice=='4'){
            strcpy(li.tmp,"收入查询");
	    send(conn,&li,sizeof(li),0);
            char buffer[1000];
            while(1){
                memset(buffer,0,sizeof(buffer));
                recv(sock,buffer,sizeof(buffer),0);//接收响应
                string recv_str(buffer);
                if(recv_str=="over"||recv_str=="wrong") break;
                cout<<recv_str<<endl;
            }
        }

        //个人信息查询
        else if(choice=='5'){
            strcpy(li.tmp,"个人信息查询");
	    send(conn,&li,sizeof(li),0);
	    struct user jia;
   	    memset(&jia,0,sizeof(jia));	
            recv(sock,&jia,sizeof(jia),0);//接收响应
	    userinfo(&jia);
        }

        else cout<<"请按提示输入选项进行操作！"<<endl;
    }
    close(sock);
}


void client::userinfo(struct user *user) {
        cout << " ---------------------------------------------------\n";
        cout << endl;
        cout << "            记账堡:" << endl;
        cout << "              用户名：" << user->UserID << "（不可更改）" << endl;
        cout << "              姓名：" << user->UserName << endl;
        cout << "              年龄：" << user->UserAge << endl;
        cout << "              银行卡号：" << user->BankAccount << endl << endl;
//      cout << "              1:修改个人信息" << endl;
//        cout << "              0:(退出)返回记账堡界面" << endl;
        cout << " ---------------------------------------------------\n";
        cout << endl << endl;
	return;
}
