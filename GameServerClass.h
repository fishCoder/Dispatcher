#ifndef _COMMON_GAME_SERVER_CLASS_
#define _COMMON_GAME_SERVER_CLASS_

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/pool/pool.hpp>

#include "MessageList.h"

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> shared_ptr_socket;
class MapManager;
class GameSever{
public:
    GameSever(boost::asio::io_service &io_ser,MessageList &_msgLst);

    void preAccept();
    void acceptHandler(shared_ptr_socket psocket, boost::system::error_code ec);

    void async_read(int _h_socket);
    void read_handle(void * packege_head,int _h_socket,boost::system::error_code ec,std::size_t length);
    void read_handler(boost::system::error_code ec,std::size_t length);

    void async_write(int _h_socket,void * data,int data_size);
    void write_handle(int _h_socket,boost::system::error_code ec,std::size_t length);

    void send_field_map(int _h_socket,int map_type_id ,int sence_id,MapManager &map_manager);

    shared_ptr_socket find_socket_by_id(int _h_socket);
    void delete_socket_by_id(int _h_socket);
private:
    boost::pool<> socket_pool;
    std::map<int,shared_ptr_socket> socket_map;
    boost::asio::io_service &io_service;
    MessageList &msgLst;
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> pacceptor;
};

#endif // _COMMON_GAME_SERVER_CLASS_
