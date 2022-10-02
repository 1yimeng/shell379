#include <map>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <cstring>
#include <csignal>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

struct Process;
extern map<pid_t, Process> all_process;

void start_new_process(vector<char *> commands, string commandLine, bool is_background);
int get_p_id(vector<char *> commands);
void set_p_status(Process& cur_process, char status);
bool check_p_id(int p_id);
