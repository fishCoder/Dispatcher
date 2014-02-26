#ifndef _COMMON_NODE_
#define _COMMON_NODE_
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/pool/pool.hpp>

#include "MessageList.h"

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> shared_ptr_socket;



class NodeServer{
public:
    NodeServer(boost::asio::io_service &io_ser,MessageList &_msgLst);

    void preAccept();
    void acceptHandler(shared_ptr_socket psocket, boost::system::error_code ec);

    void async_read(int _h_socket);
    void read_handle(char * receive_pool_size,int _h_socket,boost::system::error_code ec,std::size_t length);

    void async_write(int _h_socket,std::string &sendstr);
    void write_handle(int _h_socket,boost::system::error_code ec,std::size_t length);

    shared_ptr_socket find_socket_by_id(int _h_socket);
    void delete_socket_by_id(int _h_socket);
private:
    boost::pool<> socket_pool;
    std::map<int,shared_ptr_socket> socket_map;
    boost::asio::io_service &io_service;
    MessageList &msgLst;
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> pacceptor;
};

#endif
