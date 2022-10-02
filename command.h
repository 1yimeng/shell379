#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <csignal>
#include <sstream>
#include <sys/wait.h>
#include <cstring>
#include <sys/resource.h>
#include "process.h"

using namespace std;

#define LINE_LENGTH 100 // Max # of characters in an input line
#define MAX_ARGS 7 // Max number of arguments to a command
#define MAX_LENGTH 20 // Max # of characters in an argument
#define MAX_PT_ENTRIES 32 // Max entries in the Process Table

void process_command(string input);