#include "GameServerClass.h"
#include "Output.h"
#include "Packege.h"
#include "MapManager.h"


#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <json/json.h>


using namespace std;
typedef boost::asio::ip::tcp tcp;

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
        int _h_socket = abs((now.time_of_day().total_microseconds()<<32)>>32);

        std::cout << "[GameServer]: there is a GameServer connecting : " << _h_socket << std::endl;

        socket_map.insert(pair<int,shared_ptr_socket>(_h_socket,psocket));
        async_read(_h_socket);
    }
    preAccept();
}

void GameSever::async_read(int _h_socket){
    find_socket_by_id(_h_socket)->async_read_some(boost::asio::buffer(read_buf,READ_BUF_SIZE),boost::bind(&GameSever::read_handle,this,read_buf,_h_socket,_1,_2));
}
void GameSever::read_handle(void * ptr_packege_head,int _h_socket,boost::system::error_code ec,std::size_t length){
    if(!ec){
        //将接收到的协议包解析成json对象传入消息队列
        Json::FastWriter  writer;
        big_packege packege_head;
        //读取包头 获得包长度
        memcpy(&packege_head,ptr_packege_head,sizeof(int)*2);
        packege_head.body = (char *) ptr_packege_head+sizeof(int)*2;

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

        memset(read_buf,'\0',READ_BUF_SIZE);
        async_read(_h_socket);
    }else{
        delete_socket_by_id(_h_socket);
    }
}

void GameSever::async_write(int _h_socket,void * data,int data_size){
    find_socket_by_id(_h_socket)->async_write_some(boost::asio::buffer(data,data_size),boost::bind(&GameSever::write_handle,this,_h_socket,_1,_2));
}
void GameSever::write_handle(int _h_socket,boost::system::error_code ec,std::size_t length){
    if(ec)
        delete_socket_by_id(_h_socket);
    else
    //    std::cout << "[GameServer] : send length :" << length << std::endl;
    return;
}


shared_ptr_socket GameSever::find_socket_by_id(int _h_socket){
    for(map<int,shared_ptr_socket>::iterator itr = socket_map.begin() ; itr != socket_map.end() ; itr++){
        if(itr->first ==  _h_socket) {
            return itr->second;
        }
    }
    std::cout << "[GameServer] find failed !" << std::endl;
    return socket_map.begin()->second;
}
void GameSever::delete_socket_by_id(int _h_socket){
    std::cout << "[GameSever]: delete a GameSever socket: " << _h_socket << std::endl;
    for(map<int,shared_ptr_socket>::iterator itr = socket_map.begin() ; itr != socket_map.end() ; itr++){
        if(itr->first ==  _h_socket) {
            itr->second->close();
            socket_map.erase(itr);
            return;
        }
    }
}
void GameSever::sendError(int _h_socket,unsigned int scence_obj_id){
    char failed_msg[18];
    unsigned int len = 2+sizeof(reply_map);
    unsigned int clen = len;
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
        std::stringstream ss;
        ss << "http://"<<param.NODE_ADDRESS<<"/getmap?map="<<ret.second<<"&&code="<<ret.first;

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

    }catch(...){
        std::cout << "[GameSever][getMap][error]" << std::endl;
        sendError(_h_socket,sence_id);
        return;
    }

}
