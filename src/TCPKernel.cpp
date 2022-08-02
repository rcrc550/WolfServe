#include<TCPKernel.h>
#include "packdef.h"
#include<stdio.h>
#include<sys/time.h>

using namespace std;

TcpNet* TcpKernel::m_tcp2= NULL ;
#define NetPackMap(a)  TcpKernel::GetInstance()->m_NetPackMap[ a - DEF_PACK_BASE ]
//设置网络协议映射

void TcpKernel::setNetPackMap()
{
    //清空映射
    bzero( m_NetPackMap , sizeof(m_NetPackMap) );
    //协议映射赋值
    NetPackMap(DEF_PACK_REGISTER_RQ)    = &TcpKernel::RegisterRq;
    NetPackMap(DEF_PACK_LOGIN_RQ)       = &TcpKernel::LoginRq;
    NetPackMap(DEF_PACK_CREATEROOM_RQ)     = &TcpKernel::CreateRoomRq;
    NetPackMap(DEF_PACK_JOINROOM_RQ)       = &TcpKernel::JoinRoomRq;
    NetPackMap(DEF_PACK_LEAVEROOM_RQ)       = &TcpKernel::LeaveRoomRq;
    NetPackMap(DEF_PACK_AUDIO_FRAME)       = &TcpKernel::AudioFrameRq;
    NetPackMap(DEF_PACK_VIDEO_FRAME)       =&TcpKernel::VideoFrameRq;
    NetPackMap(DEF_PACK_VIDEO_REGISTER)    = &TcpKernel::VideoRegister;
    NetPackMap(DEF_PACK_AUDIO_REGISTER)    = &TcpKernel::AudioRegister;
    NetPackMap(DEF_PACK_JOINROOM_WOLF_RQ)  =&TcpKernel::WolfJoin;
    NetPackMap(DEF_PACK_CREATEROOM_WOLF_RQ)=&TcpKernel::WolfCreate;
    NetPackMap(DEF_PACK_LEAVEWOLFROOM_RQ)=&TcpKernel::WolfLeave;
    NetPackMap(DEF_PACK_LEAVEWOLFALIVE_RQ)=&TcpKernel::Dealalive;
    NetPackMap(DEF_PACK_READYWOLF_RQ)=&TcpKernel::ReadyWolf;
    NetPackMap(DEF_PACK_KILL_VOTE)=&TcpKernel::WolfVote;
    NetPackMap(DEF_PACK_SAVE_CHOOSE)=&TcpKernel::save;
    NetPackMap(DEF_PACK_WITCH_KILL)=&TcpKernel::dealpoison;
    NetPackMap(DEF_PACK_PROHIT_WATCH)=&TcpKernel::dealwatch;
    NetPackMap(DEF_PACK_VOTEWHO)=&TcpKernel::dealNorVote;
    NetPackMap(DEF_PACK_WOLF_MATCH)=&TcpKernel::dealMatch;
    NetPackMap(DEF_PACK_ADD_FRIEND)=&TcpKernel::dealAddfrined;
    NetPackMap(DEF_PACK_CHOOSE_INFOR)=&TcpKernel::dealinfor;
    NetPackMap(DEF_PACK_GetList)=&TcpKernel::dealgetlist;
    NetPackMap(DEF_PACK_GetMemberLike)=&TcpKernel::dealMemberLike;
    NetPackMap(DEF_PACK_SEEROOM)=&TcpKernel::dealMemberRoom;



}

TcpKernel::TcpKernel()
{
    pthread_mutex_init(&m_lock2,NULL);
    myinfor=new Myinfor;
    myu *arc=new myu;
    arc->my_infor=myinfor;
    c=&m_lock2;
    arc->arr=&c;
   // pthread_create(&alive_tid,NULL, alive,(void*)arc);
}

TcpKernel::~TcpKernel()
{
     if(myinfor)
         delete myinfor;
     myinfor=NULL;
     pthread_cancel(alive_tid);
     if(m_sql)
     {
         delete m_sql;
         m_sql=NULL;
     }

     if(m_redis)
     {
         delete m_redis;
         m_redis=NULL;
     }
}


TcpKernel *TcpKernel::GetInstance()
{
    static TcpKernel kernel;
    return &kernel;
}

int TcpKernel::Open( int port)
{
    initRand();
    setNetPackMap();
    m_sql = new CMysql;
    m_redis= new RedisTool;

    
    if(  !m_sql->ConnectMysql("localhost","root","123456","wechat")  )
    {
        printf("Conncet Mysql Failed...\n");
        return FALSE;
    }
    else
    {
        printf("MySql Connect Success...\n");
    }
    //初始网络

    m_tcp = new TcpNet(this);

    bool res = m_tcp->Initnetwork() ;
     if(res!=TRUE)
     {
         printf("false\n");
     }
    m_tcp2=m_tcp;
    if( !res )
        err_str( "net init fail:" ,-1);

    return TRUE;
}

void TcpKernel::Close()
{
    m_sql->DisConnect();

}

//随机数初始化
void TcpKernel::initRand()
{
    struct timeval time;
    gettimeofday( &time , NULL);
    srand( time.tv_sec + time.tv_usec );
}

void TcpKernel::DealData(int clientfd,char *szbuf,int nlen)
{
    PackType type = *(PackType*)szbuf;
    if( (type >= DEF_PACK_BASE) && ( type < DEF_PACK_BASE + DEF_PACK_COUNT) )
    {
        PFUN pf = NetPackMap( type );
        if( pf )
        {
            (TcpKernel::GetInstance()->*pf)( clientfd , szbuf , nlen);
        }
    }

    return;
}





void TcpKernel::sendData2(int clientfd , char *szbuf, int nlen)
{
    m_tcp2->SendData(clientfd , szbuf ,nlen );
}

void TcpKernel::SendData(int clientfd, char *szbuf, int nlen)
{
    m_tcp->SendData(clientfd,szbuf,nlen);
}

void TcpKernel::loop()
{
    m_tcp->Epoll_loop(NULL);
}


//注册
void TcpKernel::RegisterRq(int clientfd,char* szbuf,int nlen)//注册到来的回复 主要是看有没有重名重复id的
{
    printf("clientfd:%d RegisterRq\n", clientfd);
        //1.拆包
        STRU_REGISTER_RQ * rq = (STRU_REGISTER_RQ *)szbuf;
        STRU_REGISTER_RS rs;
        //获取tel password  name
        string c;
        string na=rq->m_name;
        string key="name";
        if(m_redis->isExist(rq->m_tel))
        {
                  rs.m_lResult = userid_is_exist;
                  m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
                  return ;
        }
        else if(m_redis->getHashVal(key,na,c))
        {
                  rs.m_lResult = name_is_exist;
                  m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
                  return ;
        }
        else
        {
        //查表 t_user 根据tel 查tel
        char sqlStr[1024]={0}/*""*/;
        sprintf( sqlStr , "select tel from t_user where tel = '%s';",rq->m_tel );
        list<string> resList;
        if( !m_sql->SelectMysql(sqlStr , 1, resList ))
        {
              printf("select wrong\n");
              return ;
        }


        //有 user存在 返回
        if( resList.size() > 0){
            rs.m_lResult = userid_is_exist;
        }else
        {
        //没有 查表 t_user 根据name myevent_s查name  name有没有
            char sqlStr[1024]={0}/*""*/;
            resList.clear();
            sprintf( sqlStr , "select name from t_user where name = '%s';",rq->m_name );

            if( !m_sql->SelectMysql(sqlStr , 1, resList ))
            {
                printf("SelectMysql error: %s \n", sqlStr);
                return;
            }
            if( resList.size() > 0 )
            {   //有 name存在 返回
                rs.m_lResult = name_is_exist;
            }else{
                //没有 写表 tel pass  name 头像和签名的默认值  返回注册成功
                rs.m_lResult = register_sucess;
                sprintf( sqlStr , "insert into t_user( tel, password , name , icon , feeling) values('%s','%s','%s',%d ,'%s');"
                         ,rq->m_tel,rq->m_szPassword , rq->m_name , 1 , "比较懒,什么也没写" );
                string tel=rq->m_tel;
                 m_redis->setHashVal("everyone",tel,"1");
                 string pssword=rq->m_szPassword;
                 string name=rq->m_name;
                 vector<string>myve;
                 string c="password";
                 string k="name";
                 myve.push_back(c);
                 myve.push_back(k);
                 vector<string>myvec;
                 myvec.push_back(pssword);
                 myvec.push_back(name);
                 m_redis->setHashVals(tel,myve,myvec);
                 m_redis->setHashVal("name",name,"1");
                if( !m_sql->UpdataMysql(sqlStr ))
                {
                    printf("UpdataMysql error: %s \n", sqlStr);
                }
            }
        }
        m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
        }


}

//创建房间
void TcpKernel::CreateRoomRq(int clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d CreateRoomRq\n", clientfd);
    //拆包
     STRU_CREATEROOM_RQ *createrq=(STRU_CREATEROOM_RQ *)szbuf;
     STRU_CREATEROOM_RS creaters;
    int roomid=0;
    //房间号1-8位 随机数

   UserInfo *infor=myinfor->m_mapIdToUserInfo.find(createrq->m_UserID);
   if(infor->m_roomid!=0)
   {
       creaters.m_lResult=room_is_exist;

   }
   else
   {
       do {
           roomid=rand()%99999999+1;
               //随机数 得到房间号 , 看有没有房间号 , 可能循环随机  map  roomid -> list

        } while (m_mapRoomidToMember.IsExist(roomid));
    list<int> th;
    th.push_back(createrq->m_UserID);
    m_mapRoomidToMember.insert(roomid,th);  //把房間號和人员插入进去
    infor->m_roomid=roomid;   //在刚开始的人员表里加上这个信息
    printf("room %d create successfully,frist member is %d/n",roomid,createrq->m_UserID);

    creaters.m_lResult=1;
    creaters.m_RoomId=roomid;
   }
     SendData(clientfd,(char*)&creaters,sizeof (creaters));

    //回复
}
//加入房间
void TcpKernel::JoinRoomRq(int clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d JoinRoomRq\n", clientfd);
    //拆包
     STRU_JOINROOM_RQ *joinrq=(STRU_JOINROOM_RQ *)szbuf;
     STRU_JOINROOM_RS joinrs;
    //查看房间是否存在
    if(!m_mapRoomidToMember.IsExist(joinrq->m_RoomID))
    {
         //不存在 返回失败
        joinrs.m_lResult=0;

       SendData(clientfd,(char*)&joinrs,sizeof (joinrs));
        return;


    }
    joinrs.m_RoomID=joinrq->m_RoomID;



    //存在返回成功
     joinrs.m_lResult=1;

     m_tcp->SendData(clientfd,(char*)&joinrs,sizeof (joinrs));

    //根据房间号拿到房间成员列表
     if(!myinfor->m_mapIdToUserInfo.IsExist(joinrq->m_UserID))return;
     STRU_ROOM_MEMBER_RQ myrm;
     myrm.m_UserID=joinrq->m_UserID;
     UserInfo *myinf=myinfor->m_mapIdToUserInfo.find(joinrq->m_UserID);
     myinf->m_roomid=joinrq->m_RoomID;
    strcpy(myrm.m_szUser,myinf->m_userName);
    //自己给自己发一个包用来提醒
     m_tcp->SendData(myinf->m_sockfd,(char*)&myrm,sizeof(myrm));

     list<int>lstRoommember=m_mapRoomidToMember.find(joinrq->m_RoomID);
    //遍历列表  -- 互换信息
     for(auto ite=lstRoommember.begin();ite!=lstRoommember.end();++ite)
     {
         //把加入人的信息发给每一个房间内成员
          int memid=*ite;
          if(!myinfor->m_mapIdToUserInfo.IsExist(memid))continue;
          STRU_ROOM_MEMBER_RQ mem;

          UserInfo *meminfo=myinfor->m_mapIdToUserInfo.find(memid);
          strcpy(mem.m_szUser,meminfo->m_userName);
          mem.m_UserID=meminfo->m_id;

                 //房间内成员每个人信息发给加入人
        SendData(clientfd,(char*)&mem,sizeof(mem));
          SendData(meminfo->m_sockfd,(char*)&myrm,sizeof(myrm));

     }



    //加入人 添加到房间列表
     lstRoommember.push_back(joinrq->m_UserID);
     m_mapRoomidToMember.insert(joinrq->m_RoomID,lstRoommember);

}

//离开房间
void TcpKernel::LeaveRoomRq(int clientfd, char *szbuf, int nlen)
{


    //拆包
    STRU_LEAVEROOM_RQ *rq=(STRU_LEAVEROOM_RQ*)szbuf;
    //看房间是否存在
    if(!myinfor->m_mapIdToUserInfo.IsExist(rq->m_nUserId))
       {
            printf("leave no this person\n");
            return;
      }
    UserInfo * r=myinfor->m_mapIdToUserInfo.find(rq->m_nUserId);
    r->m_roomid=0;
    if(!m_mapRoomidToMember.IsExist(rq->m_RoomId))
    {
        return ;
    }
    //如果房间存在，就可以获得用户列表。
    printf("clientfd:%d LeaveRoomRq:%d\n", clientfd,rq->m_RoomId);
    list<int>th=m_mapRoomidToMember.find(rq->m_RoomId);
    //遍历每个用户，用户是否在线，在线转发
    for(auto ite=th.begin();ite!=th.end();)
    {
        int id=*ite;
        //是不是自己，如果是自己从列表中去除
        if(id==rq->m_nUserId)
        {
            ite=th.erase(ite);
        }
        else
        {
            //遍历每个用户，用户是否在线
            if(myinfor->m_mapIdToUserInfo.IsExist(id))
            {
                UserInfo *h=myinfor->m_mapIdToUserInfo.find(id);
                SendData(h->m_sockfd,szbuf,nlen);
            }

                ++ite;

        }
    }
    //列表是否节点数为空（count） map项就要去掉
    if(th.size()==0)
    {
        m_mapRoomidToMember.erase(rq->m_RoomId);
        return;
    }
    //更新房间成员列表
     m_mapRoomidToMember.insert(rq->m_RoomId,th);

}
void TcpKernel::WolfLeave(int clientfd, char *szbuf, int nlen)
{
     //printf("leave no this person\n");
    STRU_LEAVEWOLFROOM_RQ *rq=(STRU_LEAVEWOLFROOM_RQ*)szbuf;
    if(MapWolfPlayRoom.IsExist(rq->m_RoomId))
    {
     WolfPlayRoom *m_room=MapWolfPlayRoom.find(rq->m_RoomId);
     if(m_room->going==1)
     m_room->leave(rq->m_nUserId);
    }
    if(!myinfor->m_mapIdToUserInfo.IsExist(rq->m_nUserId))
       {
            printf("leave no this person\n");
            return;
      }
    UserInfo * r=myinfor->m_mapIdToUserInfo.find(rq->m_nUserId);
    r->m_wolfid=0;
    if(r->ready!=0)
    {
        MapWolfReady.ded(r->m_wolfid);
        r->ready=0;
    }
    if(!m_mapWOLfRoomidToMember.IsExist(rq->m_RoomId))
    {
        return ;
    }
    //如果房间存在，就可以获得用户列表。
    printf("clientfd:%d LeaveRoomRq:%d\n", clientfd,rq->m_RoomId);
    list<int>th=m_mapWOLfRoomidToMember.find(rq->m_RoomId);
    //遍历每个用户，用户是否在线，在线转发
    printf("leave no this person\n");


    for(auto ite=th.begin();ite!=th.end();)
    {
        int id=*ite;
        //是不是自己，如果是自己从列表中去除
        if(id==rq->m_nUserId)
        {
            ite=th.erase(ite);
            char ar[10];
             sprintf(ar,"%d",rq->m_nUserId);

             m_redis->setHashVal(ar,"room","0");
        }
        else
        {
            //遍历每个用户，用户是否在线
            if(myinfor->m_mapIdToUserInfo.IsExist(id))
            {
                UserInfo *h=myinfor->m_mapIdToUserInfo.find(id);
                SendData(h->m_sockfd,szbuf,nlen);
            }

            ++ite;

        }
    }
    if(th.size()==WOLf_NUM-1)
    {
        char ar[10];
        sprintf(ar,"%d",rq->m_RoomId);
        m_redis->setHashVal("room",ar,"1");//若房间尚未开始 则在redis中加入
    }
    //列表是否节点数为空（count） map项就要去掉

    if(th.size()==0)
    {
        m_mapWOLfRoomidToMember.erase(rq->m_RoomId);
        MapWolfNumber.erase(rq->m_RoomId);
        MapWolfReady.erase(rq->m_RoomId);
        MapWolfStart.erase(rq->m_RoomId);
        if(MapWolfPlayRoom.IsExist(rq->m_RoomId))
        {
             WolfPlayRoom *m_room=MapWolfPlayRoom.find(rq->m_RoomId);
             MapWolfPlayRoom.erase(rq->m_RoomId);
             delete  m_room;
             m_room=NULL;
        }
        char ar[10];
        sprintf(ar,"%d",rq->m_RoomId);
        m_redis->delHashField("room",ar);
        return;
    }
    //更新房间成员列表
    m_mapWOLfRoomidToMember.insert(rq->m_RoomId,th);
    MapWolfNumber.ded(rq->m_RoomId);

}

void TcpKernel::ReadyWolf(int clientfd, char *szbuf, int nlen)
{

    //printf("ready\n");
    STRU_WOLF_READY*rq=(STRU_WOLF_READY*)szbuf;
    printf("this is%d ready\n",rq->m_roomid);
    if(!m_mapWOLfRoomidToMember.IsExist(rq->m_roomid))
    {
        printf("no exis\n");
        return;

    }
    if(myinfor->m_mapIdToUserInfo.IsExist(rq->m_id))
    {
         // printf("read2\n");
       UserInfo *c=myinfor->m_mapIdToUserInfo.find(rq->m_id);
       if(c->m_wolfid==rq->m_roomid&&c->ready==0)
       {
           MapWolfReady.add(rq->m_roomid);
            c->ready=1;
               printf("read2\n");
           if(MapWolfReady.find(rq->m_roomid)==WOLf_NUM)
           {
               MapWolfStart.add(rq->m_roomid);
               WolfPlayRoom *m_room;
                 if(!MapWolfPlayRoom.IsExist(rq->m_roomid))
                 {
                  m_room=new WolfPlayRoom(&TcpKernel::sendData2);
                  m_room->roomid=rq->m_roomid;
                   MapWolfPlayRoom.insert(rq->m_roomid,m_room);
                 }
                  else//分配身份 开始游戏             //在游戏结束后判断该房间人数是否为10 小于则在redis中加入该房间
                   m_room=MapWolfPlayRoom.find(rq->m_roomid);
                  list<int>ar=m_mapWOLfRoomidToMember.find(rq->m_roomid);

                 for(auto ite=ar.begin();ite!=ar.end();ite++)
                 {
                      UserInfo*user=myinfor->m_mapIdToUserInfo.find(*ite);
                      m_room->memberin(user);
                 }

           }

       }
    }
}

void TcpKernel::WolfVote(int clientfd, char *szbuf, int nlen)
{
    STRU_KILL_VOTE *rq=(STRU_KILL_VOTE*)szbuf;
    if(!MapWolfPlayRoom.IsExist(rq->m_roomid))
        return;
     WolfPlayRoom *m_room=MapWolfPlayRoom.find(rq->m_roomid);
     printf("wolfvote\n");
     m_room->wolf_kill(rq->m_id);
}

void TcpKernel::save(int clientfd, char *szbuf, int nlen)
{
    STRU_SAVE_CHOOSE *rq= (STRU_SAVE_CHOOSE *)szbuf;
    if(!MapWolfPlayRoom.IsExist(rq->m_roomid))
            return ;
    WolfPlayRoom*room=MapWolfPlayRoom.find(rq->m_roomid);
    if(rq->result==1)
    {
        room->diewho=-1;
        room->save=0;
        room->ref();

    }
    else
        room->die();

}

void TcpKernel::dealpoison(int clientfd, char *szbuf, int nlen)
{
      STRU_WITCH_KILL*rq=(STRU_WITCH_KILL*)szbuf;
      if(!MapWolfPlayRoom.IsExist(rq->m_roomid))
          return;
       WolfPlayRoom *m_room=MapWolfPlayRoom.find(rq->m_roomid);
       m_room->witchposion(rq->m_id);
}

void TcpKernel::dealwatch(int clientfd, char *szbuf, int nlen)
{
    STRU_PROHIT_WATCH*rq=(STRU_PROHIT_WATCH*)szbuf;
    if(!MapWolfPlayRoom.IsExist(rq->m_roomid))
        return;
     WolfPlayRoom *m_room=MapWolfPlayRoom.find(rq->m_roomid);
     int i=m_room->dealwatch(rq->m_id);
      STRU_DEAL_WATCH rqq;
      rqq.m_roomid=rq->m_roomid;
      rqq.m_result= i;
      SendData(clientfd,(char*)&rqq,sizeof (rqq));
}

void TcpKernel::dealNorVote(int clientfd, char *szbuf, int nlen)
{
    STRU_VOTE_WHO *rq=(STRU_VOTE_WHO*)szbuf;
    if(!MapWolfPlayRoom.IsExist(rq->m_roomid))
        return;
     WolfPlayRoom *m_room=MapWolfPlayRoom.find(rq->m_roomid);
     m_room->getmember();
     m_room->vote(rq->m_id);
}

void TcpKernel::dealAddfrined(int clientfd, char *szbuf, int nlen)
{
    STRU_ADD_FRIEND *rq=(STRU_ADD_FRIEND*)szbuf;
    STRU_ADD_RESULT rs;
    char arr[20];
    char arr2[30];
    char arr3[30];
    sprintf(arr2,"member%d",rq->m_id);
     sprintf(arr3,"%d",rq->m_bid);
    sprintf(arr,"%d",rq->m_bid);
    if(!m_redis->isExist(arr))
    {
              rs.m_result = -1;
              m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
              return ;
    }
    string c;
    if(m_redis->getHashVal(arr2,arr3,c))
    {
        rs.m_result = -2;
        m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
        return ;
    }
    char sqlStr[1024];
    sprintf( sqlStr , "insert into t_friend( idA, idB) values(%d,%d);"
             ,rq->m_id,rq->m_bid);
    if( !m_sql->UpdataMysql(sqlStr ))
    {
        printf("UpdataMysql error: %s \n", sqlStr);
        return;
    }
    sprintf( sqlStr , "insert into t_friend( idA, idB) values(%d,%d);"
             ,rq->m_bid,rq->m_id);
    if( !m_sql->UpdataMysql(sqlStr ))
    {
        printf("UpdataMysql error: %s \n", sqlStr);
        return;
    }
    char ack[30];
    char apl[30];
    sprintf(ack,"member%d",rq->m_id);
     sprintf(apl,"%d",rq->m_bid);
    m_redis->setHashVal(ack,apl,"1");
    sprintf(ack,"member%d",rq->m_bid);
      sprintf(apl,"%d",rq->m_id);
       m_redis->setHashVal(ack,apl,"1");

    //printf("gei in\n");
    sprintf(ack,"friend%d",rq->m_id);
      sprintf(apl,"%d",rq->m_bid);
    m_redis->settheSet(ack,apl);
     sprintf(ack,"friend%d",rq->m_bid);
        sprintf(apl,"%d",rq->m_id);
         m_redis->settheSet(ack,apl);
      rs.m_result=1;
     m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );


}

void TcpKernel::dealMatch(int clientfd, char *szbuf, int nlen)
{
    STRU_MATCH *joinrq=(STRU_MATCH*)szbuf;
    STRU_JOINROOM_WOLF_RS joinrs;

    vector<string>cc;
    m_redis->hkeys("room",cc);
    if(cc.size()==0)
    {
        joinrs.m_lResult=-2;

       SendData(clientfd,(char*)&joinrs,sizeof (joinrs));
        return;
    }
    string s=cc.front();
    int m_RoomID=atoi(s.c_str());

    if(MapWolfNumber.get(m_RoomID)==WOLf_NUM)
    {
        joinrs.m_lResult=-2;

       SendData(clientfd,(char*)&joinrs,sizeof (joinrs));
        return;
    }
    joinrs.m_RoomID=m_RoomID;


    //存在返回成功
     joinrs.m_lResult=1;
     m_tcp->SendData(clientfd,(char*)&joinrs,sizeof (joinrs));
     MapWolfNumber.add(m_RoomID);
     //若房间数量为10 则在redis中删除该房见
     int rck=MapWolfNumber.find(m_RoomID);
     if(rck==WOLf_NUM)
     {
         char arr[10];
         sprintf(arr,"%d",m_RoomID);
         string k=arr;
         m_redis->delHashField("room",k);
     }


    //根据房间号拿到房间成员列表
     if(!myinfor->m_mapIdToUserInfo.IsExist(joinrq->m_id))return;
     STRU_ROOM_WOLF_MEMBER_RQ myrm;
     myrm.m_UserID=joinrq->m_id;
     UserInfo *myinf=myinfor->m_mapIdToUserInfo.find(joinrq->m_id);
     myinf->m_wolfid=m_RoomID;
    strcpy(myrm.m_szUser,myinf->m_userName);
    //自己给自己发一个包用来提醒
     m_tcp->SendData(myinf->m_sockfd,(char*)&myrm,sizeof(myrm));


     list<int>lstRoommember=m_mapWOLfRoomidToMember.find(m_RoomID);
    //遍历列表  -- 互换信息
     for(auto ite=lstRoommember.begin();ite!=lstRoommember.end();++ite)
     {
         //把加入人的信息发给每一个房间内成员
          int memid=*ite;
          if(!myinfor->m_mapIdToUserInfo.IsExist(memid))continue;
          STRU_ROOM_WOLF_MEMBER_RQ mem;

          UserInfo *meminfo=myinfor->m_mapIdToUserInfo.find(memid);
          strcpy(mem.m_szUser,meminfo->m_userName);
          mem.m_UserID=meminfo->m_id;

                 //房间内成员每个人信息发给加入人
        SendData(clientfd,(char*)&mem,sizeof(mem));
          SendData(meminfo->m_sockfd,(char*)&myrm,sizeof(myrm));

     }
//加入人 添加到房间列表
     lstRoommember.push_back(joinrq->m_id);
     m_mapWOLfRoomidToMember.insert(m_RoomID,lstRoommember);
}

void TcpKernel::dealinfor(int clientfd, char *szbuf, int nlen)
{
    STRU_CHOOSE_ADDRESS *rq=(STRU_CHOOSE_ADDRESS*)szbuf;
    int id=rq->m_id;
    char ar[50];
    sprintf(ar,"%dinfor",rq->m_id);
    m_redis->settheSet(ar,rq->m_address);
}

void TcpKernel::dealgetlist(int clientfd, char *szbuf, int nlen)
{
    STRU_GetList*rq=(STRU_GetList*)szbuf;
    STRU_GetListRs rs;
    char arr[50];
    sprintf(arr,"friend%d",rq->m_id);
    vector<string>myvec;
    m_redis->smember(arr,myvec);
    for(int i=0;i<myvec.size();i++)
    {
        rs.m_id[i]=atoi(myvec[i].c_str());
    }
    rs.size=myvec.size();
    SendData( clientfd , (char*)&rs , sizeof rs );
}

void TcpKernel::dealMemberLike(int clientfd, char *szbuf, int nlen)
{
    printf("member like\n");
    STRU_GetMemberLike*rq=(STRU_GetMemberLike*)szbuf;
    STRU_GetMembero rs;
    char arr[40];
    sprintf(arr,"friend%d",rq->m_id);
    char arr2[40];
    sprintf(arr2,"%dinfor",rq->m_id);
    vector<string>myvec;
    m_redis->hkeys("everyone",myvec);
    int cc=0;
    for(int i=0;i<myvec.size();i++)
    {

        string c="friend";
        string k="infor";
        string p=c+myvec[i];
        string p2=myvec[i]+k;
        if(m_redis->sinter(p,arr)>0)
        {
            rs.m_id[cc]=atoi(myvec[i].c_str());
            cc++;
             printf("now is here\n");

        }
        else if(m_redis->sinter(p2,arr2)>0)
        {
            rs.m_id[cc]=atoi(myvec[i].c_str());
            cc++;
        }


    }
       SendData( clientfd , (char*)&rs , sizeof rs );


}

void TcpKernel::dealMemberRoom(int clientfd, char *szbuf, int nlen)
{
    STRU_SEEROOM *rq=(STRU_SEEROOM*)szbuf;
    STRU_SEEROOMRS rs;
    int id=rq->m_id;
    char arr[30];
    sprintf(arr,"%d",rq->m_id);
    string k;
    if(m_redis->getHashVal(arr,"room",k))
    {
      rs.m_roomid=atoi(k.c_str());
    }


     SendData( clientfd , (char*)&rs , sizeof rs );
}


void *TcpKernel::alive(void *arg)//心跳
{
        myu *arc=(myu*)arg;
        Myinfor *myin=arc->my_infor;
        pthread_mutex_t *m_lock2= *arc->arr;
        while(1)
        {
            pthread_mutex_lock(m_lock2);
            int c=myin->myvec.size();
             pthread_mutex_unlock(m_lock2);

            for(int i=0;i<c;i++)
            {

                time_t c;
                time(&c);
                pthread_mutex_lock(m_lock2);
                if((c-myin->myvec[i].time)>4)
                {

                     int id=myin->myvec[i].id;
                    if(myin->m_mapIdToUserInfo.IsExist(myin->myvec[i].id))
                    {

                      UserInfo *user=myin->m_mapIdToUserInfo.find(id);
                      printf("fd  is timeout\n");
                      if(user->m_sockfd!=0)
                      {
                        //close(user->m_sockfd );
                        printf("sockt quit1\n");
                        user->m_sockfd=0;
                      }
                      if(user->m_audiofd!=0)
                      {
                           // close(user->m_audiofd );
                         printf("audio quit2\n");
                          user->m_audiofd=0;
                      }
                      if(user->m_videofd!=0)
                     {
                           // close(user->m_videofd );
                        printf("vidio quit3\n");
                        user->m_videofd=0;
                      }
                      myin->m_mapIdToUserInfo.erase(id);

                    }

                    myin->myvec.erase(myin->myvec.begin()+i);

                    for(int k=0;k<myin->myvec.size();k++)
                    {
                        int ac=myin->myvec[k].id;
                       UserInfo*user=myin->m_mapIdToUserInfo.find(ac);
                       user->myvec=k;
                    }


                }
                pthread_mutex_unlock(m_lock2);


            }

            usleep(1000000);
        }

        pthread_exit((void*)1);
}





void TcpKernel::AudioFrameRq(int clientfd, char *szbuf, int nlen)
{
    char *tmp=szbuf;
    tmp+=sizeof(int);
    //跳过type

    //跳过userid
    int id=*(int*)tmp;
     tmp+=sizeof(int);
    //获取roomid
     int roomid2=*(int *)tmp; //按照int取数据

     if(m_mapRoomidToMember.IsExist(roomid2))
     {
         list<int >th=m_mapRoomidToMember.find(roomid2);
         for(auto ite=th.begin();ite!=th.end();ite++)
         {
             int icd=*ite;
             if(icd!=id)
             {
             if(!myinfor->m_mapIdToUserInfo.IsExist(icd))continue;
             UserInfo *user=myinfor->m_mapIdToUserInfo.find(icd);

             SendData(user->m_sockfd,szbuf,nlen);
             }

         }

     }
     else if(m_mapWOLfRoomidToMember.IsExist(roomid2))
     {
          printf("this wolf room is%d\n",roomid2);
         list<int >th=m_mapWOLfRoomidToMember.find(roomid2);
         for(auto ite=th.begin();ite!=th.end();ite++)
         {
             int icd=*ite;
             if(icd!=id)
             {
             if(!myinfor->m_mapIdToUserInfo.IsExist(icd))continue;
             UserInfo *user=myinfor->m_mapIdToUserInfo.find(icd);

             SendData(user->m_sockfd,szbuf,nlen);
            }

         }
     }
     else
     {
          printf("this room is%d\n",roomid2);
         printf ("no find in audioframerq\n");
         if(m_mapWOLfRoomidToMember.IsExist(roomid2))
         {
              printf ("rqq\n");
         }
         return;
     }





}
#include<pthread.h>
#include<unistd.h>
void TcpKernel::Dealalive(int clientfd, char *szbuf, int nlen)
{
   // printf("come on\n");
    STRU_ALIVE *rq=(STRU_ALIVE*)szbuf;
    int id=rq->id;
    UserInfo *user=myinfor->m_mapIdToUserInfo.find(id);
    int iad=user->myvec;
    pthread_mutex_lock(&m_lock2);
    time(&myinfor->myvec[iad].time);
     pthread_mutex_unlock(&m_lock2);

}
//登录
void TcpKernel::LoginRq(int  clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d LoginRq\n", clientfd);
    STRU_LOGIN_RQ* loginrq= (STRU_LOGIN_RQ*)szbuf;
    printf("%s\n",loginrq->m_tel);
    if(m_redis->isExist(loginrq->m_tel))
    {
        string c;
        m_redis->getHashVal(loginrq->m_tel,"password",c);
        string k=loginrq->m_szPassword;
        if(k==c)
        {
               char tel[30];
               int wo=atoi(loginrq->m_tel);
               sprintf(tel,"%d",wo);
               m_redis->setHashVal("everyone",tel,"1");
             STRU_LOGIN_RS rs;
               rs.m_lResult=login_sucess;
               rs.m_userID=atoi(loginrq->m_tel);
               printf("%d\n",rs.m_userID);
               string cc;
                 m_redis->getHashVal(loginrq->m_tel,"name",cc);
                 int id=atoi(loginrq->m_tel);
                 if(myinfor->m_mapIdToUserInfo.IsExist(id))//用户存在
                 {
                      rs.m_lResult=10;//强制下线
                         UserInfo *Info=myinfor->m_mapIdToUserInfo.find(id);
                          SendData( Info->m_sockfd , (char*)&rs , sizeof rs );
                         Info->m_sockfd=clientfd;
                         return ;
                 }
                 strcpy(rs.m_szName,cc.c_str());
                 UserInfo *pInfo=new  UserInfo;
                 pInfo->m_id=id;
                 pInfo->m_sockfd=clientfd;
                 pInfo->m_roomid=0;
                 pthread_mutex_lock(&m_lock2);
                 pInfo->myvec=myinfor->myvec.size();
                  pthread_mutex_unlock(&m_lock2);
                 strcpy(pInfo->m_userName,cc.c_str());
                 Myark c;
                 c.id=pInfo->m_id;
                 time(&c.time);
                  pthread_mutex_lock(&m_lock2);
                 myinfor->myvec.push_back(c);
                 pthread_mutex_unlock(&m_lock2);
                 myinfor->m_mapIdToUserInfo.insert( pInfo->m_id , pInfo );

                 strcpy(rs.m_szName,pInfo->m_userName);

               printf("redis\n");
                SendData( clientfd , (char*)&rs , sizeof rs );
                printf("youyou\n");
                while(1)
                {

                }
                return ;
        }
        else
        {
            STRU_LOGIN_RS rs;
              rs.m_lResult=password_error;
              printf("redis\n");
               SendData( clientfd , (char*)&rs , sizeof rs );
               return ;
        }


    }
    char selbuf[1024]={0};
    sprintf(selbuf,"select password,tel,name from t_user where tel= '%s';",loginrq->m_tel);
    list<string>rsstr;
    if(!m_sql->SelectMysql(selbuf,3,rsstr))
    {
        printf("loginrs select name password wrong\n");

    }
     STRU_LOGIN_RS rs;
    if(rsstr.size()<=0)
    {
        rs.m_lResult=userid_no_exist;


    }
    else
    {

    if(strcmp(loginrq->m_szPassword,rsstr.front().c_str())!=0)
    {
        rs.m_lResult=password_error;
    }
    else
    {

         //printf("comeon\n");
        string ii="password";
        string k="name";
        vector<string>p;
        p.push_back(ii);
        p.push_back(k);
          vector<string>pp;
          pp.push_back(rsstr.front());


        rsstr.pop_front();
        rs.m_lResult=login_sucess;
        int id=atoi(rsstr.front().c_str());
        rs.m_userID=id;
        rsstr.pop_front();
        pp.push_back(rsstr.front());
        UserInfo *pInfo=new  UserInfo;
        pInfo->m_id=id;
        pInfo->m_sockfd=clientfd;
        pInfo->m_roomid=0;
        pthread_mutex_lock(&m_lock2);
        pInfo->myvec=myinfor->myvec.size();
         pthread_mutex_unlock(&m_lock2);
        strcpy(pInfo->m_userName,rsstr.front().c_str());
        if(myinfor->m_mapIdToUserInfo.IsExist(pInfo->m_id))//用户存在
        {
            //强制下线
        }


        Myark c;
        c.id=pInfo->m_id;
        time(&c.time);
         pthread_mutex_lock(&m_lock2);
        myinfor->myvec.push_back(c);
        pthread_mutex_unlock(&m_lock2);
        myinfor->m_mapIdToUserInfo.insert( pInfo->m_id , pInfo );

        strcpy(rs.m_szName,pInfo->m_userName);
          m_redis->setHashVals(loginrq->m_tel,p,pp);

    }


    }
    SendData( clientfd , (char*)&rs , sizeof rs );

}
void TcpKernel::VideoFrameRq(int  clientfd, char *szbuf, int nlen)
{
 //   printf("clientfd:%d VideoFrameRq\n", clientfd);

    //拆包
    char* tmp = szbuf;
    //跳过type
    tmp += sizeof(int);
    //读取 userid
    int uid = *(int*) tmp;
    //跳过 userid
    tmp += sizeof(int);
    //获取 roomid
    int roomid =  *(int*) tmp;  //按照int取数据


    //看房间是否存在
    if( !m_mapRoomidToMember.IsExist(roomid) ) return;
    list<int> lst = m_mapRoomidToMember.find( roomid );
    //获取成员列表
    for( auto ite = lst.begin() ; ite !=lst.end() ; ++ite )
    {
    //看是否在线 转发
        int userid = *ite;
        //屏蔽掉自己  不转发
        //printf("%d/n",roomid);
        if( uid == userid ) continue;
        if( !myinfor->m_mapIdToUserInfo.IsExist( userid  ) ) continue;
        UserInfo *userinfo = myinfor->m_mapIdToUserInfo.find(userid);
        //printf("%d/n2",roomid);
        //原样转发
        SendData( userinfo->m_sockfd , szbuf , nlen );
    }
}
//音频注册
void TcpKernel::AudioRegister(/*int clientfd*/int  clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d AudioRegister\n", clientfd);
    //拆包
    STRU_AUDIO_REGISTER *rq = (STRU_AUDIO_REGISTER *)szbuf;
    int userid = rq->m_userid;
    //根据 userid  找到节点 更新 fd
    if( myinfor->m_mapIdToUserInfo.IsExist(userid) )
    {
        UserInfo *  info = myinfor->m_mapIdToUserInfo.find(userid);
        info->m_audiofd = clientfd;
    }
    printf("p\n");
}
//视频注册
void TcpKernel::VideoRegister(/*int clientfd*/int  clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d VideoRegister\n", clientfd);
    //拆包
    STRU_VIDEO_REGISTER *rq = (STRU_VIDEO_REGISTER *)szbuf;
    int userid = rq->m_userid;
    //根据 userid  找到节点 更新 fd
    if( myinfor->m_mapIdToUserInfo.IsExist(userid) )
    {
        UserInfo *  info = myinfor->m_mapIdToUserInfo.find(userid);
        info->m_videofd = clientfd;
    }
}

void TcpKernel::WolfJoin(int clientfd, char *szbuf, int nlen)
{

    STRU_JOINROOM_WOLF_RQ *joinrq=(STRU_JOINROOM_WOLF_RQ*)szbuf;
    STRU_JOINROOM_WOLF_RS joinrs;

    printf("oh yeah\n");
    if(MapWolfPlayRoom.IsExist(joinrq->m_RoomID))
    {
        WolfPlayRoom*r=MapWolfPlayRoom.find(joinrq->m_RoomID);
        if(r->going!=0)
        {
             joinrs.m_lResult=-3;
             SendData(clientfd,(char*)&joinrs,sizeof (joinrs));
              return;
        }
    }
    if(!m_mapWOLfRoomidToMember.IsExist(joinrq->m_RoomID))
    {
         //不存在 返回失败
        joinrs.m_lResult=0;

       SendData(clientfd,(char*)&joinrs,sizeof (joinrs));
        return;


    }
    if(MapWolfNumber.get(joinrq->m_RoomID)==WOLf_NUM)
    {
        joinrs.m_lResult=-2;

       SendData(clientfd,(char*)&joinrs,sizeof (joinrs));
        return;
    }
    joinrs.m_RoomID=joinrq->m_RoomID;


    //存在返回成功
     joinrs.m_lResult=1;
     char arr[30];
     sprintf(arr,"%d",joinrq->m_RoomID);
     string ld=arr;
     m_redis->setHashVal("room",ld,"1");
     m_tcp->SendData(clientfd,(char*)&joinrs,sizeof (joinrs));
     MapWolfNumber.add(joinrq->m_RoomID);
     //若房间数量为10 则在redis中删除该房见
     int rck=MapWolfNumber.find(joinrq->m_RoomID);
     if(rck==WOLf_NUM)
     {
         char arr[10];
         sprintf(arr,"%d",joinrq->m_RoomID);
         string k=arr;
         m_redis->delHashField("room",k);
     }
    char ar[10];
     sprintf(ar,"%d",joinrq->m_UserID);
     char ack[10];
      sprintf(ack,"%d",joinrq->m_RoomID);
     m_redis->setHashVal(ar,"room",ack);

    //根据房间号拿到房间成员列表
     if(!myinfor->m_mapIdToUserInfo.IsExist(joinrq->m_UserID))return;
     STRU_ROOM_WOLF_MEMBER_RQ myrm;
     myrm.m_UserID=joinrq->m_UserID;
     UserInfo *myinf=myinfor->m_mapIdToUserInfo.find(joinrq->m_UserID);
     myinf->m_wolfid=joinrq->m_RoomID;
    strcpy(myrm.m_szUser,myinf->m_userName);
    //自己给自己发一个包用来提醒
     m_tcp->SendData(myinf->m_sockfd,(char*)&myrm,sizeof(myrm));


     list<int>lstRoommember=m_mapWOLfRoomidToMember.find(joinrq->m_RoomID);
    //遍历列表  -- 互换信息
     for(auto ite=lstRoommember.begin();ite!=lstRoommember.end();++ite)
     {
         //把加入人的信息发给每一个房间内成员
          int memid=*ite;
          if(!myinfor->m_mapIdToUserInfo.IsExist(memid))continue;
          STRU_ROOM_WOLF_MEMBER_RQ mem;

          UserInfo *meminfo=myinfor->m_mapIdToUserInfo.find(memid);
          strcpy(mem.m_szUser,meminfo->m_userName);
          mem.m_UserID=meminfo->m_id;

                 //房间内成员每个人信息发给加入人
        SendData(clientfd,(char*)&mem,sizeof(mem));
          SendData(meminfo->m_sockfd,(char*)&myrm,sizeof(myrm));

     }
//加入人 添加到房间列表
     lstRoommember.push_back(joinrq->m_UserID);
     m_mapWOLfRoomidToMember.insert(joinrq->m_RoomID,lstRoommember);
}

void TcpKernel::WolfCreate(int clientfd, char *szbuf, int nlen)
{
    printf("wolfroom create successfully,frist member is ");


    STRU_CRATEROOM_WOLF_RQ *rq=(STRU_CRATEROOM_WOLF_RQ*)szbuf;
    STRU_CREATEROOM_WOLF_RS rs;
     printf("wolfroom create successfully,frist member is %d\n",rq->m_UserID);


    printf("wolfroom %d create successfully,frist member is %d/n",1,rq->m_UserID);

    int roomid=0;
    //房间号1-8位 随机数

   UserInfo *infor=myinfor->m_mapIdToUserInfo.find(rq->m_UserID);
     printf("wolfroom create successfully,frist member is %d\n",infor->m_id);
   if(infor->m_wolfid!=0)
   {
       rs.m_lResult=room_is_exist;

   }
   else
   {
       do {
           roomid=rand()%99999999+1;
               //随机数 得到房间号 , 看有没有房间号 , 可能循环随机  map  roomid -> list

        } while (m_mapWOLfRoomidToMember.IsExist(roomid));
       printf("roomid%d\n",roomid);
    list<int> th;
    th.push_back(rq->m_UserID);
    m_mapWOLfRoomidToMember.insert(roomid,th);  //把房間號和人员插入进去
    MapWolfNumber.insert(roomid,1);
    MapWolfReady.insert(roomid,0);
    infor->m_wolfid=roomid;   //在刚开始的人员表里加上这个信息
    printf("wolfroom %d create successfully,frist member is %d/n",roomid,rq->m_UserID);
     MapWolfStart.insert(roomid,0);
    rs.m_lResult=12;
    rs.m_RoomID=roomid;
    STRU_ROOM_WOLF_MEMBER_RQ myrm;
    myrm.m_UserID=rq->m_UserID;
    strcpy(myrm.m_szUser,infor->m_userName);
    //自己给自己发一个包用来提醒
     m_tcp->SendData(infor->m_sockfd,(char*)&myrm,sizeof(myrm));
     char arr[10];
     sprintf(arr,"%d",roomid);
     string ld=arr;
     m_redis->setHashVal("room",ld,"1");
    //在redis中加入该房间
   }
     SendData(clientfd,(char*)&rs,sizeof (rs));
}



