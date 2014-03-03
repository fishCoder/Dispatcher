#ifndef _COMMON_GENERATOR_
#define _COMMON_GENERATOR_


#ifndef _BOOST_ASIO_
#define _BOOST_ASIO_

#include <boost/asio.hpp>

#endif // _BOOST_ASIO_

#include <boost/bind.hpp>
#include <boost/asio/yield.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <deque>

typedef boost::asio::ip::tcp tcp;
class Generator{
public:
    Generator(boost::shared_ptr<tcp::socket> psocket);
    void setState(int state);
    bool isfree();
    bool cmpID(int id);
    boost::asio::ip::tcp::socket & getSocket();
    void clearbuf();
    int getID();
    char readbuf[1024];
    std::string current_task;
protected:
private:
    boost::shared_ptr<tcp::socket> psocket;
    int state;
    int id;
};

#endif // _COMMON_GENERATOR_
