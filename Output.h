#include "INIReader.h"
//端口

#define GeneratorServerPort  9010
#define GameServerPort  9091
#define NodeJsPort 9099

//Json 协议

#define OPERATE_TYEP "type"
#define SENCE_ID "sence"
#define MAP_ID   "map"
#define _H_SOCKET "id"
#define CFG_SCHEME "scheme"
#define AMOUNT "num"

//static bool isoutput = true;

//void outputmsg(std::string msg){
//    if(isoutput)
//       std::cout << msg << std::endl;
//}
#ifndef _DISPATCHER_SET_PARAM_
#define _DISPATCHER_SET_PARAM_
#include <vector>
#include <iostream>
struct _settings_param_{
    std::string NODE_ADDRESS;
    std::string redis_address;
    int redis_port;
    int TASK_SIZE;
    int MAX_GET_TIMES;
    int MAX_USE_NUM;
    int EXSIT_TIME;
    int THREAD_POOL_NUM;
    int TIME_BUCKET;
    int is_run_config;
    int NODE_NUM;
    int SLOW_DEAL;
    std::vector<std::string> node_url;
    unsigned int mark;
    _settings_param_():node_url(){
        INI ini("dispatcher.ini");
        redis_address = ini.get("dispatcher.redisaddress");
        redis_port    = ini.get_int("dispatcher.redisport");
        EXSIT_TIME    = ini.get_int("dispatcher.exsit_time");
        TASK_SIZE     = ini.get_int("dispatcher.task_size");
        MAX_USE_NUM   = ini.get_int("dispatcher.map_max_use");
        THREAD_POOL_NUM = ini.get_int("dispatcher.thread_pool_num");
        TIME_BUCKET   = ini.get_int("dispatcher.time_bucket");
        SLOW_DEAL = ini.get_int("dispatcher.slow_deal");
        NODE_NUM =  ini.get_int("node.num");

        mark = 0;
        for(int i=0;i<NODE_NUM;i++){
            std::stringstream ss;
            ss << "node.url" << (i+1);
            node_url.push_back(ini.get(ss.str()));
        }
    };
    std::string get_node_url(){
        mark++;
        return node_url[mark%NODE_NUM];
    }
};
extern struct _settings_param_ param;
#endif // _DISPATCHER_SET_PARAM_



#ifndef _SOCKET_SETTINGS_
#define _SOCKET_SETTINGS_

#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> shared_ptr_socket;


//int static set_amount = 0;
inline void set_socket_option(shared_ptr_socket ptr_socket){
    boost::system::error_code ec;

    boost::asio::ip::tcp::socket & m_socket = *ptr_socket;

	boost::asio::ip::tcp::endpoint ep = m_socket.local_endpoint(ec);

	int try_count = 20;



	//const char* StepName[] = {"Nodelay", "Linger", "ReceiveBufferSize", "SendBufferSize","KeepAlive", "KeepAliveInterval", "KeepAliveIdle", "KeepAliveCount"};

	int step = 0;

	while(try_count--)
	{
		std::string msg;
		switch(step)
		{
		case 0:
			{
			    boost::asio::ip::tcp::no_delay optionNoDelay(true);
                m_socket.set_option(optionNoDelay, ec);
			}
			if(ec) break;
			step++;
		case 1:
			{
				//unsigned int  l_linger = 0;
				//POCO_LOG_WARNING(NET_LOGGER, __FUNCTION__<<" so linger:"<<l_linger);
				//boost::asio::socket_base::lingeroptionLinger(true, l_linger);
				//m_socket.set_option(optionLinger, ec);
			}
			if(ec) break;
			step++;
		case 2:
		    {
                boost::asio::detail::socket_option::integer<SOL_SOCKET, SO_RCVBUF> optionRcvBuff(65000);
                m_socket.set_option(optionRcvBuff, ec);
		    }
			if(ec) break;
			step++;

		case 3:
		    {
                boost::asio::detail::socket_option::integer<SOL_SOCKET, SO_SNDBUF> optionSndBuff(65000);
                m_socket.set_option(optionSndBuff, ec);
            }
			if(ec) break;
			step++;
            /**
			*case 4，5,6,7 的目的是为了防止TCP半连接的出现
			*/
		case 4:
			{
			    boost::asio::detail::socket_option::boolean<SOL_SOCKET, SO_KEEPALIVE> optionKeepAlive(true);
                m_socket.set_option(optionKeepAlive, ec);
			}
			if(ec) break;
			step++;

		case 5:
			{
			    boost::asio::detail::socket_option::integer<SOL_TCP, TCP_KEEPINTVL> optionKeepInterval(5);
                m_socket.set_option(optionKeepInterval, ec);
			}
			if(ec) break;
			step++;
		case 6:
			{
			    boost::asio::detail::socket_option::integer<SOL_TCP, TCP_KEEPIDLE> optionKeepIdle(30);
                m_socket.set_option(optionKeepIdle, ec);
			}
			if(ec) break;
			step++;
		case 7:
			{
			    boost::asio::detail::socket_option::integer<SOL_TCP, TCP_KEEPCNT> optionKeepCount(2);
                m_socket.set_option(optionKeepCount, ec);
			}
			if(ec) break;
			step = 100;
		}
		if(step == 100){
            //std::cout << "[set_socket_option]:" << ++set_amount << std::endl;
            break;
		}
	}

}


#endif // _SOCKET_SETTINGS_








