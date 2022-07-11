#include "wolfplayroom.h"
DealData_CallBack WolfPlayRoom::m_dealData_callback = NULL ;


WolfPlayRoom::WolfPlayRoom(DealData_CallBack callback)
{
    nw=0;
    wolfkillvwkill=0;
    diewho=-1;
    for(int i=0;i<10;i++)
    {
        noramlvote.push_back(0);
    }
    for(int i=0;i<10;i++)
    {
        wolfkill.push_back(0);
    }
    dieready=new int;
    ro=new room;
    *dieready=0;
    going=0;
    arr=new person[10];
    if(m_dealData_callback==NULL)
    m_dealData_callback=callback;
    wolfnum=3;
    villnum=4;
    godnum=3;
    srand(time(0));
    numbersl=0;
    allvote=WOLf_NUM;
    wolfvote=1;
    save=1;
    kill=1;
    alive=WOLf_NUM;
    nowallmember=0;
}

WolfPlayRoom::~WolfPlayRoom()
{
    if(arr)
    {
        delete arr;
        arr=NULL;
    }

    if(dieready)
    {
        delete dieready;
        dieready=NULL;
    }
    if(ro)
    {
        delete  ro;
        ro=NULL;
    }
}

void WolfPlayRoom::memberin(UserInfo *k)
{
    arr[numbersl].m_id=k->m_id;
    arr[numbersl].fd=k->m_sockfd;
    arr[numbersl].alive=1;

    numbersl++;
    if(numbersl==WOLf_NUM)
    {

        going=1;
            int a;
           int c=3;//int c=3;
           while(c)
           {
               a=rand()%2;//int a=rand()%10;
             if(arr[a].status==0)
             {
                 arr[a].status=Wolf;
                 wolfwhre.push_back(arr[a].m_id);
                 c--;
             }
           }
//           int k=1;// k=4;
//           while(k)
//           {
//              a=rand()%10;
//             if(arr[a].status==0)
//             {
//                 arr[a].status=villager;
//                 k--;
//             }
//           }
           int p=1;
           while(p)
           {
              a=rand()%2;
             if(arr[a].status==0)
             {
                 arr[a].status=witch;
                  nw=a;
                 p--;
             }
           }
//           int pp=1;
//           while(pp)
//           {
//              a=rand()%10;
//             if(arr[a].status==0)
//             {
//                 arr[a].status=hunter;
//                 pp--;
//             }
//           }
//           int lk=1;
//           while(lk)
//           {
//               a=rand()%3;
//               if(arr[a].status==0)
//               {
//                   arr[a].status=prophet;
//                   lk--;
//               }
//           }

          startgame();
//    }

//}
    }
}
void WolfPlayRoom::gameover(int final)
{
    //向所有成员发游戏结束的信息
    for(int i=0;i<WOLf_NUM;i++)
    {
         STRU_GAMEOVER rq;
         rq.result=final;
         rq.m_roomid=roomid;
         m_dealData_callback(arr[i].fd, (char*)&rq,sizeof(rq));
    }
    nw=0;
    wolfkillvwkill=0;

    diewho=-1;
    numbersl=0;
    going=0;
    wolfnum=1;
    villnum=0;
    godnum=2;
    numbersl=0;
    allvote=WOLf_NUM;
    wolfvote=1;
    save=1;
    kill=1;
    alive=WOLf_NUM;
    nowallmember=0;
     *dieready=0;

}

void WolfPlayRoom::startgame()
{
    printf("start game\n");

    for(int g=0;g<WOLf_NUM;g++)
    {
        STRU_ROOM_START_RQ rq;
        rq.m_idtype=arr[g].status;
        if(arr[g].status==Wolf)
        {
            for(int oo=0;oo<wolfwhre.size();oo++)
            {
              rq.wolfmember[oo]=wolfwhre[oo];
            }
        }
          printf("start game2\n");
        m_dealData_callback(arr[g].fd, (char*)&rq,sizeof(rq));//发送身份
        //m_dealData_callback();
    }

    ro->roomid=roomid;
    ro->per=arr;
    ro->diealready=dieready;
    pthread_create(&tid,NULL,gamerun,(void*)ro);



}
void WolfPlayRoom::die()
{
      if(diewho==-1)
          return;

            int l=diewho;
            arr[l].alive=0;
            alive--;
            if(arr[l].status==Wolf)
            {
                wolfnum--;
            }
            else if(arr[l].status==villager)
            {
                villnum--;
            }
            else if(arr[l].status==witch||arr[l].status==hunter||arr[l].status==prophet)
            {
                godnum--;
            }
           // m_dealData_callback(arr[g].fd, (char*)&rq,sizeof(rq));告诉他你死了
            for(int i=0;i<WOLf_NUM;i++)//告诉死人是谁
            {

               STRU_DIE rq;
               rq.m_roomid=roomid;
               rq.m_id=arr[l].m_id;

                m_dealData_callback(arr[i].fd,(char*)&rq,sizeof(rq));
            }
            diewho=-1;
   if(going!=0)
   {
        if(wolfnum==0||villnum==0||godnum==0)
        {
            //取消线程

            pthread_cancel(tid);
            if(godnum==0||villnum==0)
            {
                 gameover(2);
            }
            else if(wolfnum==0)
            {
                 gameover(1);
            }
            //游戏结束 谁胜利了


        }
   }



    *dieready=1;

}

void WolfPlayRoom::wolf_kill(int id)
{


    for(int i=0;i<WOLf_NUM;i++)
    {
        if(arr[i].m_id==id)
        {
            wolfkill[i]++;
        }
    }
    wolfvote--;
    if(wolfvote==0)
    {
        int maxpos=0;
        int max=wolfkill[0];
        for(int i=1;i<WOLf_NUM;i++)
        {
              if(wolfkill[i]>max)
              {
                  max=wolfkill[i];
                  maxpos=i;
              }
        }
        if(wolfkill[maxpos]==0)
        {
            return ;
        }
        diewho=maxpos;
        if(save>0)
        {
        STRU_SWTICH_SAVE rq;
        rq.m_id=arr[diewho].m_id;
        rq.m_roomid=roomid;

         m_dealData_callback(arr[nw].fd,(char*)&rq,sizeof(rq));

        }
        else
        {
            die();
        }
           wolfvote=1;
    }
    for(int i=0;i<10;i++)
    {
        wolfkill[i]=0;
    }

}

void WolfPlayRoom::leave(int id)
{
    for(int l=0;l<WOLf_NUM;l++)
    {
     if(arr[l].m_id==id)
     {
      arr[l].alive=0;
      if(arr[l].status==Wolf)
      {
        wolfnum--;
      }
      else if(arr[l].status==villager)
      {
        villnum--;
      }
       else if(arr[l].status==witch||arr[l].status==hunter||arr[l].status==prophet)
      {
        godnum--;
      }

      alive--;
     }
    }
    if(wolfnum==0||villnum==0||godnum==0)
    {
        //取消xc
         pthread_cancel(tid);
        if(godnum==0||villnum==0)
        {
              gameover(2);
        }
        else if(wolfnum==0)
        {
             gameover(1);
        }
        //游戏结束 谁胜利了

    }
}

void *WolfPlayRoom::gamerun(void* arg)
{

     room *c= (room *)arg;
     person *arr=c->per;
     int cc=1;
     int *ready=c->diealready;
     while(1)
     {
         *ready=0;

          for(int i=0;i<WOLf_NUM;i++)//天黑了
          {

             STRU_DAY_DARK rq;
             rq.m_id=arr[i].m_id;
             rq.m_roomid=c->roomid;
             if(arr[i].alive!=0)
              m_dealData_callback(arr[i].fd,(char*)&rq,sizeof(rq));
          }
          cc=cc-1;
         sleep(5);
         for(int i=0;i<WOLf_NUM;i++)
         {
             if(arr[i].status==Wolf&&arr[i].alive>0)
             {
                     STRU_WOLF_KILL rq;
                     rq.m_id=arr[i].m_id;
                     rq.m_roomid=c->roomid;
                      m_dealData_callback(arr[i].fd,(char*)&rq,sizeof(rq));

             }



             //m_dealData_callback(arr[i].fd,(char*)&rq,sizeof(rq));
         }
         while(*ready!=1)
         {
             sleep(2);
         }
         *ready=0;
         pthread_testcancel();

            for(int i=0;i<WOLf_NUM;i++)                                 //发送天亮了
           {

                if(arr[i].alive!=0)
                {
                    STRU_WAKE rq;

                    rq.m_roomid=c->roomid;
                   m_dealData_callback(arr[i].fd,(char*)&rq,sizeof(rq));

                }
            }
            sleep(3);
            for(int i=0;i<WOLf_NUM;i++)                                 //发送天亮了
           {

                if(arr[i].alive!=0)
                {
                    STRU_SAY rq;
                    rq.id=arr[i].m_id;
                    rq.m_roomid=c->roomid;
                    for(int p=0;p<WOLf_NUM;p++)
                    {
                        if(arr[p].alive>0)
                       m_dealData_callback(arr[p].fd,(char*)&rq,sizeof(rq));
                    }
                   sleep(20);
                   STRU_STOPSAY rs;
                   rs.m_roomid=c->roomid;
                   m_dealData_callback(arr[i].fd,(char*)&rs,sizeof(rs));
                }
            }
            for(int i=0;i<WOLf_NUM;i++)
           {

                if(arr[i].alive!=0)
                {
                   STRU_NORVOTE rq;
                   rq.m_roomid=c->roomid;
                    m_dealData_callback(arr[i].fd,(char*)&rq,sizeof(rq));
                }
            }
             while(*ready!=1)
             {
                 sleep(1);
             }
            *ready=0;

          pthread_testcancel();
      }

}

void WolfPlayRoom::witchposion(int id)
{
    for(int i=0;i<WOLf_NUM;i++)
    {
        if(arr[i].m_id==id)
        {
               arr[i].alive=0;
               STRU_DIE rq;
               rq.m_roomid=roomid;
               rq.m_id=arr[i].m_id;
               if(arr[i].status==Wolf)
               {
                   wolfnum--;

               }
               else if(arr[i].status==villager)
               {
                   villnum--;
               }
               else if(arr[i].status==witch||arr[i].status==hunter||arr[i].status==prophet)
               {
                   godnum--;
               }
                alive--;
                for(int i=0;i<WOLf_NUM;i++)
                {

                   m_dealData_callback(arr[i].fd,(char*)&rq,sizeof(rq));
                }


        }



        //m_dealData_callback(arr[i].fd,(char*)&rq,sizeof(rq));
    }


    if(going!=0)
    {
    if(wolfnum==0||villnum==0||godnum==0)
    {
        //取消线程
        //pthread_kill(tid , 9);
        pthread_cancel(tid);
        if(godnum==0||villnum==0)
        {
              gameover(2);
        }
        else if(wolfnum==0)
        {
              gameover(1);
        }
        //游戏结束 谁胜利了

    }
    }


}

void WolfPlayRoom::ref()
{

    *dieready=1;
}

int WolfPlayRoom::dealwatch(int id)
{
    for(int i=0;i<WOLf_NUM;i++)
    {
        if(arr[i].m_id==id)
        {
            STRU_DEAL_WATCH rq;
            rq.m_roomid=roomid;
            if(arr[i].status==Wolf)
            {
                return 1;
            }
            else
            {
                return 2;
            }


        }




    }
    return 0;
}

void WolfPlayRoom::vote(int id)
{

    for(int i=0;i<WOLf_NUM;i++)
    {
        if(arr[i].m_id==id)
        {
            noramlvote[i]++;
        }
    }
    nowallmember--;
    if(nowallmember==0)
    {
        int maxpos=0;
        int max;
         int max2;
        if(noramlvote[0]>=noramlvote[1])
        {
          max=noramlvote[0];
          max2=noramlvote[1];
        }
        else if(noramlvote[0]<noramlvote[1])
        {
          max=noramlvote[1];
          max2=noramlvote[0];
        }
        for(int i=1;i<WOLf_NUM;i++)
        {
              if(noramlvote[i]>=max)
              {
                  max=noramlvote[i];
                  max2=max;
                  maxpos=i;
              }
        }
        if(wolfkill[maxpos]==0||max==max2)
        {
            printf("go back\n");
            *dieready=1;

            return ;
        }
        diewho=maxpos;

        for(int i=0;i<10;i++)
        {
            noramlvote[i]=0;
        }
      die();
     *dieready=1;


    }
}

void WolfPlayRoom::getmember()
{
    if(nowallmember==0)
     nowallmember=wolfnum+villnum+godnum;
}

