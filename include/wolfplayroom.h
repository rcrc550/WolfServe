#ifndef WOLFPLAYROOM_H
#define WOLFPLAYROOM_H
#include<vector>
//#include <QObject>
#include"packdef.h"
#include"TCPKernel.h"
#include"pthread.h"
#include"tcpnet.h"

enum Status { Wolf=1, prophet=2, villager=3, witch=4, hunter=5 };
typedef struct person
{

    person()
    {
        alive=1;
        m_id=0;
        fd=0;
        status=0;
    }
    int alive;
    int fd;
    int m_id;
    int status;

}person;
typedef struct room
{

    room()
    {
       per=NULL;
       roomid=0;
       diealready=NULL;
    }
    person *per;
    int roomid;
    int *diealready;


}room;
class WolfPlayRoom
{
public:
    WolfPlayRoom(DealData_CallBack c);
    ~WolfPlayRoom();
    void memberin(UserInfo *k);
    void gameover(int final);
    void startgame();
    void die();
    void wolf_kill(int id);
    void leave(int id);
    static void *gamerun(void* arg);
    void witchposion(int id);
    void ref();
    int dealwatch(int id);
    void vote(int id);
     void getmember();
public:
   person *arr;
    int numbersl;//加入进来arr中的成员数量
     int going;
     int roomid;
    int wolfnum;
    int villnum;
    int nowallmember;
    int alive;
    room *ro;
    int godnum;
    vector<int>wolfwhre;
    vector<int>noramlvote;
    vector<int>wolfkill;
    pthread_t tid;
    int *dieready;
    static DealData_CallBack m_dealData_callback;
    int wolfvote;
    int allvote;
    int diewho;
    int nw;
    int save;
    int kill;
    int wolfkillvwkill;
};

#endif // WOLFPLAYROOM_H
