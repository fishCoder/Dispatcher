#ifndef _GENERATOR_CLASS_
#define _GENERATOR_CLASS_

#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include "MessageCenter.h"
#include "Output.h"

#define READ_BUF_SIZE 1024

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> shared_ptr_socket;


class TaskList;

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

    /**
    *网络操作
    */
    void preAccept();
    void acceptHandler(shared_ptr_socket psocket, boost::system::error_code ec);

    void async_read(int _h_socket);
    void read_handle(int _h_socket,boost::system::error_code ec,std::size_t length);
    void read_body_handle(int _h_socket,boost::system::error_code ec,std::size_t length);

    void async_write(int _h_socket,void * data,unsigned int data_size);
    void write_handle(int _h_socket,boost::system::error_code ec,std::size_t length);

    /**
    *根据id删除网络连接
    */
    void delete_socket_by_id(int _h_socket);
    /**
    *操作该id是否有网络连接
    */
    bool try_find_socket(int _h_socket);
    gen_info * find_gen_by_id(int _h_socket);


    void send_task(int _h_socket,std::string & task);
    /**
    *向生成器发送任务 一般用于发现数据库地图以及耗尽的情况下
    */
    void urgent_gen_task(TaskList &lst);

    /**
    *查找空闲的生成器 返回id
    */
    int find_free_generator();
    bool has_free_generator();

    /**
    *设置生成器状态
    */
    void set_generator_free(int _h_socket);
    void set_generator_busy(int _h_socket);

    char * read_buffer(int _h_socket);
    char * send_buffer(int _h_socket);
private:
    boost::mutex gen_mtx;
    //接收缓冲区
    map<int,char *>  map_read_buf;
    map<int,char *>  map_send_buf;

    std::map<int,gen_info *> gen_map;

    boost::asio::io_service &io_service;
    MessageCenter &msg_center;
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> pacceptor;

    /**
    *free_gen_amount 表示生成器的空闲数量
    *如 free_gen_amount = 1 表示有个一个生成器处于空闲状态
    *如 free_gen_amount = 0 表示没有生存器处于空闲
    *当一个空闲的生存器分配到一个任务free_gen_amount将减一
    *当一个生存器完成一个任务而调度器没有再分配一个任务给它时，则free_gen_amount加一
    */
    int free_gen_amount;
};


#endif // _GENERATOR_CLASS_
