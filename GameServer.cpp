#include "GameServer.h"
//#include <boost/asio/yield.hpp>
#include <json/json.h>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <exception>
#include "MessageList.h"
#include "Packege.h"

GameSever::GameSever(boost::shared_ptr<tcp::socket> _psocket,MessageList &_msgLst,MapManager &_mm):psocket(_psocket),cpool(sizeof(char)),msgLst(_msgLst),mm(_mm){
    boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
    this->id = abs((now.time_of_day().total_microseconds()<<32)>>32);
    std::cout << "[GameSever]: there is a new gameserver connecting " << id << std::endl;
    async_read();
}
void GameSever::async_read(){
    big_packege * bp = (big_packege *) cpool.ordered_malloc(sizeof(big_packege));
    psocket->async_read_some(boost::asio::buffer(bp,8),boost::bind(&GameSever::read_handle,this,bp,_1,_2));
}
void GameSever::read_handle(void * _bp,boost::system::error_code ec,std::size_t length){
    std::cout << "GameSever::read_handle()" << std::endl;
    if(!ec){
        Json::FastWriter  writer;
        big_packege * bp = (big_packege *)_bp;
        bp->body = (char *) cpool.ordered_malloc(bp->len);
        boost::system::error_code ec;
        psocket->read_some(boost::asio::buffer(bp->body,bp->len),ec);

        if(!ec) {
            Packege packege;
            packege.parse(bp->body,bp->len);
            while(packege.hasNext()){
                req_map  ptr_req_map = packege.getBody();
                Json::Value root;
                root["type"] = 100;
                root["id"] = this->id;
                root["map"] = ptr_req_map.map_id;
                root["scence"] = ptr_req_map.scence_obj_id;
                msgLst.push_back(writer.write(root));
            }
        }
        cpool.free(bp->body);


        async_read();
    }else{
        Json::Value root;
        Json::FastWriter  writer;

        root["type"] = 101;
        root["id"]   = this->id;

        msgLst.push_back(writer.write(root));
        psocket->close();
    }
    cpool.free(_bp);
}
void GameSever::async_write(std::string &sendstr){
    psocket->async_write_some(boost::asio::buffer(sendstr.c_str(),sendstr.length()),boost::bind(&GameSever::write_handle,this,_1,_2));
}
void GameSever::write_handle(boost::system::error_code ec,std::size_t length){
    if(ec){
        Json::FastWriter  writer;
        Json::Value root;

        root["type"] = 101;
        root["id"]   = this->id;

        msgLst.push_back(writer.write(root));
        psocket->close();
    }
    std::cout << "send length :" << length << std::endl;
}
int GameSever::getID(){
    return id;
}
