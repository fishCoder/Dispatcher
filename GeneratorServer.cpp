#include "GeneratorServer.h"
#include "Output.h"

#include "MessageList.h"
#include <boost/bind.hpp>

GeneratorServer::GeneratorServer(boost::asio::io_service &io_service,MessageList & msgLst):receiver(msgLst){
    tcp::endpoint endpoint(tcp::v4(),GeneratorServerPort);
    pacceptor.reset(new tcp::acceptor(io_service,endpoint));
    preAccept();

    std::cout << "[GeneratorServer]: has started !" << std::endl;
}

void GeneratorServer::preAccept(){
    boost::shared_ptr<boost::asio::ip::tcp::socket> p(new boost::asio::ip::tcp::socket(pacceptor->get_io_service()));
    pacceptor->async_accept(*p,boost::bind(&GeneratorServer::acceptHandler,this,p,_1));
}

void GeneratorServer::acceptHandler(boost::shared_ptr<boost::asio::ip::tcp::socket> psocket , boost::system::error_code ec){
    preAccept();
    receiver.addGenerator(psocket);
}

void GeneratorServer::setState(int id , int state){
    receiver.setState(id,state);
}

std::string GeneratorServer::get_address(int id){
    return  receiver.get_address(id);
}

void GeneratorServer::sendTask(int id,std::string task){
    return receiver.sendTask(id,task);
}

int GeneratorServer::getFreeGenrator(){
    return receiver.getFreeGenrator();
}
