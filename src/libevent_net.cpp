#include "libevent_net.h"

#include"packdef.h"


DealData_CallBack libevent_Net::m_dealData_callback = NULL ;

void libevent_Net::accept_cb(int fd, short events, void* arg)
{
    evutil_socket_t sockfd;

    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    sockfd = ::accept(fd, (struct sockaddr*)&client, &len );
    evutil_make_socket_nonblocking(sockfd);

    printf("accept a client %d\n", sockfd);

    libevent_Net* pthis = (libevent_Net*)arg;

    bufferevent* bev = bufferevent_socket_new( pthis->base, sockfd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, socket_read_cb, NULL, event_cb, arg);//因为有this指针 所以socket_read_cb必须是静态函数 而静态函数里边只能调用静态成员 所以回调函数也是静态成员

    bufferevent_enable(bev, EV_READ | EV_PERSIST);

    //创建对应的缓冲区 用于合包
    pthis->m_mapSockTobuff[ bev ] = new MyBuff;

}

#include<memory>

void libevent_Net::socket_read_cb(bufferevent* bev, void* arg)
{
    libevent_Net* pthis = (libevent_Net*)arg;
    //接收代码 每次接收最大4kb 大量数据需要合包

    if( pthis->m_mapSockTobuff.count(bev) == 0 ){
        printf("cant find sock\n");
        return;
    }
    int nPackSize = 0;

    MyBuff * mybuff = pthis->m_mapSockTobuff[bev];

    if( mybuff->nCur == 0 || mybuff->nPackSize == 0 )
    {
        size_t len = bufferevent_read(bev, &nPackSize , sizeof(int));
        if( len == 4 )
        {
            mybuff->nCur = 0;
            mybuff->nPackSize = nPackSize;
            mybuff->m_buff.resize( nPackSize );
        }
    }

    size_t len = bufferevent_read(bev, mybuff->GetPos() , mybuff->nPackSize - mybuff->nCur );

    if( len > 0 )
    {
        mybuff->nCur += len;

        if( mybuff->nCur == mybuff->nPackSize )
        {
            //处理业务
            lc *rc=new lc;
            rc->mybuff=mybuff;
            rc->bev=bev;
           // m_dealData_callback( bev , mybuff->Begin() , mybuff->nPackSize );
           pthis->m_threadpool->Producer_add( pthis->m_pool , Buffer_Deal , (void*) rc );
           // mybuff->nPackSize = 0;
           // mybuff->nCur = 0;
        }
    }
    //sleep(2);

//    msg[len] = '\0';
//    printf("recv the client msg: %s", msg);


//    char reply_msg[4096] = "I have recvieced the msg: ";

//    strcat(reply_msg + strlen(reply_msg), msg);
//     bufferevent_write(bev, reply_msg, strlen(reply_msg));
}
void * libevent_Net::Buffer_Deal( void * arg )
{
    lc * buffer = (lc *)arg;
    if( !buffer ) return NULL;

     m_dealData_callback(buffer->bev,buffer->mybuff->Begin(), buffer->mybuff->nPackSize);
     buffer->mybuff->nPackSize = 0;
     buffer->mybuff->nCur = 0;
 //   printf("pszbuf = %p \n",buffer->buf);
     delete  buffer;

    return 0;
}


void libevent_Net::event_cb(struct bufferevent *bev, short event, void *arg)
{
    libevent_Net* pthis = (libevent_Net*)arg;

    if (event & BEV_EVENT_EOF)
        printf("connection closed\n");
    else if (event & BEV_EVENT_ERROR)
        printf("some other error\n");

    //这将自动close套接字和free读写缓冲区
    //if(bev!=0)
    bufferevent_free(bev);
    //bev=0;
    //回收map对应的节点
    MyBuff* buff = NULL;
    if( pthis->m_mapSockTobuff.count( bev) > 0  )
        buff = pthis->m_mapSockTobuff[bev];

    pthis->m_mapSockTobuff.erase( bev );

    if( buff )
        delete buff;
}

//先发包大小 再发数据包
int libevent_Net::SendData(bufferevent *bufev, const void *szbuf, size_t nlen)
{

    int nPackSize = nlen + 4;
    std::vector<char> vecbuf( nPackSize , 0);
    //vecbuf.resize( nPackSize );

    char* buf = &* vecbuf.begin();
    char* tmp = buf;
    *(int*)tmp = nlen;//按四个字节int写入
    tmp += sizeof(int );
    memcpy( tmp , szbuf , nlen );

    return bufferevent_write( bufev, buf, nPackSize );

}


bool libevent_Net::InitServer(int port, int listen_num)
{
    int errno_save;
    m_pool = NULL;
    m_threadpool = new thread_pool;
    if((m_pool = (m_threadpool->Pool_create(200,10,50000))) == NULL)
    {
        perror("Create Thread_Pool Failed:");
        exit(-1);
    }
    evutil_socket_t listener = ::socket(AF_INET, SOCK_STREAM, 0);

    if( listener == -1 )
    {
        perror( "create socket error" );
        return false;
    }

    //允许多次绑定同一个地址。要用在socket和bind之间
    evutil_make_listen_socket_reuseable(listener);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);

    struct event* ev_listen;

    if( ::bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0 )
        goto error;

    if( ::listen(listener, listen_num) < 0)
        goto error;


    //跨平台统一接口，将套接字设置为非阻塞状态
    evutil_make_socket_nonblocking(listener);

    /* struct event_base*  */ base = event_base_new();

    //添加监听客户端请求连接事件
    /*struct event*  */ ev_listen = event_new(base, listener, EV_READ | EV_PERSIST,
                                        accept_cb, this);

    event_add(ev_listen, NULL);

    return true;

    error:
        errno_save = errno;
        evutil_closesocket(listener);
        errno = errno_save;
        perror(" tcp_server_init error ");
        return false;

}

//开启事件循环
void libevent_Net::ServerRunning()
{
    printf("server running\n");

    event_base_dispatch(base);
}

void libevent_Net::UnitServer()
{
    printf("UnitServer\n");
    event_base_free(base);
}
