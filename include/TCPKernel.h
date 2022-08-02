#ifndef _TCPKERNEL_H
#define _TCPKERNEL_H


#include"threadpool.h"
#include "Mysql.h"
#include"tcpnet.h"
#include"pthread.h"
#include"wolfplayroom.h"

//类成员函数指针 , 用于定义协议映射表
class TcpKernel;
class WolfPlayRoom;
typedef void (TcpKernel::*PFUN)(int fd,char*,int nlen);
typedef  struct Myark
{
      int id;

      time_t time;
      Myark()
      {
          time=0;
          id=0;

      }

}Myark;
typedef struct Myinfor
{
     MyMap<int,UserInfo*>m_mapIdToUserInfo;
     vector<Myark>myvec;
}Myinfor;
typedef  struct myu
{
    Myinfor *my_infor;
    pthread_mutex_t **arr;
}myu;

class TcpKernel:public IKernel
{
public:
    //单例模式
    static TcpKernel* GetInstance();


    int Open(int port);
    //初始化随机数
    void initRand();
    //设置协议映射
    void setNetPackMap();
    //关闭核心服务
    void Close();
    //处理网络接收
     void DealData(int clientfd, char*szbuf, int nlen);

     static void sendData2(int clientfd, char*szbuf, int nlen);

    //发送数据
    void SendData( int clientfd, char*szbuf, int nlen );

    /************** 网络处理 *********************/
    void loop();
    //注册
    void RegisterRq(int clientfd, char*szbuf, int nlen);
    //登录
    void LoginRq(int clientfd, char*szbuf, int nlen);
    //创建房间
    void CreateRoomRq(int clientfd, char *szbuf, int nlen);
    //加入房间
    void JoinRoomRq(int clientfd, char *szbuf, int nlen);
    //离开房间
    void LeaveRoomRq(int clientfd, char *szbuf, int nlen);
    //处理音频帧
    void AudioFrameRq(int clientfd, char *szbuf, int nlen);
    void Dealalive(int clientfd, char *szbuf, int nlen);
    //处理视频帧
      void VideoFrameRq(int clientfd, char *szbuf, int nlen);
      void AudioRegister(int clientfd, char *szbuf, int nlen);
       void VideoRegister(int clientfd, char *szbuf, int nlen);
      void  WolfJoin(int clientfd, char *szbuf, int nlen);
      void WolfCreate(int clientfd, char *szbuf, int nlen);
      void WolfLeave(int clientfd, char *szbuf, int nlen);
      void ReadyWolf(int clientfd, char *szbuf, int nlen);
      void WolfVote(int clientfd, char *szbuf, int nlen);
      void save(int clientfd, char *szbuf, int nlen);
      void dealpoison(int clientfd, char *szbuf, int nlen);
      void dealwatch(int clientfd, char *szbuf, int nlen);
      void dealNorVote(int clientfd, char *szbuf, int nlen);
       void dealAddfrined(int clientfd, char *szbuf, int nlen);
       void dealMatch(int clientfd, char *szbuf, int nlen);
       void dealinfor(int clientfd, char *szbuf, int nlen);
         void dealgetlist(int clientfd, char *szbuf, int nlen);
           void   dealMemberLike(int clientfd, char *szbuf, int nlen);
              void     dealMemberRoom(int clientfd, char *szbuf, int nlen);


public:
       static void * alive(void* arg);


    /*******************************************/
private:
    TcpKernel();
    ~TcpKernel();
    //数据库
    CMysql * m_sql;
    //网络
    TcpNet* m_tcp;
     static TcpNet* m_tcp2;
    pthread_t alive_tid;
    //协议映射表
    PFUN m_NetPackMap[DEF_PACK_COUNT];
    Myinfor *myinfor;
    //需要建立一个map来映射每个id的socket，房间号，名字 来转发数据音频画面，判断一个房间内的转发，但是map不是线程安全的，我们自己用的时候需要加锁,避免移除元素的时候出现问题.
   // MyMap<int,UserInfo*>m_mapIdToUserInfo;
    MyMap<int,list<int>>m_mapRoomidToMember;
    MyMap<int,list<int>>m_mapWOLfRoomidToMember;
    MyMap<int,int>MapWolfNumber;
    MyMap<int,int>MapWolfReady;
   MyMap<int,int>MapWolfStart;
   MyMap<int,WolfPlayRoom*>MapWolfPlayRoom;
    pthread_mutex_t m_lock2;
     pthread_mutex_t *c;
public:
     RedisTool *m_redis;

};

#endif
