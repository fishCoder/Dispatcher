#ifndef _COMMON_RECEIVER_
#define _COMMON_RECEIVER_

#ifndef _BOOST_ASIO_
#define _BOOST_ASIO_

#include <boost/asio.hpp>

#endif // _BOOST_ASIO_

#include <iostream>
#include <list>

class Generator;

class MessageList;

typedef boost::shared_ptr<Generator> share_generator;
class Receiver{
public:
    typedef boost::asio::ip::tcp tcp;

    Receiver(MessageList & msgLst);

    void pusk_msg(std::string msg);
    void regReader(boost::shared_ptr<Generator> pGenerator);
    void readHanler(boost::shared_ptr<Generator>,boost::system::error_code ec,size_t len);

    std::list<Generator> getListGenerator();
    int getFreeGenrator();
    void sendTask(int id,std::string task);
    void setState(int id,int state);
    boost::shared_ptr<Generator> findGenerator(int id);
    void sendHandler(int id , boost::system::error_code ec , size_t len);
    void delGenerator(int id);
    void addGenerator(boost::shared_ptr<boost::asio::ip::tcp::socket> psocket);

    void showLstGenerator();

protected:
private:
    MessageList & msgLst;
    std::list<share_generator> lstGenerator;
};

#endif // _COMMON_RECEIVER_
