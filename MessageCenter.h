#ifndef _MESSAGE_CENTER_
#define _MESSAGE_CENTER_

#include <boost/function.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread/mutex.hpp>


#include "Output.h"



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
private:
    boost::mutex mc_mtx;
    boost::asio::io_service & io_ser;
    boost::function<void(std::string)>  deal_message;
};

#endif // _MESSAGE_CENTER_
