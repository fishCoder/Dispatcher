#include "INIReader.h"


INI::INI(string ini_file){
    if (access(ini_file.c_str(), 0) == 0) {
        this->err_code = 0;
        boost::property_tree::ini_parser::read_ini(ini_file, this->m_pt);
    } else {
        cout << "format ini error!" << endl;
        this->err_code = 1;
        exit(0);
    }
}

short INI::errCode(){
    return this->err_code;
}

string INI::get(string path){
    if (this->err_code == 0) {
        return this->m_pt.get<string>(path);
    } else {
        return "";
    }
}
int INI::get_int(string path){
    return m_pt.get<int>(path);
}
//bool INI::get_bool(string path){
//    return m_pt.get<bool>(path);
//}
