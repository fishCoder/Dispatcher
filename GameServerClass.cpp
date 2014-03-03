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

GameSever::GameSever(boost::asio::io_service &io_ser,MessageList &_msgLst):
socket_pool(sizeof(char)),socket_map(),io_service(io_ser),msgLst(_msgLst)
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
    big_packege * bp = (big_packege *) socket_pool.ordered_malloc(sizeof(big_packege));
    find_socket_by_id(_h_socket)->async_read_some(boost::asio::buffer(bp,sizeof(int)*2),boost::bind(&GameSever::read_handle,this,bp,_h_socket,_1,_2));
}
void GameSever::read_handle(void * packege_head,int _h_socket,boost::system::error_code ec,std::size_t length){
    if(!ec){

        Json::FastWriter  writer;
        big_packege * ptr_packege_head = (big_packege *)packege_head;
        ptr_packege_head->body = (char *) socket_pool.ordered_malloc(ptr_packege_head->len);
        boost::system::error_code m_ec;
        find_socket_by_id(_h_socket)->read_some(boost::asio::buffer(ptr_packege_head->body,ptr_packege_head->len),m_ec);

        if(!m_ec) {
            Packege packege;
            packege.parse(ptr_packege_head->body,ptr_packege_head->len);
            while(packege.hasNext()){
                req_map  ptr_req_map = packege.getBody();
                Json::Value root;
                root[OPERATE_TYEP] = 100;
                root[_H_SOCKET] = _h_socket;
                root[MAP_ID] = ptr_req_map.map_id;
                root[SENCE_ID] = ptr_req_map.scence_obj_id;
                msgLst.push_back(writer.write(root));
            }
        }
        socket_pool.free(ptr_packege_head->body);
        async_read(_h_socket);
    }else{
        delete_socket_by_id(_h_socket);
    }
    socket_pool.free(packege_head);
}

void GameSever::async_write(int _h_socket,void * data,int data_size){
    find_socket_by_id(_h_socket)->async_write_some(boost::asio::buffer(data,data_size),boost::bind(&GameSever::write_handle,this,_h_socket,_1,_2));
}
void GameSever::write_handle(int _h_socket,boost::system::error_code ec,std::size_t length){
    if(ec)
        delete_socket_by_id(_h_socket);
    else
        std::cout << "[GameServer] : send length :" << length << std::endl;
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

void GameSever::send_field_map(int _h_socket,int map_type_id ,int sence_id,MapManager & map_manager){
    reply_map reply;
    std::string sendstr;

    pair<string,int> ret_pair = map_manager.get_field_map(map_type_id);
    string & map_data = ret_pair.first;
    reply.result = 1;
    reply.scence_obj_id = sence_id;
    reply.verify_code = ret_pair.second;
    reply.data_len = map_data.length();

    short _little_packege = map_data.length()+sizeof(reply_map);

    int len = 2+_little_packege;
    int clen = len;

    int packege_size = len+sizeof(int)*2;
    char * send = (char *) socket_pool.ordered_malloc(packege_size);
    memcpy(send,(&len),sizeof(int));
    memcpy(send+4,(&clen),sizeof(int));
    memcpy(send+8,(&_little_packege),sizeof(short));
    memcpy(send+10,(&reply),sizeof(reply_map));
    memcpy(send+24,map_data.c_str(),map_data.length());

    async_write(_h_socket,send,packege_size);

    socket_pool.free(send);
}
