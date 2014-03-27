#ifndef _MESSAGE_CENTER_
#define _MESSAGE_CENTER_

#include <boost/function.hpp>
#include <boost/threadpool.hpp>

class MessageCenter{
public:
    MessageCenter();
    void push_message(std::string msg);
    void set_handle_func(boost::function<void(std::string)> const & func);
private:
    boost::threadpool::pool thread_pool;
    boost::function<void(std::string)>  deal_message;
};

#endif // _MESSAGE_CENTER_
