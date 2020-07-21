/*************************************************************************
	> File Name: datatype.h
	> Author: 
	> Mail: 
	> Created Time: Mon 20 Jul 2020 10:16:47 PM CST
 ************************************************************************/

#ifndef _DATATYPE_H
#define _DATATYPE_H

struct Point {
    int x;
    int y;
};

struct User {
    int team;           // 0 RED, 1 BLUE
    int fd;             //该玩家对应的连接
    char name[20];      //玩家名称
    int online;         //1 在线, 0 不在线
    int flag;           //未响应次数
    struct Point loc;   //坐标
};

struct LogRequest {     //请求报文
    char name[20];
    int team;
    char msg[512];
};

struct LogResponse {    //响应报文
    int type;           //0 OK, 1 NO
    char msg[512];
};

#endif
