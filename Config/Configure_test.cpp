#include <iostream>
#include "Configure.h"

using namespace std;

int main(int argc, char* argv[]){
    Config& config = Config::getInstance();
    config.parse_args(argc, argv);
    cout << config.server_port << endl 
         << config.num_thread << endl
         << config.run_backend << endl 
         << config.enable_logging << endl
         << config.log_file_name << endl
         << config.log_mode << endl 
         << config.db_url << endl 
         << config.db_port << endl 
         << config.db_user << endl 
         << config.db_passwd << endl
         << config.min_num_conn << endl 
         << config.max_num_conn << endl;
    return 0;
}