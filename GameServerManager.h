#ifndef _COMMON_GSM_
#define _COMMON_GSM_

#include <boost/asio.hpp>
//#include "GameServer.h"
#include "MapManager.h"

#include <boost/shared_ptr.hpp>
#include <boost/pool/pool.hpp>
#include <list>

class GameSever;
class MessageList;
class TaskList;
typedef boost::shared_ptr<GameSever> shared_gameserver;
class GameSeverManager{
public:
    GameSeverManager(boost::asio::io_service &io_ser,MessageList &_msgLst,TaskList & _taskLst);
    void acceptHandler(boost::shared_ptr<boost::asio::ip::tcp::socket> psocket , boost::system::error_code ec);
    void preAccept();
    bool findGameServer(int id,GameSever **gs);
    void sendMap(int id,int type,int sence);
    void delGameServer(int id);
protected:
private:
    MapManager mm;
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> pacceptor;
    std::list<shared_gameserver> GSLst;
    MessageList &msgLst;
};


#endif // _COMMON_GSM_
