

#include "GameServerManager.h"
#include <boost/bind.hpp>
#include <json/json.h>
#include "Output.h"
#include "Packege.h"
#include "GameServer.h"
#include "Packege.h"

typedef boost::asio::ip::tcp tcp;

class MessageList;

GameSeverManager::GameSeverManager(boost::asio::io_service &io_ser,MessageList &_msgLst,TaskList & _taskLst):mm(_taskLst),msgLst(_msgLst){
    tcp::endpoint endpoint(tcp::v4(),GameServerPort);
    pacceptor.reset(new tcp::acceptor(io_ser,endpoint));
    preAccept();
    std::cout << "GameSeverManager has started !" << std::endl;
}

void GameSeverManager::preAccept(){
    boost::shared_ptr<boost::asio::ip::tcp::socket> p(new boost::asio::ip::tcp::socket(pacceptor->get_io_service()));
    pacceptor->async_accept(*p,boost::bind(&GameSeverManager::acceptHandler,this,p,_1));
}

void GameSeverManager::acceptHandler(boost::shared_ptr<boost::asio::ip::tcp::socket> psocket , boost::system::error_code ec){
    preAccept();
    shared_gameserver p(new GameSever(psocket,msgLst,mm));
    GSLst.push_back(p);
}
bool GameSeverManager::findGameServer(int id,GameSever **gs){
    for(std::list<shared_gameserver>::iterator  itr=GSLst.begin() ; itr!=GSLst.end() ; itr++){
        if((*itr)->getID()==id){
            *gs = &(**itr);
            return true;
        }
    }
    return false;
}
void GameSeverManager::sendMap(int id,int type,int sence){
    GameSever *gameserver=NULL;
    reply_map reply;
    if(findGameServer(id,&gameserver)){
        try{
            std::string sendstr;
            std::string & rmap = *(mm.getMap(type));
            reply.result = 1;
            reply.scence_obj_id = sence;
            reply.verify_code = 0;
            reply.data_len =  rmap.length();
            sendstr.assign((const char *)(&reply),sizeof(reply_map));
            sendstr.append(rmap);
            gameserver->async_write(sendstr);
        }catch(...){
            if(!mm.isexsit(type)){
                reply.result = 0;
                std::string sendstr;
                sendstr.assign((const char *)(&reply));
                gameserver->async_write(sendstr);
                std::cout << "catch Exception : sendTask" << std::endl;
            }
        }
    }
}
void GameSeverManager::delGameServer(int id){
    for(std::list<shared_gameserver>::iterator  itr=GSLst.begin() ; itr!=GSLst.end() ; itr++){
        if((*itr)->getID()==id){
            GSLst.erase(itr);
            return ;
        }
    }
}
