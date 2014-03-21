#ifndef _COMMON_GENERATOR_SERVER_
#define _COMMON_GENERATOR_SERVER_

#ifndef _BOOST_ASIO_
#define _BOOST_ASIO_

//#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#endif // _BOOST_ASIO_

#include <boost/shared_ptr.hpp>
#include <list>

#include "Receiver.h"

class MessageList;
class GeneratorServer{
public:
    GeneratorServer(boost::asio::io_service &io_service,MessageList & msgLst);

    int getFreeGenrator();
    void sendTask(int id,std::string task);
    void setState(int id,int state);
    void acceptHandler(boost::shared_ptr<boost::asio::ip::tcp::socket> psocket , boost::system::error_code ec);
    void preAccept();
    std::string get_address(int id);
protected:
private:
    typedef boost::asio::ip::tcp tcp;
    boost::shared_ptr<tcp::acceptor> pacceptor;

    Receiver receiver;
};

#endif // _COMMON_GENERATOR_SERVER_
