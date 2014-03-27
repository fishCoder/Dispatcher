#include "NodeServer.h"
#include "Output.h"

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <json/json.h>
#include <boost/lexical_cast.hpp>
using namespace std;

typedef boost::asio::ip::tcp tcp;

NodeServer::NodeServer(boost::asio::io_service &io_ser,MessageCenter &_msg_center):
socket_pool(sizeof(char)),socket_map(),io_service(io_ser),msg_center(_msg_center)
{
    tcp::endpoint endpoint(tcp::v4(),NodeJsPort);
    pacceptor.reset(new tcp::acceptor(io_ser,endpoint));
    preAccept();
    std::cout << "[NodeServer]: has started !" << std::endl;
}

void NodeServer::preAccept(){
    shared_ptr_socket p(new boost::asio::ip::tcp::socket(pacceptor->get_io_service()));
    pacceptor->async_accept(*p,boost::bind(&NodeServer::acceptHandler,this,p,_1));
}
void NodeServer::acceptHandler(shared_ptr_socket psocket, boost::system::error_code ec){
    if(!ec){
        boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
        int _h_socket = abs((now.time_of_day().total_microseconds()<<32)>>32);

        std::cout << "[NodeServer]: there is a node connecting : " << _h_socket << std::endl;

        socket_map.insert(pair<int,shared_ptr_socket>(_h_socket,psocket));
        async_read(_h_socket);
        preAccept();
    }
}

void NodeServer::async_read(int _h_socket){
    char * receive_pool_size = (char *) socket_pool.ordered_malloc(5);
    find_socket_by_id(_h_socket)->async_read_some(
                                                  boost::asio::buffer(receive_pool_size,5),
                                                  boost::bind(
                                                              &NodeServer::read_handle,
                                                              this,
                                                              receive_pool_size,
                                                              _h_socket,
                                                              _1,
                                                              _2
                                                              )
                                                  );
}
void NodeServer::read_handle(char * receive_pool_size, int _h_socket,boost::system::error_code ec,std::size_t length){
    if(!ec){
        int _receive_size = boost::lexical_cast<int>(receive_pool_size);
        char * c_msg = (char *) socket_pool.ordered_malloc(_receive_size);
        //std::cout << "[NodeServer]: receive_pool_size  " << _receive_size << std::endl;

        boost::system::error_code ec;
        find_socket_by_id(_h_socket)->read_some(boost::asio::buffer(c_msg,_receive_size),ec);
        if(!ec){
            //std::cout << "[NodeServer]: content: " << c_msg << std::endl;
            Json::Reader reader;
            Json::FastWriter writer;
            Json::Value root;
            std::string json_msg(c_msg);
            if(reader.parse(json_msg,root)){
                root[_H_SOCKET] = _h_socket;
            }
            msg_center.push_message(writer.write(root));
        }

        socket_pool.free(c_msg);
        async_read(_h_socket);
    }else{
        delete_socket_by_id(_h_socket);
    }
    socket_pool.free(receive_pool_size);
}

void NodeServer::async_write(int _h_socket,std::string &sendstr){
    find_socket_by_id(_h_socket)->async_write_some(boost::asio::buffer(sendstr.c_str(),sendstr.length()),boost::bind(&NodeServer::write_handle,this,_h_socket,_1,_2));
}
void NodeServer::write_handle(int _h_socket,boost::system::error_code ec,std::size_t length){
    if(!ec){
        std::cout << "[NodeServer]: sending success " << length << std::endl;
    }else{
        delete_socket_by_id(_h_socket);
    }
}

shared_ptr_socket NodeServer::find_socket_by_id(int _h_socket){
    for(map<int,shared_ptr_socket>::iterator itr = socket_map.begin() ; itr != socket_map.end() ; itr++){
        if(itr->first ==  _h_socket) return itr->second;
    }
    return socket_map.begin()->second;
}
void NodeServer::delete_socket_by_id(int _h_socket){
    std::cout << "[NodeServer]: delete a node : " << _h_socket << std::endl;
    for(map<int,shared_ptr_socket>::iterator itr = socket_map.begin() ; itr != socket_map.end() ; itr++){
        if(itr->first ==  _h_socket) {
            itr->second->close();
            socket_map.erase(itr);
            return;
        }
    }
}
