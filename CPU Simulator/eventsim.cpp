#include <fstream>
#include "headers.h"
#include <cstring>
#include <cmath>
#include <iostream>


using namespace std;


//Custom comparison type for priority queue
struct timeComp {
    bool operator()(Event const &e1, Event const &e2) {
        // return "true" if "e2" is ordered before "e1" (Min heap)
        return e1.time > e2.time;
    }
};


//TODO: Collect necessary data

//Function declaration
void process_CPU(Event currentJob, priority_queue<Event, vector<Event>, timeComp> *eventQueue, queue<Event> *cpu);

void process_Disk(Event current_job, priority_queue<Event, vector<Event>, timeComp> *eventQueue, queue<Event> *diskI,
                  queue<Event> *diskII, queue<Event> *cpu);


//Global Variables

int generalTime = 0;
int lines = 0;
bool cpu_idle = true;
bool disk1_idle = true;
bool disk2_idle = true;
int jobs = 0;
int jobQueueMax = 0;
int cpuQueueMax = 0;
int diskIQueueMax = 0;
int diskIIQueueMax = 0;

vector<int> temp;


int main() {

    freopen("output.txt", "w", stdout);
    //Read from file
    FILE *file = fopen("textConfig.txt", "r");
    if (!file) {
        printf("Could not open file. Exiting application.");
        return 1;
    }
    //Read file and store numbers on vector
    temp = readFromFile(file);


    //Seed for random number generator
    seedRan(SEED);

    //Initial Events to be pushed into jobQueue
    Event sim_start(0, SIM_START, 0);
    Event job1(0, CPU_ARRIVAL, 1);
    Event sim_end(FIN_TIME, SIM_END, 0);


    //Data Structures
    priority_queue<Event, vector<Event>, timeComp> jobQueue;
    queue<Event> diskI;
    queue<Event> diskII;
    queue<Event> cpu;
    vector<int> done;

    //Push initial jobs into queue
    jobQueue.push(sim_start);
    jobQueue.push(job1);
    jobQueue.push(sim_end);


    while (!jobQueue.empty() && generalTime < FIN_TIME) {

        if (jobQueue.size() > jobQueueMax) {
            jobQueueMax = jobQueue.size();
        }
        if (cpu.size() > cpuQueueMax) {
            cpuQueueMax = cpu.size();
        }
        if (diskI.size() > diskIQueueMax) {
            diskIQueueMax = diskI.size();
        }
        if (diskII.size() > diskIIQueueMax) {
            diskIIQueueMax = diskII.size();
        }

        Event current_event = jobQueue.top();
        jobQueue.pop();
        string typeString;
        generalTime = current_event.time;

        if (current_event.jobSeq > jobs) {
            jobs = current_event.jobSeq;
        }


        //Assign enum to String
        switch (current_event.type) {

            case 0:
                typeString = "indicates that simulation has ended";
                break;

            case 1:
                typeString = "arrives at CPU";
                break;

            case 2:
                typeString = "finishes at CPU";
                break;

            case 3:
                typeString = "arrives at Disk";
                break;

            case 4:
                typeString = "finishes IO at disk I";
                break;

            case 5:
                typeString = "finishes IO at disk II";
                break;

            case 6:
                typeString = "exited";
                break;

            case 7:
                typeString = "indicates that simulation has ended";
                break;

            default:
                typeString = "Error: No ";
        }

        //Iterator to check if job has exited. If so, do not display events involving such job
        vector<int>::iterator found;
        found = find(done.begin(), done.end(), current_event.jobSeq);

        if (found != done.end()) {
            //DO NOTHING

        } else {
            //Print to console -> setw: sets width
            cout << "At time " << setw(4) << current_event.time << " job " << setw(4) << current_event.jobSeq << " "
                 << typeString << endl;

            //Count processes
            lines++;
        }

        //Decides where to send the task to
        switch (current_event.type) {

            case CPU_ARRIVAL:
                process_CPU(current_event, &jobQueue, &cpu);
                break;

            case FINISHED:
                process_CPU(current_event, &jobQueue, &cpu);
                break;

            case DISK_ARRIVAL:
                process_Disk(current_event, &jobQueue, &diskI, &diskII, &cpu);
                break;

            case DISK_I_FINISH:
                process_Disk(current_event, &jobQueue, &diskI, &diskII, &cpu);
                break;

            case DISK_II_FINISH:
                process_Disk(current_event, &jobQueue, &diskI, &diskII, &cpu);
                break;

            case EXIT:
                done.push_back(current_event.jobSeq);
                break;

            case SIM_END:
                process_exit();
                break;

            case SIM_START:
                break;

            default:
                puts("Error: Event type could not be found");
                break;
        }

    }

    cout << "Statistics:" << endl;
    cout << "===============================================" << endl;
    cout << "Number of events(throughput): " << lines << endl;
    cout << "Number of Jobs: " << jobs << endl;
    cout << "Max size reached by Job Queue: " << jobQueueMax << endl;
    cout << "Max size reached by CPU Queue: " << cpuQueueMax << endl;
    cout << "Max size reached by Disk I Queue: " << diskIQueueMax << endl;
    cout << "Max size reached by Disk II Queue: " << diskIIQueueMax << endl;
    //cout << "Average time a job spent in CPU: " << cpu_average_time() << endl;


}

//CPU Handler
void process_CPU(Event currentJob, priority_queue<Event, vector<Event>, timeComp> *eventQueue, queue<Event> *cpu) {

    if (currentJob.type == CPU_ARRIVAL) {

        Event next_job(generalTime + get_random(ARRIVE_MIN, ARRIVE_MAX), CPU_ARRIVAL, currentJob.jobSeq + 1);

        eventQueue->push(next_job);

        fifo_in(cpu, currentJob);


    } else if (currentJob.type == FINISHED) {
        cpu_idle = true;

        if (!quitProb(QUIT_PROB)) {

            Event disk_task(generalTime, DISK_ARRIVAL, currentJob.jobSeq);

            eventQueue->push(disk_task);

        } else {
            Event exitEvent(generalTime, EXIT, currentJob.jobSeq);
            eventQueue->push(exitEvent);

        }
    }

    if ((!cpu->empty()) && cpu_idle) {
        Event process = cpu->front();

        cpu->pop();
        Event job_finis(generalTime + get_random(CPU_MIN, CPU_MAX), FINISHED, process.jobSeq);

        eventQueue->push(job_finis);

        cpu_idle = false;

    }
}

void process_Disk(Event current_job, priority_queue<Event, vector<Event>, timeComp> *eventQueue, queue<Event> *diskI,
                  queue<Event> *diskII, queue<Event> *cpu) {


    if (current_job.type == DISK_ARRIVAL) {

        //Send job to to least full queue -> If they are equal random to decide
        if (diskI->size() < diskII->size()) {

            fifo_in(diskI, current_job);

            if ((!diskI->empty()) && disk1_idle) {
                Event process = diskI->front();

                Event job_finis_Disk(generalTime + get_random(DISK_I_MIN, DISK_I_MAX), DISK_I_FINISH, process.jobSeq);

                eventQueue->push(job_finis_Disk);

                disk1_idle = false;


            }


        } else {
            fifo_in(diskII, current_job);

            if ((!diskII->empty()) && disk2_idle) {
                Event process = diskII->front();

                Event job_finis_DiskII(generalTime + get_random(DISK_II_MIN, DISK_II_MAX), DISK_II_FINISH,
                                       process.jobSeq);

                eventQueue->push(job_finis_DiskII);

                disk2_idle = false;

            }

        }

    } else {

        if ((!diskI->empty()) && (diskI->front().jobSeq == current_job.jobSeq)) {

            Event topOfQueue = diskI->front();
            diskI->pop();

            Event backToCpu(generalTime, CPU_ARRIVAL, topOfQueue.jobSeq);

            disk1_idle = true;

            fifo_in(cpu, backToCpu);


        } else if (diskII->front().jobSeq == current_job.jobSeq) {

            Event topOfQueue = diskII->front();
            diskII->pop();

            Event backToCpuII(generalTime, CPU_ARRIVAL, topOfQueue.jobSeq);


            fifo_in(cpu, backToCpuII);
            disk2_idle = true;

        } else {
            puts("Error: Event not found");
        }

    }

}





