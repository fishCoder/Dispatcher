#include "Generator.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <stdlib.h>
Generator::Generator(boost::shared_ptr<tcp::socket> psocket){
    std::cout << "Generator::Generator()" << std::endl;
    boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
    this->id =abs((now.time_of_day().total_microseconds()<<32)>>32);
    this->state = 0;
    this->psocket = psocket;
    psocket->write_some(boost::asio::buffer(&id,sizeof(int)));
}
void Generator::setState(int state){
    this->state = state;
}
bool Generator::isfree(){
    return state==0;
}
bool Generator::cmpID(int id){
    return this->id == id;
}
boost::asio::ip::tcp::socket & Generator::getSocket(){
    return *psocket;
}
void Generator::clearbuf(){
    memset(readbuf,'\0',1024);
}
int Generator::getID(){
    return id;
}
