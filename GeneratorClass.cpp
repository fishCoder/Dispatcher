#include "GeneratorClass.h"
#include "Output.h"

using namespace std;
typedef boost::asio::ip::tcp tcp;

Generator::Generator(boost::asio::io_service &io_ser,MessageCenter &_msg_center):
gen_map(),io_service(io_ser),msg_center(_msg_center)
{
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
        boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
        int _h_socket = abs((now.time_of_day().total_microseconds()<<32)>>32);

        std::cout << "[Generator]: there is a Generator connecting : " << _h_socket << std::endl;
        psocket->write_some(boost::asio::buffer(&_h_socket,sizeof(int)));
        free_gen_amount++;
        gen_info *generator = new gen_info(psocket);
        gen_map.insert(pair<int,gen_info *>(_h_socket,generator));
        async_read(_h_socket);
    }
    preAccept();
}

void Generator::async_read(int _h_socket){
    find_gen_by_id(_h_socket)->ptr_socket->async_read_some(boost::asio::buffer(read_buf,READ_BUF_SIZE),boost::bind(&Generator::read_handle,this,_h_socket,_1,_2));
}
void Generator::read_handle(int _h_socket,boost::system::error_code ec,std::size_t length){
    if(ec)
        delete_socket_by_id(_h_socket);
    else{
        msg_center.push_message(read_buf);
        async_read(_h_socket);
    }
    return;

}


void Generator::async_write(int _h_socket,void * data,int data_size){
    find_gen_by_id(_h_socket)->ptr_socket->async_write_some(boost::asio::buffer(data,data_size),boost::bind(&Generator::write_handle,this,_h_socket,_1,_2));
}
void Generator::write_handle(int _h_socket,boost::system::error_code ec,std::size_t length){
    if(ec)
        delete_socket_by_id(_h_socket);
    else
    //    std::cout << "[Generator] : send length :" << length << std::endl;
    return;
}

void Generator::delete_socket_by_id(int _h_socket){
    std::cout << "[Generator]: delete a Generator socket: " << _h_socket << std::endl;
    gen_info * gen = find_gen_by_id(_h_socket);
    if(gen->is_busy == false){
        free_gen_amount--;
    }
    gen_map.erase(_h_socket);
}
gen_info * Generator::find_gen_by_id(int _h_socket){
    for(map<int,gen_info*>::iterator itr = gen_map.begin() ; itr != gen_map.end() ; itr++){
        if(itr->first ==  _h_socket) {
            return itr->second;
        }
    }
    std::cout << "[Generator] find failed !" << std::endl;
    return gen_map.begin()->second;
}

void Generator::send_task(int _h_socket,std::string & task){
    gen_info *gen = find_gen_by_id(_h_socket);
    std::cout << "[Generator] [free_gen_amount] " << free_gen_amount << std::endl;
    gen->ptr_socket->async_write_some(boost::asio::buffer(task.c_str(),task.length()),boost::bind(&Generator::write_handle,this,_h_socket,_1,_2));
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
    find_gen_by_id(_h_socket)->is_busy = false;
    free_gen_amount++;
}
void Generator::set_generator_busy(int _h_socket){
    find_gen_by_id(_h_socket)->is_busy = true;
    free_gen_amount--;
}
