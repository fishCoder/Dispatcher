#include "GameServerClass.h"
#include "Packege.h"
#include "MapManager.h"


#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <json/json.h>

#include <signal.h>
#include <unistd.h>

using namespace std;
typedef boost::asio::ip::tcp tcp;


static int amount = 0;
static int send_amount = 0;

GameSever::GameSever(boost::asio::io_service &io_ser,MessageCenter &_msg_center):
socket_map(),io_service(io_ser),msg_center(_msg_center)
{
    tcp::endpoint endpoint(tcp::v4(),GameServerPort);
    pacceptor.reset(new tcp::acceptor(io_ser,endpoint));
    preAccept();
    std::cout << "[GameSever]: has started !" << std::endl;
}

void GameSever::preAccept(){
    shared_ptr_socket p(new boost::asio::ip::tcp::socket(pacceptor->get_io_service()));

    pacceptor->async_accept(*p,boost::bind(&GameSever::acceptHandler,this,p,_1));
}
void GameSever::acceptHandler(shared_ptr_socket psocket, boost::system::error_code ec){
    if(!ec){
        boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
        int _h_socket = ++amount;//abs((now.time_of_day().total_microseconds()<<32)>>32);
        std::cout << "[GameServer]: there is a GameServer connecting : " << _h_socket << std::endl;
        set_socket_option(psocket);
        socket_map.insert(pair<int,shared_ptr_socket>(_h_socket,psocket));
        async_read(_h_socket);

    }else{
        psocket->close();
        //std::cout << "[GameSever][acceptHandler]:" << ec.message() << std::endl;
    }
    preAccept();

}

void GameSever::async_read(int _h_socket){
    if(try_find_socket(_h_socket))
        find_socket_by_id(_h_socket)->async_read_some(boost::asio::buffer(read_buf,sizeof(unsigned int)*2),boost::bind(&GameSever::read_handle,this,read_buf,_h_socket,_1,_2));
}
void GameSever::read_handle(void * ptr_packege_head,int _h_socket,boost::system::error_code ec,std::size_t length){
    if(!ec){
        //将接收到的协议包解析成json对象传入消息队列
        Json::FastWriter  writer;
        big_packege packege_head;
        //读取包头 获得包长度
        memcpy(&packege_head,ptr_packege_head,sizeof(int)*2);
        packege_head.body = (char *) ptr_packege_head+sizeof(int)*2;
        /**
        * 接收到的长度 len
        */
        unsigned int len=0;
        /**
        *尝试从socket中读取len字节
        */
        //std::cout << "读取开始" << std::endl;
        while(len!=packege_head.len){
            len += find_socket_by_id(_h_socket)->read_some(boost::asio::buffer((packege_head.body+len),(packege_head.len-len)),ec);
            if(ec){
                std::stringstream ss;
                ss << "{\"type\":400,\"hsocket\":"<<_h_socket<<"}";
                msg_center.push_message(ss.str());
                std::cout << "[GameSever][read_handle][body] " << ec.message() << std::endl;
            }
        }
        //std::cout << "读取完毕" << std::endl;

        Packege packege;
        //解析小包
        packege.parse(packege_head.body,packege_head.len);
        while(packege.hasNext()){
            req_map  ptr_req_map = packege.getBody();
            Json::Value root;
            root[OPERATE_TYEP] = 100;
            root[_H_SOCKET] = _h_socket;
            root[MAP_ID] = ptr_req_map.map_id;
            root[SENCE_ID] = ptr_req_map.scence_obj_id;
            msg_center.push_message(writer.write(root));
        }
        //std::cout << "解析完毕" << std::endl;
        memset(read_buf,'\0',READ_BUF_SIZE);
        async_read(_h_socket);
    }else{
        std::stringstream ss;
        ss << "{\"type\":400,\"hsocket\":"<<_h_socket<<"}";
        msg_center.push_message(ss.str());
        //std::cout << "[GameSever][read_handle] " << ec.message() << std::endl;
    }
}

void GameSever::async_write(int _h_socket,void * data,int data_size){
    if(try_find_socket(_h_socket)){
        find_socket_by_id(_h_socket)->async_write_some(boost::asio::buffer(data,data_size),boost::bind(&GameSever::write_handle,this,_h_socket,_1,_2));
        std::cout << "[static send_amount]" << send_amount++ << "send_length :"  << data_size << std::endl;
    }
}
void GameSever::write_handle(int _h_socket,boost::system::error_code ec,std::size_t length){
    if(ec){
        std::stringstream ss;
        ss << "{\"type\":400,\"hsocket\":"<<_h_socket<<"}";
        msg_center.push_message(ss.str());
        std::cout << "[GameSever][write_handle] " << ec.message() << std::endl;
    }
    else
    //    std::cout << "[GameServer] : send length :" << length << std::endl;
    return;
}
bool GameSever::try_find_socket(int _h_socket){
    return socket_map.find(_h_socket)!=socket_map.end();
}

shared_ptr_socket GameSever::find_socket_by_id(int _h_socket){
    return socket_map.find(_h_socket)->second;
}



void GameSever::delete_socket_by_id(int _h_socket){
    //std::cout << "[GameSever]: delete a GameSever socket: " << _h_socket << std::endl;
    socket_map.erase(_h_socket);
    //std::cout << "[GameSever][socket_map][size]:" << socket_map.size() << std::endl;
}


void GameSever::sendError(int _h_socket,unsigned int scence_obj_id){
    char failed_msg[18];
    unsigned int len = 2+sizeof(reply_map);
    unsigned int clen = send_amount;
    unsigned short little = sizeof(reply_map);
    reply_map reply;
    reply.result = 0;
    reply.scence_obj_id=scence_obj_id;
    reply.ulen = 0;
    memcpy(failed_msg,&len,4);
    memcpy(failed_msg+4,&clen,4);
    memcpy(failed_msg+8,&little,2);
    memcpy(failed_msg+10,&reply,sizeof(reply));
    async_write(_h_socket,failed_msg,18);
}
void GameSever::send_field_map(int _h_socket,int map_type_id ,int sence_id,MapManager & map_manager){
    reply_map reply;
    std::string sendstr;
    std::string map_data;
    std::string loot_npc_data;
    try{
        std::pair<unsigned int,std::string>  ret = map_manager.get_field_map(map_type_id,map_data,loot_npc_data);
        //msg_center.push_task(boost::bind(&MapManager::useMapKey,&map_manager,ret.second,map_type_id));
        std::stringstream ss;
        ss << "http://"<<param.get_node_url()<<"/getmap?map="<<ret.second<<"&&code="<<ret.first;

        reply.result = 1;
        reply.scence_obj_id = sence_id;
        reply.ulen = ss.str().length();

        int data_len = map_data.length();

        int npc_data_len = loot_npc_data.length();

        short _little_packege = sizeof(reply_map)+ss.str().length()+data_len+2*sizeof(int)+npc_data_len;

        int len = 2+_little_packege;
        int clen = len;

        int packege_size = len+sizeof(int)*2;
        char * send = send_buf;
        //大包头
        memcpy(send,(&len),sizeof(int));
        memcpy(send+4,(&clen),sizeof(int));

        //小包头
        memcpy(send+8,(&_little_packege),sizeof(short));

        //包体
        memcpy(send+10,(&reply),sizeof(reply_map));

        memcpy(send+18,ss.str().c_str(),reply.ulen);
        memcpy(send+18+reply.ulen,&data_len,sizeof(int));
        memcpy(send+22+reply.ulen,map_data.c_str(),map_data.length());
        memcpy(send+22+reply.ulen+data_len,(&npc_data_len),sizeof(int));
        memcpy(send+26+reply.ulen+data_len,loot_npc_data.c_str(),npc_data_len);
        async_write(_h_socket,send,packege_size);
        memset(send,'\0',SEND_BUF_SIZE);

    }catch(exception &e){
        //std::cout << "[GameSever][getMap][error]" << e.what() << std::endl;
        sendError(_h_socket,sence_id);
    }

}
