
#ifndef LAB1C_HEADERS_H
#define LAB1C_HEADERS_H

#include <iomanip>
#include <string>
#include <iostream>
#include <exception>
#include <string>
#include <typeinfo>
#include <cstdio>
#include <queue>
#include <set>
#include <algorithm>
#include <functional>
#include "Event.h"
#include <utility>

#define SEED temp[0]
#define QUIT_PROB temp[1]
#define INIT_TIME temp[2]
#define FIN_TIME temp[3]
#define ARRIVE_MIN temp[4]
#define ARRIVE_MAX temp[5]
#define CPU_MIN temp[6]
#define CPU_MAX temp[7]
#define DISK_I_MIN temp[8]
#define DISK_I_MAX temp[9]
#define DISK_II_MIN temp[10]
#define DISK_II_MAX temp[11]



#endif //LAB1C_HEADERS_H


enum type{
    SIM_START,
    CPU_ARRIVAL,
    FINISHED,
    DISK_ARRIVAL,
    DISK_I_FINISH,
    DISK_II_FINISH,
    EXIT,
    SIM_END

};

using namespace std;

//Function Headers

int get_random(int min, int max);

void seedRan(int seed);

bool quitProb(int percent);

void process_CPU(Event task);

void process_Disk();

void process_exit();

void fifo_in(queue<Event> *queue, Event event);
vector<int> readFromFile(FILE *file);











