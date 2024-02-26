#include "./global.h"
using namespace std;

class Server {
    public:
        string port_number;
        Server(string p_n);

        void get_my_ip(string input_command);
        int start_server(int argc, char **argv);
        void process_server_command(char* cmd);
};