#include "GeneratorClass.h"
#include "TaskList.h"
#include <time.h>


#define size_unsigned_int sizeof(unsigned int)


using namespace std;
typedef boost::asio::ip::tcp tcp;





Generator::Generator(boost::asio::io_service &io_ser,MessageCenter &_msg_center):
gen_map(),io_service(io_ser),msg_center(_msg_center)
{
    free_gen_amount = 0;
    tcp::endpoint endpoint(tcp::v4(),GeneratorServerPort);
    pacceptor.reset(new tcp::acceptor(io_ser,endpoint));
    preAccept();
    std::cout << "[Generator]: has started !" << std::endl;
}

void Generator::preAccept(){
    shared_ptr_socket p(new boost::asio::ip::tcp::socket(pacceptor->get_io_service()));
    pacceptor->async_accept(*p,boost::bind(&Generator::acceptHandler,this,p,_1));
}
void Generator::acceptHandler(shared_ptr_socket psocket, boost::system::error_code ec){
    if(!ec){
        /**
        *获取当前毫秒数 作为生成器ID
        */
        int _h_socket = 0;
        do{
            _h_socket = time(NULL);
        }while(gen_map.find(_h_socket)!=gen_map.end());
        std::cout << "[Generator]: there is a Generator connecting : " << _h_socket << std::endl;
        psocket->write_some(boost::asio::buffer(&_h_socket,sizeof(int)));
        //free_gen_amount++;
        gen_info *generator = new gen_info(psocket);

        map_read_buf.insert(pair<int,char *>(_h_socket,new char[1024]));
        map_send_buf.insert(pair<int,char *>(_h_socket,new char[1024]));
        gen_map.insert(pair<int,gen_info *>(_h_socket,generator));

        async_read(_h_socket);
    }
    preAccept();
}

void Generator::async_read(int _h_socket){
    find_gen_by_id(_h_socket)->ptr_socket->async_receive(boost::asio::buffer(read_buffer(_h_socket),size_unsigned_int),boost::bind(&Generator::read_handle,this,_h_socket,_1,_2));
}
void Generator::read_handle(int _h_socket,boost::system::error_code ec,std::size_t length){
    if(ec){
        std::stringstream ss;
        ss << "{\"type\":401,\"hsocket\":"<<_h_socket<<"}";
        msg_center.push_message(ss.str());
    }
    else{
        unsigned int len;
        memcpy(&len,read_buffer(_h_socket),size_unsigned_int);
        find_gen_by_id(_h_socket)->ptr_socket->async_receive(boost::asio::buffer(read_buffer(_h_socket),len),boost::bind(&Generator::read_body_handle,this,_h_socket,_1,_2));
    }

}
void Generator::read_body_handle(int _h_socket,boost::system::error_code ec,std::size_t length){
    if(ec){
        std::stringstream ss;
        ss << "{\"type\":401,\"hsocket\":"<<_h_socket<<"}";
        msg_center.push_message(ss.str());
    }
    else{
        msg_center.push_message_high_prio(read_buffer(_h_socket));
        async_read(_h_socket);
    }
}

void Generator::async_write(int _h_socket,void * data,unsigned int data_size){
    if(try_find_socket(_h_socket))
        find_gen_by_id(_h_socket)->ptr_socket->async_send(boost::asio::buffer(data,data_size),boost::bind(&Generator::write_handle,this,_h_socket,_1,_2));
}
void Generator::write_handle(int _h_socket,boost::system::error_code ec,std::size_t length){
    if(ec){
        std::stringstream ss;
        ss << "{\"type\":401,\"hsocket\":"<<_h_socket<<"}";
        msg_center.push_message(ss.str());
    }
    else
    //    std::cout << "[Generator] : send length :" << length << std::endl;
    return;
}

void Generator::delete_socket_by_id(int _h_socket){
    std::cout << "[Generator]: delete a Generator socket: " << _h_socket << std::endl;
    if(try_find_socket(_h_socket)){
        gen_info * gen = find_gen_by_id(_h_socket);
        if(gen->is_busy == false){
            free_gen_amount--;
        }
        gen_map.erase(_h_socket);

        delete map_read_buf.find(_h_socket)->second;
        map_read_buf.erase(_h_socket);

        delete map_send_buf.find(_h_socket)->second;
        map_send_buf.erase(_h_socket);
    }

}
bool Generator::try_find_socket(int _h_socket){
    return gen_map.find(_h_socket)!=gen_map.end();
}
gen_info * Generator::find_gen_by_id(int _h_socket){
    for(map<int,gen_info*>::iterator itr = gen_map.begin() ; itr != gen_map.end() ; itr++){
        if(itr->first ==  _h_socket) {
            return itr->second;
        }
    }
    //std::cout << "[Generator] find failed !" << std::endl;
    return gen_map.begin()->second;
}

void Generator::send_task(int _h_socket,std::string & task){
    gen_info *gen = find_gen_by_id(_h_socket);
    //std::cout << "[Generator] [free_gen_amount] " << free_gen_amount << std::endl;
    char * send_buf = send_buffer(_h_socket);
    unsigned int len = task.length();
    memcpy(send_buf,&len,size_unsigned_int);
    memcpy(send_buf+size_unsigned_int,task.c_str(),len);
    gen->ptr_socket->async_send(boost::asio::buffer(send_buf,len+size_unsigned_int),boost::bind(&Generator::write_handle,this,_h_socket,_1,_2));
}
void Generator::urgent_gen_task(TaskList &lst){
    if(has_free_generator()&&!lst.empty()){
        int _h_socket = find_free_generator();
        std::string task = lst.pop_front();
        send_task(_h_socket,task);
        std::cout << "[urgent_gen_task]" << task.size() << std::endl;
    }
}
int Generator::find_free_generator(){
    for(map<int,gen_info*>::iterator itr = gen_map.begin() ; itr != gen_map.end() ; itr++){
        if(itr->second->is_busy ==  false) {
            return itr->first;
        }
    }
    return 0;
}
bool Generator::has_free_generator(){
    return free_gen_amount > 0;
}
void Generator::set_generator_free(int _h_socket){
    boost::mutex::scoped_lock  lock(gen_mtx);
    find_gen_by_id(_h_socket)->is_busy = false;
    free_gen_amount++;
}
void Generator::set_generator_busy(int _h_socket){
    boost::mutex::scoped_lock  lock(gen_mtx);
    find_gen_by_id(_h_socket)->is_busy = true;
    free_gen_amount--;
}

char * Generator::read_buffer(int _h_socket){
    return map_read_buf.find(_h_socket)->second;
}
char * Generator::send_buffer(int _h_socket){
    return map_send_buf.find(_h_socket)->second;
}
