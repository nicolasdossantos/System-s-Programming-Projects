#include<string>
#include<cstring>
#include "headers.h"
#include <iostream>

using namespace std;

//Seed to generate real random int
void seedRan(int seed) {
    srand(unsigned(time(nullptr)));
}

//Generate random between min and max
int get_random(int min, int max) {
    //Verify if min is smaller than max
    if (max < min) {
        int temp = min;
        min = max;
        max = temp;
    }

    int random = rand() % (max - min) + min;
    return random;
}


//probability generator
bool quitProb(int percent) {
    int random = 0;
    if ((percent < 0) || (percent > 100)) {
        cout << percent << " is not a valid percentage. Enter a number between 0 and 100";

    } else {
        random = get_random(0, 100);
    }
    return random <= percent;
}

void process_exit() {
    puts("All done");
}

void fifo_in(queue<Event> *queue, Event event) {
    queue->push(event);
}

vector<int> readFromFile(FILE *file){
    char line[100];
    char junk[100];

    vector<int> numbers;
    vector<string> text;


    while(!feof(file)) {
        fscanf(file,"%[^ \n\t\r]s",line ); //Get text
        text.push_back(line);


        fscanf(file,"%[ \n\t\r]",junk); //Remove any 'white space' characters
    }
    fclose(file);


    for(int i = 1; i < text.size(); i+= 2){
        numbers.push_back(stoi(text[i]));
    }

    return numbers;


}




