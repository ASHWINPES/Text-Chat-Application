#include "./global.h"
using namespace std;
class Client {
    public:
        const char* my_ip;
        string port_number;
        Client(string port_number);

        void get_my_ip(string input_command);
        int start_client(int argc, char **argv);
        int connect_to_host(char *server_ip, char* server_port);
};