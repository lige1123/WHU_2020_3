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

struct Bpoint {
    double x;
    double y;
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

struct Map {            //球场信息
    int width;
    int height;
    struct Point start;
    int gate_width;
    int gate_height;
};

struct Aspeed {         //球加速度
    double x;
    double y;
};

struct Speed {          //球的速度
    double x;
    double y;
};

struct BallStatus {     //球的状态
    struct Speed v;
    struct Aspeed a;
    int by_time;
    char name[20];
    //pthread_mutex_t mutex;
};

struct Score {          //比分信息
    int red;
    int blue;
};

//actino的值
#define ACTION_KICK 0x01    //踢球  
#define ACTION_CARRY 0x02   //带球
#define ACTION_STOP 0x04    //停球
#define ACTION_DFL 0x08     //默认

struct Ctl {            //控制信息
    int action;         //动作
    int dirx;           //方向
    int diry;
    int strength;       //力度
};


//type的值
#define FT_HEART 0x01   //心跳
#define FT_ACK 0x02     //ACK
#define FT_MSG 0x04     //队内私聊消息
#define FT_WALL 0x08    //广播信息
#define FT_CTL 0x10     //控球
#define FT_MAP 0x20     //场地数据
#define FT_FIN 0x40     //下线信息
#define FT_SCORE 0x80   //分数信息
#define FT_GAMEOVER 0x100 //结束

#define MAX_MSG 1024

struct FootBallMsg {
    int type;
    int size;
    int team;
    char name[20];
    char msg[MAX_MSG];
};

#endif
