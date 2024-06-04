/*
 * @Author       : mark
 * @Date         : 2020-06-18
 * @copyleft Apache 2.0
 */ 
#include <unistd.h>
#include "server/webserver.h"

int main() {
    /* 守护进程 后台运行 */
    //daemon(1, 0); 

    int port = 1316;                            //服务器端口
    int trigMode = 3;                           //ET模式
    int timeoutMS = 60000;                      //timeoutMs
    bool OptLinger = false;                     //优雅退出

    int sqlPort = 3306;                         //sql端口
    const char* sqlUser = "wlw";                //sql用户名
    const char* sqlPwd = "wlw_22021211637";     //sql密码
    const char* dbName = "web_heap_db";         //sql数据库名
    int connPoolNum = 12;                       //连接池数量
    int threadNum = 12;                         //线程池数量

    bool openLog = true;                        //日志开关
    int logLevel = 1;                           //日志等级
    int logQueSize = 1024;                      //日志异步队列容量

    WebServer server(
        port, trigMode, timeoutMS, OptLinger,                               
        sqlPort, sqlUser, sqlPwd, dbName, connPoolNum, threadNum, 
        openLog, logLevel, logQueSize);             
    server.Start();
} 
  