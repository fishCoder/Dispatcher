#ifndef _MESSAGE_CENTER_
#define _MESSAGE_CENTER_

#include <boost/function.hpp>
#include <boost/threadpool.hpp>
#include <boost/asio/io_service.hpp>

#include "Output.h"
using namespace boost::threadpool;
class MessageCenter{
public:
    MessageCenter(boost::asio::io_service & io_server);
    /**
    *投递到线程池执行
    */
    void push_message(std::string msg);
    /**
    *高优先级执行
    */
    void push_message_high_prio(std::string msg);
    void set_handle_func(boost::function<void(std::string)> const & func);
    void push_task(boost::function<void()>  const & func);
private:
    boost::asio::io_service & io_ser;
    prio_pool thread_pool;
    boost::function<void(std::string)>  deal_message;
};

#endif // _MESSAGE_CENTER_
