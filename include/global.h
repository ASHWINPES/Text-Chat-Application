#include <map>
#include <string>
using namespace std;

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define HOSTNAME_LEN 128
#define PATH_LEN 256

extern std::map<int, pair<std::string, std::string> > clients;
#endif
