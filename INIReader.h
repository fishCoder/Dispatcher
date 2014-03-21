
#ifndef INI_H
#define INI_H


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <string>
using namespace std;


class INI{
public:
    INI(string ini_file);
    string get(string path);
    int get_int(string path);
 //   bool get_bool(string path);
    short int errCode();
private:
    short int err_code;
    boost::property_tree::ptree m_pt;
};

#endif  /* INI_H */
