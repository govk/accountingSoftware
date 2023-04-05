#include "server1.h"

vector<bool> server::sock_arr(10000,false);    //将10000个位置都设为false，sock_arr[i]=false表示套接字描述符i未打开（因此不能关闭）

//构造函数
server::server(int port,string ip):server_port(port),server_ip(ip){}

//析构函数
server::~server(){
    for(int i=0;i<sock_arr.size();i++){
        if(sock_arr[i])
            close(i);
    }
    close(server_sockfd);
}
//服务器开始服务
void server::run(){
    //定义sockfd
    server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

    //定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;//TCP/IP协议族
    server_sockaddr.sin_port = htons(server_port);//server_port;//端口号
    server_sockaddr.sin_addr.s_addr = inet_addr(server_ip.c_str());//ip地址，127.0.0.1是环回地址，>相当于本机ip

    //bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");//输出错误原因
        exit(1);//结束程序
    }

    //listen，成功返回0，出错返回-1
    if(listen(server_sockfd,20) == -1)
    {
        perror("listen");//输出错误原因
        exit(1);//结束程序
    }
    //客户端套接字
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    //不断取出新连接并创建子线程为其服务
    while(1){
        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
        if(conn<0)
        {
            perror("connect");//输出错误原因
            exit(1);//结束程序
        }
        cout<<"用户"<<conn<<"上线！\n";
        sock_arr.push_back(conn);
        //创建线程
        thread t(server::RecvMsg,conn);
        t.detach();//置为分离状态，不能用join，join会导致主线程阻塞
    }
}

//子线程工作的静态函数
void server::RecvMsg(int conn){
    tuple<bool,string> info;//元组类型，if_login、login_name
    get<0>(info)=false;//把if_login置为false

    //不断接收数据
    while(1)
    {
        struct user li;
        memset(&li,0,sizeof(struct user));
        int len = recv(conn, &li, sizeof(li),0);
        //客户端发送exit或者异常结束时，退出
        if(strcmp(li.tmp,"exit")==0 || len<=0){
	    cout<<"用户"<<conn<<"退出！\n\n";
            close(conn);
            sock_arr[conn]=false;
            break;
        }
        cout<<conn<<"发来的信息:"<<li.tmp<<endl;
        HandleRequest(conn,&li,info);
    }
}

void server::HandleRequest(int conn,struct user *li,tuple<bool,string> &info){
    //把参数提出来，方便操作
    bool if_login=get<0>(info);//记录当前服务对象是否成功登录
    string login_name=get<1>(info);//记录当前服务对象的名字

    //连接MYSQL数据库
    MYSQL *con=mysql_init(NULL);
    mysql_real_connect(con,"127.0.0.1","root","","AccountingSoftware",0,NULL,CLIENT_MULTI_STATEMENTS);

        //登录
        if(strcmp(li->tmp,"登录")==0){
                string search="SELECT * FROM user WHERE UserID=\"";
                search+=li->UserID;
                search+="\";";

                cout<<"sql语句:"<<search<<endl;
                auto search_res=mysql_query(con,search.c_str());
                auto result=mysql_store_result(con);

                int col=mysql_num_fields(result);//获取列数
                int row=mysql_num_rows(result);//获取行数

                //查询到用户名
                if(search_res==0&&row!=0){
                    cout<<"查询成功\n";
                    auto info=mysql_fetch_row(result);//获取一行的信息
                    cout<<"查询到用户名:"<<info[0]<<" 密码:"<<info[3]<<endl;

                        //密码正确
                    if(strcmp(info[3],li->UserPassword)==0){
                        cout<<"登录密码正确\n\n";
                        string str1="ok";
                        if_login=true;
                        login_name=info[1];//记录下当前登录的姓名
                        send(conn,str1.c_str(),str1.length()+1,0);
                    }
                    //密码错误
                    else{
                        cout<<"登录密码错误\n\n";
                        char str1[100]="wrong";
                        send(conn,str1,strlen(str1),0);
                    }
                }
                //没找到用户名
                else{
                    cout<<"查询失败\n\n";
                    char str1[100]="wrong";
                    send(conn,str1,strlen(str1),0);
                }
        }

        //注册
        if(strcmp(li->tmp,"注册")==0){

                string insert="INSERT INTO user(UserID,UserName,UserAge,UserPassword) VALUES (\"";
                insert+=li->UserID;
                insert+="\",\"";
                insert+=li->UserName;
                insert+="\",";
                insert+=li->UserAge;
                insert+=",\"";
                insert+=li->UserPassword;
                insert+="\");";

                cout<<"sql语句:"<<insert<<endl<<endl;
                mysql_query(con,insert.c_str());

        }

        //记录支出
        if(strcmp(li->tmp,"记录支出")==0){
        	string insert="insert into account values(";
        	insert+="\"";
        	insert+=li->ExpensesType;
        	insert+="\",2,\"";
        	insert+=li->Remark;
        	insert+="\");";
        	cout<<"sql语句:"<<insert<<endl<<endl;
            mysql_query(con,insert.c_str());
            
        //insert into expense(ExpensesMoney,ExpensesType,ExpensesDate,ExpensesLocation,Remark,ExpensesID)values(200,"20220725",now(),"home","eat","1793928068");
            insert="insert into expense(ExpensesMoney,ExpensesType,ExpensesDate,ExpensesLocation,Remark,ExpensesID)values(";
            insert+=li->ExpensesMoney;
            insert+=",\"";
            insert+=li->ExpensesType;
            insert+="\",now(),\"";
            insert+=li->ExpensesLocation;
            insert+="\",\"";
            insert+=li->Remark;
            insert+="\",\"";
            insert+=li->UserID;
            insert+="\");";

            cout<<"sql语句:"<<insert<<endl<<endl;
            mysql_query(con,insert.c_str());

        }

        //记录收入
        if(strcmp(li->tmp,"记录收入")==0){
        	string insert="insert into account values(";
        	insert+="\"";
        	insert+=li->IncomeType;
        	insert+="\",1,\"";
        	insert+=li->Remark;
        	insert+="\");";
        	cout<<"sql语句:"<<insert<<endl<<endl;
            mysql_query(con,insert.c_str());
            
            //insert into expense(IncomeMoney,ExpensesType,ExpensesDate,ExpensesLocation,Remark,ExpensesID)values(200,"20220725",now(),"eat","1793928068");
		insert="INSERT INTO income(IncomeMoney,IncomeType,IncomeDate,Remark,IncomeID) VALUES (";
            insert+=li->IncomeMoney;
            insert+=",\"";
            insert+=li->IncomeType;
            insert+="\",now(),\"";
            insert+=li->Remark;
            insert+="\",\"";
            insert+=li->UserID;
            insert+="\");";

            cout<<"sql语句:"<<insert<<endl<<endl;
            mysql_query(con,insert.c_str());
        }

        //支出查询
        if(strcmp(li->tmp,"支出查询")==0){
	    //SELECT ExpensesType,ExpensesDate,ExpensesMoney,Expenseslocation,Remark from expense join account on expense.ExpensesType=account.AccountTypeID and ExpensesID in ('1793928068');
	    string select="SELECT ExpensesType,ExpensesDate,ExpensesMoney,Expenseslocation,Remark from expense join account on expense.ExpensesType=account.AccountTypeID and ExpensesID in (\'";
	    select+=li->UserID;
	    select+="\');";

            auto select_res=mysql_query(con,select.c_str());
            auto result=mysql_store_result(con);

            int col=mysql_num_fields(result);
            int row=mysql_num_rows(result);
            if(select_res==0&&row!=0){
		cout<<"查询成功\n";
		for(int ii=0;ii<row;ii++){
                    auto info=mysql_fetch_row(result);//获取一行的信息
                    cout<<"消费单号:"<<info[0]<<"消费时间:"<<info[1]<<"消费金额:"<<info[2]<<"消费地点:"<<info[3]<<"备注:"<<info[4]<<endl;
		    string str1="消费单号:";
		    str1+=info[0];
		    str1+="消费时间:";
		    str1+=info[1];
		    str1+="消费金额:";
		    str1+=info[2];
		    str1+="消费地点:";
		    str1+=info[3];
		    str1+="备注:";
		    str1+=info[4];
 		    send(conn,str1.c_str(),str1.length()+1,0);;
		}
		string str1="over";
		send(conn,str1.c_str(),str1.length()+1,0);
		cout<<"over\n\n";
	    }
            else{
                cout<<"查询失败\n\n";
                char str1[100]="wrong";
                send(conn,str1,strlen(str1),0);
            }
        }

        //收入查询
        if(strcmp(li->tmp,"收入查询")==0){
	    //SELECT IncomeType,IncomeDate,IncomeMoney,Remark from income join account on income.IncomeType=account.AccountTypeID and IncomeID in ('1793928068');
	    string select="SELECT IncomeType,IncomeDate,IncomeMoney,Remark from income join account on income.IncomeType=account.AccountTypeID and IncomeID in (\'";
	    select+=li->UserID;
	    select+="\');";
	    auto select_res=mysql_query(con,select.c_str());
            auto result=mysql_store_result(con);

            int col=mysql_num_fields(result);
            int row=mysql_num_rows(result);
            if(select_res==0&&row!=0){
                cout<<"查询成功\n";
                for(int ii=0;ii<row;ii++){
                    auto info=mysql_fetch_row(result);//获取一行的信息
                    cout<<"收入单号:"<<info[0]<<"收入日期:"<<info[1]<<"收入金额:"<<info[2]<<"备注:"<<info[3]<<endl;
                    string str1="收入单号:";
                    str1+=info[0];
                    str1+="收入日期:";
                    str1+=info[1];
                    str1+="收入金额:";
                    str1+=info[2];
                    str1+="备注:";
                    str1+=info[3];
                    send(conn,str1.c_str(),str1.length()+1,0);;
                }
                string str1="over";
                send(conn,str1.c_str(),str1.length()+1,0);
                cout<<"over\n\n";
            }
            else{
                cout<<"查询失败\n\n";
                char str1[100]="wrong";
                send(conn,str1,strlen(str1),0);
            }
        }

        //个人信息查询
        if(strcmp(li->tmp,"个人信息查询")==0){
	    string search("select * from user where UserID = \"");
	    search+=li->UserID;
	    search+="\";";
	    cout<<search<<endl;
	    auto select_res=mysql_query(con,search.c_str());
            auto result=mysql_store_result(con);

            int col=mysql_num_fields(result);
            int row=mysql_num_rows(result);
            if(select_res==0&&row!=0){
		struct user jia;
		memset(&jia,0,sizeof(jia));
                cout<<"查询成功\n\n";
                auto info=mysql_fetch_row(result);//获取一行的信息
		strcpy(jia.UserName,info[1]);
		strcpy(jia.UserAge,info[2]);	
		strcpy(jia.BankAccount,info[4]);
		strcpy(jia.UserID,li->UserID);
                send(conn,&jia,sizeof(jia),0);
            }
            else{
                cout<<"查询失败\n\n";
            }
        }


    //更新实参
    get<0>(info)=if_login;//记录当前服务对象是否成功登录
    get<1>(info)=login_name;//记录当前服务对象的名字
}
