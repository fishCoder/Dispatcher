#ifndef _COMMON_GS_
#define _COMMON_GS_

#ifndef _BOOST_ASIO_
#define _BOOST_ASIO_

//#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/pool/pool.hpp>

#endif // _BOOST_ASIO_

#include <boost/shared_ptr.hpp>
#include <list>
#include "Packege.h"
#include "MapManager.h"
//class MapManager;
class MessageList;

typedef boost::asio::ip::tcp tcp;
class GameSever{
public:
    GameSever(boost::shared_ptr<tcp::socket> _psocket,MessageList &_msgLst,MapManager &_mm);
    void async_read();
    void read_handle(void * _bp,boost::system::error_code ec,std::size_t length);
    void async_write(std::string &sendstr);
    void write_handle(boost::system::error_code ec,std::size_t length);
    int getID();
protected:
private:

    int id;
    boost::shared_ptr<tcp::socket> psocket;
    boost::pool<> cpool;
    char * readbuf;
    MessageList &msgLst;
    MapManager &mm;
};

#endif
