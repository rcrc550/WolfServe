#ifndef LIBEVENT_NET_H
#define LIBEVENT_NET_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include<vector>
#include<map>
#include"Thread_pool.h"

class thread_pool;
class pool_t;

struct MyBuff
{
    MyBuff(){
        nPackSize = 0;
        nCur =0;
    }
    char* GetPos()
    {
        return  Begin()+ nCur;
    }
    char* Begin()
    {
        return &*m_buff.begin();
    }
    std::vector<char> m_buff;
    int nPackSize;
    int nCur;
};
struct lc
{
    bufferevent* bev;
    MyBuff *mybuff;
};

//class IKernel
//{
//public:
//    IKernel(){};
//    virtual ~IKernel(){}
//    virtual void DealData(struct bufferevent *bufev, char*szbuf, int nlen) = 0;
//};

typedef void (*DealData_CallBack)(struct bufferevent*, char*, int );

class libevent_Net
{
public:
    libevent_Net(DealData_CallBack  callback)
    {
        m_dealData_callback = callback;
    }

    bool InitServer( int port , int listen_num = 128 );

    void ServerRunning();

    void UnitServer();
  static void *Buffer_Deal(void *arg);
    static int SendData(struct bufferevent *bufev,
                                 const void *data, size_t size);

    static void accept_cb(int fd, short events, void* arg);

    static void socket_read_cb(struct bufferevent* bev, void* arg);

    static void event_cb(struct bufferevent *bev, short event, void *arg);

private:
    struct event_base* base;
//    IKernel * pKernel;
    static DealData_CallBack m_dealData_callback;

public:
    std::map<struct bufferevent * , MyBuff* > m_mapSockTobuff;
    thread_pool *m_threadpool;
    pool_t *m_pool;
};

#endif // LIBEVENT_NET_H
