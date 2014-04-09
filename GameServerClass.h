//
//主要管理全部的游戏服务器通信
//
//当游戏服务器请求连接时，该类根据当时微妙数生成服务器id。
//可以根据这个id找到socket，与对应的服务器通信
//
#ifndef _COMMON_GAME_SERVER_CLASS_
#define _COMMON_GAME_SERVER_CLASS_

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "MessageCenter.h"
#include "Output.h"

#define READ_BUF_SIZE 1024
#define SEND_BUF_SIZE 100<<10

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> shared_ptr_socket;
class MapManager;
class GameSever{
public:
    GameSever(boost::asio::io_service &io_ser,MessageCenter &_msg_center);

    void preAccept();
    void acceptHandler(shared_ptr_socket psocket, boost::system::error_code ec);

    void async_read(int _h_socket);
    void read_handle(void * packege_head,int _h_socket,boost::system::error_code ec,std::size_t length);


    void async_write(int _h_socket,void * data,int data_size);
    void write_handle(int _h_socket,boost::system::error_code ec,std::size_t length);

    //向服务器发送地图数据
    void send_field_map(int _h_socket,int map_type_id ,int sence_id,MapManager &map_manager);
    //地图查找失败发送失败消息给服务器
    void sendError(int _h_socket,unsigned int scence_obj_id);

    bool try_find_socket(int _h_socket);
    shared_ptr_socket find_socket_by_id(int _h_socket);
    void delete_socket_by_id(int _h_socket);
private:
    //接收缓冲区
    char read_buf[READ_BUF_SIZE];
    //发送缓冲区
    char send_buf[SEND_BUF_SIZE];
    //保存socket
    std::map<int,shared_ptr_socket> socket_map;
    boost::asio::io_service &io_service;
    MessageCenter &msg_center;
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> pacceptor;
};

#endif // _COMMON_GAME_SERVER_CLASS_
