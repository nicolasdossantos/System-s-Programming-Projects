#include "functions.h"

using namespace std;


int main(int argc, char **argv) {

    //Initiate necessary variables
    string input;
    vector <string> list;
    char *buffer = (char *) calloc(200, sizeof(char));
    getcwd(buffer, 200);

    //Get Project's directory
    string projectPath = buffer;

    //#Test#
    //Check if project path is right
    //cout << projectPath << endl;

    if (argc == 2 && strcmp(argv[1], "batchfile") == 0) {
        batchExec(argv[1], projectPath, projectPath);

    } else {

        //Main loop
        while (input != "exit") {



            //get working directory
            string currentWD = getcwd(buffer, 200);

            //Shell Prompt
            //cout << "Nick's Shell> ";
            cout << currentWD << "> ";
            getline(cin, input);

            input = trim(input);

            //#Test#
            //Checks if trim and input works
            //cout << input << endl;

            //returns a vector with input separated in
            list = split(input, " ");

            //#Test#
            //Test if split works
            //for(int i = 0; i < list.size(); i++){
            //  cout << list[i] << endl;

            //Takes care of empty entries and makes sure "exit" is not sent to command handler
            if (!input.empty() && input != "exit") {
                commandhandler(list, projectPath, currentWD);
            }
            //Clear list for next loop
            list.clear();

            //#Test#
            //Test if list is emptied
            //cout << list.empty() << endl;
        }

        cout << "Exiting shell... Thank you for choosing Nick's shell" << endl;


        return 0;
    }
}

