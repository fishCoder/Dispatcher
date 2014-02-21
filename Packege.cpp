#include "Packege.h"

#include <string.h>
Packege::Packege(){
    this->packege = NULL;
    this->pos = 0;
}
void Packege::parse(big_packege * _packege){
    this->packege = _packege;
    this->len = _packege->len;
    this->pos = 2*sizeof(int);
}
bool Packege::hasNext(){
    return pos<=len;
}
req_map * Packege::getBody(){
    short _size;
    req_map * ptr_req_map=NULL;
    memcpy(packege+pos,&_size,sizeof(short));
    pos += sizeof(short);
    ptr_req_map = (req_map *) packege+pos;
    pos += _size;
    return ptr_req_map;
}
