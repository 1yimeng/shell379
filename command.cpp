#include "command.h"

using namespace std;

struct Process {
    int p_id;
    string commandLine;
    char p_status; // R: running, P: paused
};

void get_usage() {
    struct rusage usages{};
    getrusage(RUSAGE_CHILDREN, &usages);
    
    printf("%s      %7d %s\n", "User time = ", (int) usages.ru_utime.tv_sec, "seconds");
    printf("%s   %7d %s\n\n", "Process time = ", (int) usages.ru_stime.tv_sec, "seconds");
}

void get_cpu_usage(int p_id) {
    string line = "ps -p " + to_string(p_id) + " times=";
    vector<char *> charCommand = process_line(line);
    char *argv[charCommand.size() + 1];

    for (int i = 0; i < charCommand.size(); i++) {
        argv[i] = charCommand[i];
    }
    
    argv[charCommand.size()] = nullptr;
    pid_t p_id = fork();
    if (p_id < 0) {
        perror("fork problem");
    } else if (p_id == 0) {
        execvp(argv[0],argv);
    } else {
        waitpid(p_id, nullptr, 0);
    }
}

void to_sleep(vector<char *> commands) {
    if (commands.size() < 2) {
        cout << "need a 2nd argument(number) for sleep time!" << endl;
        return;
    }
    sleep(stoi(commands[1]));
}

void to_kill(vector<char *> commands) {
    if (commands.size() < 2) {
        cout << "need a 2nd argument(number) for process id!" << endl;
        return;
    }

    if (!all_process.empty()) {
        int process_pid = get_p_id(commands);

        if (check_p_id(process_pid)) {
            kill(process_pid, SIGKILL);
            all_process.erase(process_pid);
        } else {
            cout << "process id not found" << endl;
        }

    } else {
        cout << "no process running" << endl;
    }

}

void show_jobs() {
    
    int counter = 0;
    printf("%s\n", "Running Processes:");
    if (!all_process.empty()) {

        printf("%s      %s   %s   %s %s\n", "#", "PID", "S", "SEC", "COMMAND");
        
        for (auto& t : all_process) {
            printf("%2d  %7d  %c  %3d  %s\n", counter, t.second.p_id, t.second.p_status, 0, t.second.commandLine.c_str());
            fflush(stdout);
            counter += 1;
        }
    }

    printf("%s      %2d %s\n", "Processes = ", counter, "active");
    printf("%s\n", "Completed Processes:");
    get_usage();
    fflush(stdout);
}

void to_exit() {
    vector<int> all_pids;
    for (auto& p : all_process) {
        // get all pids before signal handler removes them
        all_pids.push_back(p.first);
    }

    for (auto& p : all_pids) {
        kill(p, SIGCONT);
        waitpid(p, nullptr, 0);
    }

    cout << "Resources used" << endl;
    get_usage();
    _Exit(0);
}

void resume_process(vector<char *> commands) {
    if (commands.size() < 2) {
        cout << "need a 2nd argument(number) for process id!" << endl;
        return;
    }

    if (!all_process.empty()) {
        int process_pid = get_p_id(commands);

        if (check_p_id(process_pid)) {
            kill(process_pid, SIGCONT);
            set_p_status(all_process[process_pid], 'R');
        } else {
            cout << "process id not found" << endl;
        }

    } else {
        cout << "no process running" << endl;
    }

}

void suspend_process(vector<char *> commands) {
    if (commands.size() < 2) {
        cout << "need a 2nd argument(number) for process id!" << endl;
        return;
    }

    if (!all_process.empty()) {
        int process_pid = get_p_id(commands);

        if (check_p_id(process_pid)) {
            kill(process_pid, SIGSTOP);
            set_p_status(all_process[process_pid], 'P');
        } else {
            cout << "process id not found" << endl;
        }

    } else {
        cout << "no process running" << endl;
    }

}

void to_wait(vector<char *> commands) {
    if (commands.size() < 2) {
        cout << "need a 2nd argument(number) for process id!" << endl;
        return;
    }

    if (!all_process.empty()) {
        int process_pid = get_p_id(commands);

        if (check_p_id(process_pid)) {
            waitpid(process_pid, nullptr, 0);
            all_process.erase(process_pid);
        } else {
            cout << "process id not found" << endl;
        }

    } else {
        cout << "no process running" << endl;
    }
}

bool get_is_background(vector<char *> commands) {
    if (commands.size() > 1) {
        char *my_arg = commands[commands.size()-1];
        return (strcmp(my_arg, "&") == 0); 
    } else {
        return false;
    }
}

vector<char *> process_line(string cLine) {
    string s;
    vector<string> command;
    istringstream iss(cLine);
    vector<char *> charCommand = vector<char *>();

    if (cLine.length() > LINE_LENGTH) {
        cout << "Input longer than allowed characters!";
        return charCommand;
    }

    // remove trailing white space
    cLine.erase(cLine.find_last_not_of(" \n\r\t\f\v")+1); 
    
    // convert to vector<string>
    while (iss >> s) {
        command.push_back(s);
    }

    // convert to vector<char> so no other conversion is needed for c functions
    for (const auto &word : command) {
        char *charWord = new char[word.size()+1];
        strcpy(charWord, word.c_str());
        charCommand.push_back(charWord);
    }

    // for (const auto &word : charCommand) {
    //     cout << word;
    // }
    return charCommand;
}


void process_command(string cLine) {

    vector<char *> args = process_line(cLine);
    string command = args[0];
    bool is_background = get_is_background(args);

    if (args.empty()) {
        return;
    }

    if (command == "sleep") {
        to_sleep(args);
    } else if (command == "kill") {
        to_kill(args);
    } else if (command == "exit") {
        to_exit();
    } else if (command == "jobs") {
        show_jobs();
    } else if (command == "resume") {
        resume_process(args);
    } else if (command == "suspend") {
        suspend_process(args);
    } else if (command == "wait") {
        to_wait(args);
    } else { // starting a new process
        start_new_process(args, cLine, is_background);
    }
    return;
}