#ifndef _PACKDEF_H
#define _PACKDEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "err_str.h"
#include <malloc.h>
#include"tcpnet.h"
#include<iostream>
#include<map>
#include<list>
#include"redistool.h"
//#include<qtime>
class TcpNet;
typedef struct mythis
{
    TcpNet *pthis;
    int clientfd;
}mythis;
//边界值
#define _DEF_SIZE 45
#define _DEF_BUFFERSIZE 1000
#define DEF_PORT 8000
#define DEF_SERVERIP "127.0.0.1"  //TODO

#define WOLf_NUM 10
#define TRUE true
#define FALSE false


#define _DEF_LISTEN 128
#define _DEF_EPOLLSIZE 4096
#define _DEF_IPSIZE 16
#define _DEF_COUNT 10
#define _DEF_TIMEOUT 10
#define _DEF_SQLIEN 400



#define BOOL bool
#define DEF_PACK_BASE  (10000)
#define DEF_PACK_COUNT  (100)//注册
#define  DEF_PACK_REGISTER_RQ    (DEF_PACK_BASE + 0)
#define  DEF_PACK_REGISTER_RS    (DEF_PACK_BASE + 1)//登录
#define  DEF_PACK_LOGIN_RQ    (DEF_PACK_BASE + 2)
#define  DEF_PACK_LOGIN_RS    (DEF_PACK_BASE + 3)//创建房间
#define DEF_PACK_CREATEROOM_RQ  (DEF_PACK_BASE + 4)
#define DEF_PACK_CREATEROOM_RS  (DEF_PACK_BASE + 5)//加入房间
#define DEF_PACK_JOINROOM_RQ  (DEF_PACK_BASE + 6)
#define DEF_PACK_JOINROOM_RS  (DEF_PACK_BASE + 7)//房间列表请求
#define DEF_PACK_ROOM_MEMBER    (DEF_PACK_BASE + 8)//音频数据
#define DEF_PACK_AUDIO_FRAME    (DEF_PACK_BASE + 9)//视频数据
#define DEF_PACK_VIDEO_FRAME    (DEF_PACK_BASE + 10)//退出房间请求
#define DEF_PACK_LEAVEROOM_RQ   (DEF_PACK_BASE + 11)//退出房间回复
#define DEF_PACK_LEAVEROOM_RS   (DEF_PACK_BASE + 12)//音频注册
#define DEF_PACK_AUDIO_REGISTER (DEF_PACK_BASE + 13)
#define DEF_PACK_VIDEO_REGISTER (DEF_PACK_BASE + 14)//视频注册
#define DEF_PACK_VIDEO2_REGISTER (DEF_PACK_BASE + 15)
#define  DEF_PACK_JOINROOM_WOLF_RQ (DEF_PACK_BASE + 16) //加入狼人房间请求
#define DEF_PACK_JOINROOM_WOLF_RS (DEF_PACK_BASE + 17)//加入狼人房间回复请求
#define DEF_PACK_CREATEROOM_WOLF_RQ (DEF_PACK_BASE + 18)
#define DEF_PACK_CREATEROOM_WOLF_RS (DEF_PACK_BASE + 19)
#define DEF_PACK_WOLF_ROOM_MEMBER (DEF_PACK_BASE + 20)
#define DEF_PACK_LEAVEWOLFROOM_RQ (DEF_PACK_BASE + 21)
#define DEF_PACK_LEAVEWOLFALIVE_RQ (DEF_PACK_BASE + 22)
#define  DEF_PACK_READYWOLF_RQ (DEF_PACK_BASE + 23)
#define  DEF_PACK_WOLF_START (DEF_PACK_BASE+24)
#define  DEF_PACK_DAY_DARK (DEF_PACK_BASE+25)
#define  DEF_PACK_WOLF_KILL (DEF_PACK_BASE+26)
#define  DEF_PACK_KILL_VOTE (DEF_PACK_BASE + 27)//杀谁
#define  DEF_PACK_WITCH_SAVE (DEF_PACK_BASE + 28)
#define  DEF_PACK_SAVE_CHOOSE (DEF_PACK_BASE + 29)
#define  DEF_PACK_DIE (DEF_PACK_BASE + 30)
#define  DEF_PACK_WITCH_KILL (DEF_PACK_BASE + 31)
#define  DEF_PACK_PROHIT_WATCH (DEF_PACK_BASE + 32)
#define  DEF_PACK_DEAL_WATCH (DEF_PACK_BASE + 33)
#define  DEF_PACK_DEAL_WAKE (DEF_PACK_BASE + 34)
#define  DEF_PACK_SAYSOMETHING (DEF_PACK_BASE + 35)
#define  DEF_PACK_STOPSAY (DEF_PACK_BASE + 36)
#define  DEF_PACK_NORVOTE (DEF_PACK_BASE + 37)
#define  DEF_PACK_VOTEWHO  (DEF_PACK_BASE + 38)
#define  DEF_PACK_GAMEOVER  (DEF_PACK_BASE + 39)
#define  DEF_PACK_WOLF_MATCH  (DEF_PACK_BASE + 40)
#define  DEF_PACK_ADD_FRIEND  (DEF_PACK_BASE + 41)
#define DEF_PACK_ADD_RESULT (DEF_PACK_BASE + 42)
#define DEF_PACK_CHOOSE_INFOR (DEF_PACK_BASE + 43)
#define DEF_PACK_GetList  (DEF_PACK_BASE + 44)
#define DEF_PACK_GetListRS  (DEF_PACK_BASE + 45)
#define DEF_PACK_GetMemberLike  (DEF_PACK_BASE + 46)
#define DEF_PACK_GetMemberLikeRS  (DEF_PACK_BASE + 47)
#define DEF_PACK_SEEROOM  (DEF_PACK_BASE + 48)
#define DEF_PACK_SEEROOMRS  (DEF_PACK_BASE + 49)

#define userid_is_exist      0
#define register_sucess      1
#define name_is_exist        2

//登录请求结果
#define userid_no_exist      0
#define password_error       1
#define login_sucess         2
//#define user_online          3

//创建房间结果
#define room_is_exist        0
#define create_success       1

//加入房间结果
#define room_no_exist        0
#define join_success         1

template<class K , class V>
struct MyMap
{
public:
    MyMap(){
        pthread_mutex_init(&m_lock , NULL);
    }
    map<K , V> m_map;
    //获取的结果如果找不到就是对象v 如果是指针应该是NULL 规定该函数使用时，先判断他有没有,确保一定有。
    V find( K k)
    {

        pthread_mutex_lock(&m_lock );

              V v= m_map[k];
            pthread_mutex_unlock(&m_lock );
            return v;



    }
    V get(K k)
    {
        pthread_mutex_lock(&m_lock );
           V c=m_map[k];
        pthread_mutex_unlock(&m_lock );
        return c;
    }
    void add(K k)//只能整形用
    {

        pthread_mutex_lock(&m_lock );
        if(m_map[k]<WOLf_NUM)
           m_map[k]=m_map[k]+1;
        pthread_mutex_unlock(&m_lock );

    }
    void ded(K k)//只能整形用
    {
        pthread_mutex_lock(&m_lock );
        if(m_map[k]>0)
           m_map[k]=m_map[k]-1;
        pthread_mutex_unlock(&m_lock );
    }
    void insert( K k , V v)
    {
        pthread_mutex_lock(&m_lock );
        m_map[k] = v;
        pthread_mutex_unlock(&m_lock );
    }
    void erase(K k )
    {
        pthread_mutex_lock(&m_lock );
        if(m_map.count(k)>0)
        m_map.erase(k);
        pthread_mutex_unlock(&m_lock );
    }
    bool IsExist( K k )
    {
        bool flag = false;
        pthread_mutex_lock(&m_lock );
        if( m_map.count(k) > 0 )
            flag = true;
        pthread_mutex_unlock(&m_lock );
        return flag;
    }
private:
    pthread_mutex_t m_lock;
};

#define MAX_PATH            (260 )
#define MAX_SIZE            (60  )
#define DEF_HOBBY_COUNT     (8  )
#define MAX_CONTENT_LEN     (4096 )


/////////////////////网络//////////////////////////////////////


#define DEF_MAX_BUF	  1024
#define DEF_BUFF_SIZE	  4096


typedef int PackType;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct STRU_SEEROOMRS
{
    STRU_SEEROOMRS ()
    {
        m_nType=DEF_PACK_SEEROOMRS ;
        m_roomid=0;
    }
     PackType m_nType;
      int m_roomid;


}STRU_SEEROOMRS ;
typedef struct STRU_SEEROOM
{
    STRU_SEEROOM()
    {
        m_nType=DEF_PACK_SEEROOM;
        m_id=0;
    }
     PackType m_nType;
      int m_id;


}STRU_SEEROOM;
typedef struct STRU_GetMemberLike
{
    STRU_GetMemberLike()
    {
        m_nType=DEF_PACK_GetMemberLike;
          m_id=0;
    }
     PackType m_nType;
      int m_id;
}STRU_GetMemberLike;
typedef struct STRU_GetListRs
{
    STRU_GetListRs()
    {
        m_nType=DEF_PACK_GetListRS;
          memset(m_id,0,MAX_SIZE);
        size=0;
    }
     PackType m_nType;
      int m_id[50];
      int size;

}STRU_GetListRs;
typedef struct STRU_GetMembero
{
    STRU_GetMembero()
    {
        m_nType=DEF_PACK_GetMemberLikeRS;
          memset(m_id,0,MAX_SIZE);
        size=0;
    }
     PackType m_nType;
      int m_id[50];
      int size;

}STRU_GetMembero;
typedef struct STRU_GetList
{
    STRU_GetList()
    {
        m_nType=DEF_PACK_GetList;
        m_id=0;
    }
     PackType m_nType;
      int m_id;

}STRU_GetList;
typedef struct STRU_CHOOSE_ADDRESS
{
    STRU_CHOOSE_ADDRESS()
    {
        m_nType = DEF_PACK_CHOOSE_INFOR;
        m_id=0;
        memset(m_address,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    int     m_id ; //用户ID
    char     m_address[MAX_SIZE];  //密码
}STRU_CHOOSE_ADDRESS;
typedef struct STRU_ADD_RESULT
{
    STRU_ADD_RESULT()
    {
        m_nType=DEF_PACK_ADD_RESULT;
      m_result=0;
    }
     PackType m_nType;
      int m_result;

}STRU_ADD_RESULT;
typedef struct STRU_ADD_FRIEND
{
    STRU_ADD_FRIEND()
    {
        m_nType=DEF_PACK_ADD_FRIEND;
        m_bid=0;
        m_id=0;
    }
     PackType m_nType;
      int m_id;
      int m_bid;

}STRU_ADD_FRIEND;
typedef struct STRU_MATCH
{
    STRU_MATCH()
    {
        m_nType=DEF_PACK_WOLF_MATCH;
        m_id=0;

    }
     PackType m_nType;
     int m_id;


}STRU_MATCH;
typedef struct STRU_VOTE_WHO
{
    STRU_VOTE_WHO()
    {
        m_nType=DEF_PACK_VOTEWHO;
        m_roomid=0;
        m_id=0;
    }
     PackType m_nType;
     int m_roomid;
     int m_id;

}STRU_VOTE_WHO;
typedef struct STRU_NORVOTE
{
    STRU_NORVOTE()
    {
        m_nType=DEF_PACK_NORVOTE;
        m_roomid=0;

    }
     PackType m_nType;
     int m_roomid;

}STRU_NORVOTE;
typedef struct STRU_STOPSAY
{
    STRU_STOPSAY()
    {
        m_nType=DEF_PACK_STOPSAY;
        m_roomid=0;

    }
     PackType m_nType;
     int m_roomid;

}STRU_STOPSAY;
typedef struct STRU_SAY
{
    STRU_SAY()
    {
        m_nType=DEF_PACK_SAYSOMETHING;
        m_roomid=0;
        id=0;

    }
     PackType m_nType;
     int m_roomid;
     int id;

}STRU_SAY;
typedef struct STRU_GAMEOVER
{
    STRU_GAMEOVER()
    {
        m_nType=DEF_PACK_GAMEOVER;
        m_roomid=0;
        result=0;
    }
     PackType m_nType;
     int m_roomid;
     int result;


}STRU_GAMEOVER;
typedef struct STRU_WAKE
{
    STRU_WAKE()
    {
        m_nType=DEF_PACK_DEAL_WAKE;
        m_roomid=0;

    }
     PackType m_nType;
     int m_roomid;

}STRU_WAKE;
typedef struct STRU_DEAL_WATCH
{
    STRU_DEAL_WATCH()
    {
        m_nType=DEF_PACK_DEAL_WATCH;
        m_roomid=0;
        m_result=0;
    }
     PackType m_nType;
     int m_roomid;
     int  m_result;
}STRU_DEAL_WATCH;
typedef struct STRU_PROHIT_WATCH
{
    STRU_PROHIT_WATCH()
    {
        m_nType=DEF_PACK_PROHIT_WATCH;
        m_roomid=0;
        m_id=0;
    }
     PackType m_nType;
     int m_roomid;
     int  m_id;
}STRU_PROHIT_WATCH;

typedef struct STRU_WITCH_KILL
{
    STRU_WITCH_KILL()
    {
        m_nType=DEF_PACK_WITCH_KILL;
        m_roomid=0;
        m_id=0;
    }
     PackType m_nType;
     int m_roomid;
     int  m_id;
}STRU_WITCH_KILL;
typedef struct STRU_DIE
{
    STRU_DIE()
    {
        m_nType = DEF_PACK_DIE;

        m_roomid=0;
        m_id=0;
    }

    PackType m_nType;   //包类型
    int m_roomid;
    int m_id;

} STRU_DIE;
typedef struct STRU_SAVE_CHOOSE
{
    STRU_SAVE_CHOOSE()
    {
        m_nType = DEF_PACK_SAVE_CHOOSE;
        result=0;
        m_roomid=0;
    }

    PackType m_nType;   //包类型
    int result;
    int m_roomid;

} STRU_SAVE_CHOOSE;
typedef struct STRU_SWTICH_SAVE
{
    STRU_SWTICH_SAVE()
    {
        m_nType = DEF_PACK_WITCH_SAVE;
        m_id=0;
        m_roomid=0;
    }

    PackType m_nType;   //包类型
    int m_id;
    int m_roomid;

} STRU_SWTICH_SAVE;
typedef struct STRU_KILL_VOTE
{
    STRU_KILL_VOTE()
    {
        m_nType = DEF_PACK_KILL_VOTE;
        m_id=0;
        m_roomid=0;
    }

    PackType m_nType;   //包类型
    int m_id;
    int m_roomid;

} STRU_KILL_VOTE;
typedef struct STRU_WOLF_KILL
{
    STRU_WOLF_KILL()
    {
        m_nType = DEF_PACK_WOLF_KILL;
         m_id=0;
         m_roomid=0;
    }

    PackType m_nType;   //包类型
    int m_id;
    int m_roomid;

} STRU_WOLF_KILL;
typedef struct STRU_DAY_DARK
{
    STRU_DAY_DARK()
    {
        m_nType = DEF_PACK_DAY_DARK;
        m_id=0;
         m_roomid=0;
    }

    PackType m_nType;   //包类型
   int m_id;
   int m_roomid;
} STRU_DAY_DARK;
typedef struct STRU_WOLF_READY
{
    STRU_WOLF_READY()
    {
        m_nType = DEF_PACK_READYWOLF_RQ;
        m_id=0;
         m_roomid=0;
    }

    PackType m_nType;   //包类型
   int m_id;
   int m_roomid;
} STRU_WOLF_READY;
typedef struct STRU_JOINROOM_WOLF_RQ
{
    STRU_JOINROOM_WOLF_RQ()
    {
        m_nType = DEF_PACK_JOINROOM_WOLF_RQ;
        m_UserID = 0;
        m_RoomID=0;
    }

    PackType m_nType;   //包类型
    int m_UserID;
      int m_RoomID ;

}STRU_JOINROOM_WOLF_RQ;
typedef struct STRU_ROOM_START_RQ
{
    STRU_ROOM_START_RQ()
    {
        m_nType = DEF_PACK_WOLF_START;
        m_idtype = 0;

    }

    PackType m_nType;   //包类型
    int m_idtype;
    int wolfmember[5];



}STRU_ROOM_START_RQ;
typedef struct STRU_ALIVE
{
    STRU_ALIVE()
    {
        m_nType = DEF_PACK_LEAVEWOLFALIVE_RQ;
        id=0;
    }

    PackType m_nType;   //包类型
    int id;

} STRU_ALIVE;
typedef struct STRU_JOINROOM_WOLF_RS
{
     STRU_JOINROOM_WOLF_RS()
    {
        m_nType = DEF_PACK_JOINROOM_WOLF_RS;
        m_UserID = 0;
        m_RoomID=0;
        m_lResult=0;
    }

    PackType m_nType;   //包类型
    int m_UserID;
    int m_RoomID ;
    int  m_lResult ;

}STRU_JOINROOM_WOLF_RS;
typedef struct STRU_CRATEROOM_WOLF_RQ
{
    STRU_CRATEROOM_WOLF_RQ()
    {
        m_nType = DEF_PACK_CREATEROOM_WOLF_RQ;
        m_UserID = 0;

    }

    PackType m_nType;   //包类型
    int m_UserID;


}STRU_CRATEROOM_WOLF_RQ;
typedef struct STRU_CREATEROOM_WOLF_RS
{
     STRU_CREATEROOM_WOLF_RS()
    {
        m_nType = DEF_PACK_CREATEROOM_WOLF_RS;
        m_UserID = 0;
        m_lResult=0;
        m_RoomID=0;
    }

    PackType m_nType;   //包类型
    int m_UserID;
    int m_lResult;
    int m_RoomID ;

}STRU_CREATEROOM_WOLF_RS;
struct STRU_VIDEO2_REGISTER
{
    STRU_VIDEO2_REGISTER():m_nType(DEF_PACK_VIDEO2_REGISTER){
        m_userid = 0;
    }

    PackType m_nType;   //包类型
    int m_userid;
};


//登录请求
typedef struct STRU_LOGIN_RQ
{
    STRU_LOGIN_RQ()
    {
        m_nType = DEF_PACK_LOGIN_RQ;
        memset(m_tel,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_tel[MAX_SIZE] ; //用户ID
    char     m_szPassword[MAX_SIZE]; //密码
} STRU_LOGIN_RQ;


//登录回复
typedef struct STRU_LOGIN_RS
{
    STRU_LOGIN_RS()
    {
        m_nType= DEF_PACK_LOGIN_RS;
        m_userID = 0;
        m_lResult = 0;
        memset(m_szName,0,MAX_SIZE);
    }
    PackType m_nType;   //包类型
    int  m_userID;
    int  m_lResult ; //注册结果
    char m_szName[MAX_SIZE];

} STRU_LOGIN_RS;


//注册请求
typedef struct STRU_REGISTER_RQ
{
    STRU_REGISTER_RQ()
    {
        m_nType = DEF_PACK_REGISTER_RQ;
        memset(m_tel,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
        memset(m_name,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_tel[MAX_SIZE] ; //用户名
    char     m_szPassword[MAX_SIZE];  //密码
    char      m_name[MAX_SIZE];

} STRU_REGISTER_RQ;

//注册回复
typedef struct STRU_REGISTER_RS
{
    STRU_REGISTER_RS()
    {
        m_nType= DEF_PACK_REGISTER_RS;
        m_lResult = 0;
    }
    PackType m_nType;   //包类型
    int  m_lResult ; //注册结果

} STRU_REGISTER_RS;



typedef struct STRU_CREATEROOM_RQ
{
    STRU_CREATEROOM_RQ()
    {
        m_nType = DEF_PACK_CREATEROOM_RQ;
        m_UserID = 0;
    }

    PackType m_nType;   //包类型
    int m_UserID;

}STRU_CREATEROOM_RQ;

//创建房间回复
typedef struct STRU_CREATEROOM_RS
{
    STRU_CREATEROOM_RS()
    {
        m_nType= DEF_PACK_CREATEROOM_RS;
        m_lResult = 0;
        m_RoomId = 0;
    }
    PackType m_nType;   //包类型
    int  m_lResult ;    //创建结果
    int  m_RoomId;

}STRU_CREATEROOM_RS;

//加入房间请求
typedef struct STRU_JOINROOM_RQ
{
    STRU_JOINROOM_RQ()
    {
        m_nType = DEF_PACK_JOINROOM_RQ;
        m_UserID = 0;
        m_RoomID = 0;
    }

    PackType m_nType;   //包类型
    int m_UserID;
    int m_RoomID;

}STRU_JOINROOM_RQ;

//加入房间回复
typedef struct STRU_JOINROOM_RS
{
    STRU_JOINROOM_RS()
    {
        m_nType= DEF_PACK_JOINROOM_RS;
        m_lResult = 0;
        m_RoomID = 0;
    }
    PackType m_nType;   //包类型
    int  m_lResult ;    //注册结果
    int m_RoomID;
}STRU_JOINROOM_RS;
//房间成员请求
typedef struct STRU_ROOM_MEMBER_RQ
{
    STRU_ROOM_MEMBER_RQ()
    {
        m_nType= DEF_PACK_ROOM_MEMBER;
        m_UserID =0;
        memset(m_szUser,0,MAX_SIZE);
    }
    PackType m_nType;   //包类型
    int m_UserID;
    char m_szUser[MAX_SIZE];

}STRU_ROOM_MEMBER_RQ;
typedef struct STRU_ROOM_WOLF_MEMBER_RQ
{
    STRU_ROOM_WOLF_MEMBER_RQ()
    {
        m_nType= DEF_PACK_WOLF_ROOM_MEMBER;
        m_UserID =0;
        memset(m_szUser,0,MAX_SIZE);
    }
    PackType m_nType;   //包类型
    int m_UserID;
    char m_szUser[MAX_SIZE];
}STRU_ROOM_WOLF_MEMBER_RQ;

//离开房间请求
typedef struct STRU_LEAVEROOM_RQ
{
    STRU_LEAVEROOM_RQ()
    {
        m_nType = DEF_PACK_LEAVEROOM_RQ;
        m_nUserId = 0;
        m_RoomId = 0;
        memset(szUserName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int    m_nUserId; //用户ID
    int    m_RoomId;
    char   szUserName[MAX_SIZE];
}STRU_LEAVEROOM_RQ;
typedef struct STRU_LEAVEWOLFROOM_RQ
{
    STRU_LEAVEWOLFROOM_RQ()
    {
        m_nType = DEF_PACK_LEAVEWOLFROOM_RQ;
        m_nUserId = 0;
        m_RoomId = 0;
        memset(szUserName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int    m_nUserId; //用户ID
    int    m_RoomId;
    char   szUserName[MAX_SIZE];
}STRU_LEAVEWOLFROOM_RQ;

typedef struct UserInfo
{
    UserInfo()
    {
         m_sockfd = 0;
         m_id = 0;
         m_roomid = 0;
         memset(m_userName, 0 , MAX_SIZE);
         m_videofd = 0;
         m_audiofd = 0;
         m_wolfid=0;
         ready=0;
         myvec=0;
    }
  int m_sockfd;
    int  m_id;
    int  m_roomid;
    char m_userName[MAX_SIZE];
    int m_wolfid;
    int m_videofd;

    int m_audiofd;
    int myvec;
    int ready;

}UserInfo;


///音频数据帧
/// 成员描述
/// int type;
/// int userId;
/// int roomId;
/// int min;
/// int sec;
/// int msec;
/// QByteArray audioFrame;   -->char [] 柔性数组
///


///视频数据帧
/// 成员描述
/// int type;
/// int userId;
/// int roomId;
/// int min;
/// int sec;
/// int msec;
/// QByteArray videoFrame;
///
//注册音频
struct STRU_AUDIO_REGISTER
{
    STRU_AUDIO_REGISTER():m_nType(DEF_PACK_AUDIO_REGISTER){
        m_userid = 0;
    }

    PackType m_nType;   //包类型
    int m_userid;
};

//注册视频
struct STRU_VIDEO_REGISTER
{
    STRU_VIDEO_REGISTER():m_nType(DEF_PACK_VIDEO_REGISTER){
        m_userid = 0;
    }

    PackType m_nType;   //包类型
    int m_userid;
};


#endif



