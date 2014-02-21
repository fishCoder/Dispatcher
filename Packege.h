#ifndef _COMMON_PACKEGE_
#define _COMMON_PACKEGE_

#include <iostream>

typedef struct _big_packege{
    unsigned int len;
    unsigned int clen;
    char * body;
}big_packege;

typedef struct _little_packege{
    unsigned short len;
    char * body;
}little_packege;

#pragma pack(push,1)
typedef struct _req_map{
    unsigned short map_id;
    unsigned int scence_obj_id;
    _req_map():map_id(0),scence_obj_id(0){};
}req_map;
#pragma pack(pop)

typedef struct _reply_map{
    unsigned short result;
    unsigned int scence_obj_id;
    unsigned int verify_code;
    unsigned int data_len;
}reply_map;

class Packege{
public:
    Packege();
    void parse(big_packege * _packege);
    bool hasNext();
    req_map * getBody();
private:
    big_packege * packege;
    int pos;
    int len;
};

#endif
