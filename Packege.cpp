#include "Packege.h"
#include "stdio.h"
#include <string.h>
Packege::Packege(){
    this->body = NULL;
    this->pos = 0;
}
void Packege::parse(char * _body,int len){
    this->body = _body;
    this->len = len;
    this->pos = 0;
}
bool Packege::hasNext(){
    return pos<len;
}
req_map Packege::getBody(){
    short _size;
    req_map  ptr_req_map;
    memcpy(&_size,body+pos,sizeof(short));

    pos += sizeof(short);

    memcpy(&ptr_req_map,body+pos,sizeof(req_map));
    pos += _size;
    return ptr_req_map;
}
