#ifndef LAB2_FUNCTIONS_H
#define LAB2_FUNCTIONS_H

#include <dirent.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <sys/wait.h>
#include <sys/types.h>
#include <cerrno>
#include <unistd.h>
#include <cstdio>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>

using namespace std;

extern char **environ;

vector <string> split(const string &str, const string &delim);

void batchExec(char *file, string projectPath, string &currentWD);

void commandhandler(vector <string> list, string projectPath, string &current);

void clear();

void echo(vector <string> str);

void builtInHandler(vector <string> list, string projectPath, string &current);

void specialHandler(vector <string> list);

void execHandler(vector <string> list);

void dir(string &dirName);

void cd(vector <string> list);

void envi(char **environ);

void myPause(vector <string> list);

void help(vector <string> list, string projectPath, string &current);

string trim(const string &str);

void redirectOne(vector <string> list, int index1, string sc);

void pipeIt(vector <string> list, int index1);

void redirect2(vector <string> list, int index1, int index2, string specialChar1, string specialChar2);


#endif //LAB2_FUNCTIONS_H
