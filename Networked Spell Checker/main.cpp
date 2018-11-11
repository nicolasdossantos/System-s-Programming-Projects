#include<stdio.h>
#include<arpa/inet.h>
#include <map>
#include <string>
#include <fstream>
#include <zconf.h>
#include <iostream>
#include <queue>
#include <slapi-plugin.h>


#define WORKER_COUNT 2
#define MAXSIZE 10

pthread_mutex_t mainMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notFull = PTHREAD_COND_INITIALIZER;
pthread_cond_t notEmpty = PTHREAD_COND_INITIALIZER;

pthread_mutex_t logMutex = PTHREAD_COND_INITIALIZER;
pthread_cond_t logNotFull = PTHREAD_COND_INITIALIZER;
pthread_cond_t logNotEmpty = PTHREAD_COND_INITIALIZER;


//Data Structures
std::map<std::string, int> dictionary;
std::queue<std::string> log;
std::queue<int> jobQueue;


//Functions
void *connection_handler(void *);

void *loggerFunction(void *);

std::string trim(const std::string &str);


int main(int argc, char *argv[]) {

    //Open and clear log file
    std::ofstream clear;
    clear.open("log.txt", std::ofstream::out | std::ofstream::trunc);

    //Variables
    pthread_t threadPool[WORKER_COUNT];
    int socket_desc;
    int new_socket;
    int c;
    int optval = 1;
    struct sockaddr_in server, client;
    std::string fileName;
    int connectionPort;

    //Initialize Threads
    for (int i = 0; i < WORKER_COUNT; i++) {
        pthread_create(&threadPool[i], NULL, connection_handler, NULL);
    }

    //Logger thread
    pthread_t logger;
    pthread_create(&logger, NULL, loggerFunction, NULL);


    //Implementation of dynamic port and dictionary
    if (argc > 1) {
        connectionPort = atoi(argv[1]);
    } else {
        connectionPort = 8888;
    }
    if (connectionPort < 1024 || connectionPort > 65535) {
        puts("Port number should not be smaller than 1024 or bigger than 65535. Try again.");
        return -1;
    }
    if (argc > 2) {
        fileName = argv[2];
    } else {
        fileName = "words.txt";
    }

    //Populate dictionary from selected file
    std::ifstream file(fileName);
    std::string str;

    while (getline(file, str)) {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        dictionary.insert(std::pair<std::string, int>(str, 1));
    }

    //#Test if dictionary was populated correctly
    //std::cout << dictionary["nicolas"] << std::endl;

    //Create listening socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
    }

    //Prevents Binding error
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR,
                   (const void *) &optval, sizeof(int)) < 0) {
        return -1;
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(connectionPort);
    //Bind
    if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
        puts("bind failed");
        return 1;
    }

    //Listen
    listen(socket_desc, 3);


    //Shows port for incoming connections and file that populates dictionary
    puts("Waiting for incoming connections...");
    std::cout << "Connected to Port " << connectionPort << std::endl;
    std::cout << "Reading from file " << fileName << std::endl;


    //Infinite loop
    while (true) {
        c = sizeof(struct sockaddr_in);
        //Create socket for incoming connection
        new_socket = accept(socket_desc, (struct sockaddr *) &client, (socklen_t *) &c);

        if (new_socket < 0) {
            perror("Error: Accepted Fail");
            return 1;
        }

        //#Test
        //std::cout<<new_socket<<std::endl;


        //Check if job queue is not full before adding a new job to it
        int test = pthread_mutex_lock(&mainMutex);

        //#Test
        //std::cout<<"LOCK: "<<test<<std::endl;


        while (jobQueue.size() >= MAXSIZE) {
            pthread_cond_wait(&notFull, &mainMutex);
            //#Test
            //puts("FULL");

        }

        char *message = const_cast<char *>("Awaiting for worker...");
        write(new_socket, message, strlen(message));
        jobQueue.push(new_socket);

        //#Test
        //std::cout<<"TOP OF QUEUE:"<<jobQueue.front()<<std::endl;

        int unlock = pthread_mutex_unlock(&mainMutex);
        pthread_cond_signal(&notEmpty);
        //#Test
        //std::cout<<"UNLOCK: "<<test<<std::endl;


        //#Test -> job queue
//        for(i = 0; i< jobQueue.size(); i++){
//            std::cout<<jobQueue.front() << std::endl;
//            jobQueue.pop();
//        }

    }
}



void *connection_handler(void *) {

    //Lock
    int i = pthread_mutex_lock(&mainMutex);
    //#Test
    //std::cout<<i<<std::endl;

    while (jobQueue.empty()) {
        pthread_cond_wait(&notEmpty, &mainMutex);
        //#Test
        //puts("EMPTY");
    }

    //Get Socket descriptor from queue
    int sock = jobQueue.front();
    jobQueue.pop();
    puts("Connection accepted!");

    //Unlock and signal
    pthread_mutex_unlock(&mainMutex);
    pthread_cond_signal(&notFull);


    //Get the socket descriptor
    ssize_t read_size;
    char *message, clientmessage[2000];


    //#Test
    //std::cout<<"Socket Descriptor being passed: "<< sock<<std::endl;
    //std::cout<<"SIZE:"<<jobQueue.size()<<std::endl;


    //Send Welcome Message to Client
    char *welcomeMessage = const_cast<char *>("\nConnected!\nWelcome to Nick's Spell Checker. Enter a word:\n");
    write(sock, welcomeMessage, strlen(welcomeMessage));


    //receive message
    while ((read_size = recv(sock, clientmessage, 2000, 0)) > 0) {

        //Eliminate white spaces and transform to lower case
        std::string clientString = clientmessage;
        transform(clientString.begin(), clientString.end(), clientString.begin(), ::tolower);
        clientString[clientString.size() - 1] = ' ';
        clientString = trim(clientString);

        //#Test
        //std::cout << clientString << std::endl;
        //std::cout << dictionary["nicolas"] << std::endl;

        //Check if word is in dictionary
        if (dictionary[clientString] == 1) {
            message = const_cast<char *>("Spelled Correctly\n");
        } else {
            message = const_cast<char *>("Misspelled\n");

        }


        //Check if job queue is not full before adding a new job to it
        pthread_mutex_lock(&logMutex);
        //string builder to log
        std::string logString =
                "Socket: " + std::to_string(sock) + "\nWord Entered: " + clientString + "\nResult: " + message;
        //Add it to log queue
        log.push(logString);
        //Send result to client
        write(sock, message, strlen(message));
        //Flush the input
        fflush(stdin);

        //clear string
        for (int i = 0; i < 2000; i++) {
            clientmessage[i] = '\0';
        }
    }

    //If log queue is full, it does not add any more logs
    while (log.size() >= MAXSIZE) {
        pthread_cond_wait(&logNotFull, &logMutex);
        //#Test
        //puts("FULL");

    }


    //Test
    //std::cout<<"TOP OF QUEUE:"<<log.front()<<std::endl;

    //Unlock mutex and signal
    pthread_mutex_unlock(&logMutex);
    pthread_cond_signal(&logNotEmpty);

    //If client gets disconnected, worker will look for a new connection
    //flush output
    if (read_size == 0) {
        puts("Client Disconnected");
        fflush(stdout);
        connection_handler(NULL);


    } else if (read_size == -1) {
        puts("Error...");
    }

    return 0;

}

std::string trim(const std::string &str) {

    int isAllSpace = 1;
    //Check if it is not only white space
    for (int i = 0; i < str.size(); i++) {
        if (!isspace(str[i])) {
            isAllSpace = 0;
        }
    }
    //If input is only white space returns empty string
    if (isAllSpace) {
        return "";
    }

    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}


void *loggerFunction(void *) {


    char filename[] = "log.txt";

    //Infinite loop
    while (true) {
        std::fstream appendFileToWorkWith;
        //open file
        appendFileToWorkWith.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);

        //Semaphore to check if queue is empty
        while (log.empty()) {
            pthread_cond_wait(&logNotEmpty, &logMutex);
            //puts("EMPTY");
        }

        appendFileToWorkWith << log.front();

        appendFileToWorkWith << "\n";
        log.pop();

        pthread_cond_signal(&logNotFull);

    }
    return 0;

}