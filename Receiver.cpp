#include "Receiver.h"
#include "boost/bind.hpp"
#include <unistd.h>
#include "Generator.h"

#include "MessageList.h"
Receiver::Receiver(MessageList & msgLst):msgLst(msgLst){}

void Receiver::pusk_msg(std::string msg){
    msgLst.push_back(msg);
}
void Receiver::regReader(boost::shared_ptr<Generator> pGenerator){

    pGenerator->getSocket().async_read_some(boost::asio::buffer(pGenerator->readbuf,1024),boost::bind(&Receiver::readHanler,this,pGenerator,_1,_2));
}
void Receiver::readHanler(boost::shared_ptr<Generator> pGenerator,boost::system::error_code ec,size_t len){
    if(!ec){
        pusk_msg(pGenerator->readbuf);
        regReader(pGenerator);
    }
    else{
        int id = pGenerator->getID();
        delGenerator(id);
    }
}


void Receiver::addGenerator(boost::shared_ptr<boost::asio::ip::tcp::socket> psocket){
    boost::shared_ptr<Generator> pGenerator(new Generator(psocket));
    lstGenerator.push_back(pGenerator);
    regReader(pGenerator);
}


int Receiver::getFreeGenrator(){

    std::list<share_generator>::iterator itr;
    for(itr=lstGenerator.begin(); itr!=lstGenerator.end(); itr++){
        if((*itr)->isfree()) {
            return (*itr)->getID();
        }
    }
    return -1;
}

boost::shared_ptr<Generator> Receiver::findGenerator(int id){

    std::list<share_generator>::iterator itr;
    for(itr=lstGenerator.begin(); itr!=lstGenerator.end(); itr++){
        if((*itr)->cmpID(id))
            return (*itr);
    }
    std::cout << "糟糕程序要宕了";
    return *itr;
}

void Receiver::delGenerator(int id){

    std::cout << "[Receiver]  delete receiver:" << id << std::endl;
    std::list<share_generator>::iterator itr;
    for(itr=lstGenerator.begin(); itr!=lstGenerator.end(); itr++){
        if((*itr)->cmpID(id)){
            lstGenerator.erase(itr);
            break;
        }
    }

    showLstGenerator();
}

void Receiver::sendHandler(int id,boost::system::error_code ec,size_t len){
    if(ec){
        delGenerator(id);
    }else{
     //   std::cout << "send length : " << len << std::endl;
    }
}
void Receiver::setState(int id , int state){
    std::list<share_generator>::iterator itr;
    for(itr=lstGenerator.begin(); itr!=lstGenerator.end(); itr++){
        if((*itr)->cmpID(id)){
            (*itr)->setState(state);
            break;
        }
    }
}
void Receiver::sendTask(int id,std::string task){
    std::cout << "[Receiver] task : " << task << std::endl;
    findGenerator(id)->current_task = task;
    findGenerator(id)->getSocket().async_write_some(boost::asio::buffer(task.c_str(),task.length()), boost::bind(&Receiver::sendHandler,this,id,_1,_2));

}
void Receiver::showLstGenerator(){
    return ;
    std::list<share_generator>::iterator itr;
    for(itr=lstGenerator.begin(); itr!=lstGenerator.end(); itr++){
        std::cout << "id  :  " << (*itr)->getID() << std::endl;
    }
}
