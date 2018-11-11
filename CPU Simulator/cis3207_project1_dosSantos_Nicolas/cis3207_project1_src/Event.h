#ifndef LAB1C_EVENT_H
#define LAB1C_EVENT_H
#include <string>
#endif //LAB1C_EVENT_H
using namespace std;


class Event {
public:
    int time;
    int jobSeq;
    int type;


    Event(int timeOb, int typeOb, int jobSeqOb){
        time = timeOb;
        type = typeOb;
        jobSeq = jobSeqOb;


    }

};



