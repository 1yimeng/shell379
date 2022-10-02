#include "process.h"

using namespace std;

struct Process {
    int p_id;
    string commandLine;
    char p_status; // r: running, p: paused 
};

map<pid_t, Process> all_process = {};

void signal_handle_background_wait(int sig) {
    int status;
    pid_t child;
    while ((child = waitpid(-1, &status, WNOHANG)) > 0) {
        all_process.erase(child);
    }
    return;
}

int get_p_id(vector<char *> real_commands) {
    // assumes there is a valid p_id
    int p_id = atoi(real_commands[1]);
    return p_id;
}

bool check_p_id(int p_id) {
    if (all_process.find(p_id) == all_process.end()) {
        return false; 
    }
    return true;
}

void set_p_status(Process& cur_process, char status) {
    cur_process.p_status = status; 
}

int find_file_position(vector<char *> commands, char in_or_out) {
    for (int i = 0; i < commands.size(); i++) {
        if (*commands[i] == in_or_out) {
            // pointer to the next letter exclude '>'
            return i;
        }
    }

    return -1;
}

vector<char *> remove_file_name(vector<char *> commands, char in_or_out) {
    // returns the file name after removing it 
    int indx = find_file_position(commands, in_or_out);
    if (indx != -1) {
        commands.erase(commands.begin()+indx);
    }
    return commands;
}

char *get_file_name(vector<char *>& commands, char in_or_out) {
    char* fname = nullptr;
    for (char* word : commands) {
        if (*word == in_or_out) {
            // pointer to the next letter exclude '>'
            // cout << word+1 << endl;
            fname = word + 1;
        }
    }
    return fname;
}

void start_new_process(vector<char *> commands, string commandLine, bool is_background) {

    char* outFname = get_file_name(commands, '>');
    vector<char *> real_commands = remove_file_name(commands, '>');

    char* inFname = get_file_name(commands, '<');
    // cout << inFname;
    vector<char *> final_commands;

    if (inFname != nullptr) {
        int inFname_indx = find_file_position(real_commands, '<');
        final_commands = remove_file_name(real_commands, '<');
        ifstream infile(inFname);

        for (string line; getline(infile,line);) {
            /* do something with the value */
            istringstream iss(line);
            string s;
            while (iss >> s) {
                char *charWord = new char[s.size()+1];
                strcpy(charWord, s.c_str());
                final_commands.insert(final_commands.begin()+inFname_indx,charWord);
                inFname_indx += 1;
            }
        }

        // for (const auto &word : final_commands) {
        //     cout << word << endl;
        // }
    }

    char *argv[final_commands.size() + 1];

    for (int i = 0; i < final_commands.size(); i++) {
        argv[i] = final_commands[i];
    }
    
    argv[final_commands.size()] = nullptr;

    pid_t p_id = fork();

    if (p_id < 0) {
        perror("fork problem");
    } else if (p_id == 0) {
        // child process

        if (outFname != nullptr) {
            // writing in file
            FILE* pFile;
            pFile = freopen(outFname, "w", stdout);
            int result = execvp(argv[0], argv);
            if(result < 0) {
                perror("Exec problem");
            } 
            fclose(pFile);
        } else {
            int result = execvp(argv[0], argv);
            if(result < 0) {
                perror("Exec problem");
            } 
        }

    } else {
        // parent process
        if (is_background) {
            Process parent = {p_id, commandLine, 'R'};
            all_process[p_id] = parent;
            signal(SIGCHLD, signal_handle_background_wait);
        } else {
            // not in background, we need get notified if it exits
            waitpid(p_id, nullptr, 0);
        }

    }
}