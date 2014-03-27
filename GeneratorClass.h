#ifndef _GENERATOR_CLASS_
#define _GENERATOR_CLASS_

#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>

#include "MessageCenter.h"

#define READ_BUF_SIZE 1024

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> shared_ptr_socket;

typedef struct _gen_info{
    shared_ptr_socket ptr_socket;
    bool is_busy;
    void * current_task;
    _gen_info(shared_ptr_socket _ptr_socket)
    {
        ptr_socket = _ptr_socket;
        is_busy = false;
        current_task = NULL;
    }
}gen_info;

class Generator{
public:
    Generator(boost::asio::io_service &io_ser,MessageCenter &_msg_center);

    void preAccept();
    void acceptHandler(shared_ptr_socket psocket, boost::system::error_code ec);

    void async_read(int _h_socket);
    void read_handle(int _h_socket,boost::system::error_code ec,std::size_t length);

    void async_write(int _h_socket,void * data,int data_size);
    void write_handle(int _h_socket,boost::system::error_code ec,std::size_t length);

    void delete_socket_by_id(int _h_socket);
    gen_info * find_gen_by_id(int _h_socket);

    void send_task(int _h_socket,std::string & task);

    int find_free_generator();
    bool has_free_generator();
    void set_generator_free(int _h_socket);
    void set_generator_busy(int _h_socket);


private:
    //接收缓冲区
    char read_buf[READ_BUF_SIZE];
    //发送缓冲区
    char send_buf[READ_BUF_SIZE];

    std::map<int,gen_info *> gen_map;

    boost::asio::io_service &io_service;
    MessageCenter &msg_center;
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> pacceptor;

    //空闲的生成器数量
    int free_gen_amount;
};


#endif // _GENERATOR_CLASS_
