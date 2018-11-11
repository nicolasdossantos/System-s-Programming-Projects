//
// Created by Nicolas dos Santos on 9/29/18.
//

#include "functions.h"

using namespace std;

//-------Handlers-------//

void commandhandler(vector <string> list, string projectPath, string &current) {
    int specialChar = 0;

    //verify if it is a buitin process
    if (list[0] == "clr" || list[0] == "pause" || list[0] == "dir" || list[0] == "cd" || list[0] == "echo" ||
        list[0] == "environ" || list[0] == "help" || list[0] == "dir&" || list[0] == "cd&" || list[0] == "echo&" ||
        list[0] == "environ&" || list[0] == "help&" || list[0] == "pause&") {

        //Call Handler for built In functions
        builtInHandler(list, projectPath, current);

    } else {

        //Check if entry has any redirection or special assignment
        for (int i = 0; i < list.size(); i++) {
            if (list[i] == "<" || list[i] == ">" || list[i] == "|" || list[i] == "&" || list[i] == ">>") {
                specialChar = 1;
            }

        }
        //If specialChar flag is = 1 call specialHandler
        if (specialChar == 1) {
            specialHandler(list);
        } else {
            execHandler(list);
        }
    }
    //Resets special flag
    specialChar = 0;

}

//Directs input to respective buil-in function
void builtInHandler(vector <string> list, string projectPath, string &current) {
    string option = list[0];

    if (option == "clr") {
        clear();

    } else if (option == "echo" || option == "echo&") {
        echo(list);

    } else if (option == "dir&") {


    } else if (option == "dir") {
        dir(current);

    } else if (option == "cd" || option == "cd&") {
        cd(list);

    } else if (option == "environ") {
        envi(environ);

    } else if (option == "environ&") {


    } else if (option == "pause" || option == "pause&") {
        myPause(list);

    } else if (option == "help" || option == "help&") {
        help(list, projectPath, current);

    } else {
        cout << "Something went wrong" << endl;
    }

}

void execHandler(vector <string> list) {

    if (list[0][list[0].size() - 1] == '&') {
        list[0].pop_back();
        //#Test#
        //Check if pop worked
        //cout << list[0] << endl;
    }

    if (!list.empty()) {
        int size = list.size();
        char *args[size];


        pid_t child = fork();

        if (child < 0) {
            //#TEST
            perror("An error occurred");
        }
        if (child > 0) {

        }

        if (child == 0) {

            //#TEST
//        Test if child process is called
//        cout << "CHILD HERE" << endl;

            for (int i = 0; i < list.size(); i++) {
                args[i] = (char *) list[i].c_str();
            }
            //#TEST
            //Test to see if args is getting all parameters
//        for (int j = 0; j < size; j++) {
//            cout << args[j] << endl;
//        }

            args[size] = NULL;

            int test = execvp(args[0], args);

            if (test == -1) {
                cout << "Command " << args[0] << " not found!" << endl;
                //KILL CHILD and go back to parent process
                pid_t current = getpid();
                kill(current, SIGTERM);

            }
        }

        waitpid(child, 0, 0);
    } else {
        cout << "Empty command" << endl;
    }
}


void specialHandler(vector <string> list) {

    string specialChar1;
    string specialChar2;
    int count = 0;
    int index1 = 0;
    int index2 = 0;

    //check how many special characters
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == "<" || list[i] == ">" || list[i] == ">>" || list[i] == "|") {
            count++;
            //#TEST
            //cout << "Number of special characters: "<< count << endl;
        }
    }

    //Gets the index and saves special character1
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == "<" || list[i] == ">" || list[i] == ">>" || list[i] == "|") {
            index1 = i;
            specialChar1 = list[i];
            break;
        }
    }

    //#TEST
    //cout << "Index of first special character: " << index1 << endl;
    //cout << "First special character: " << specialChar1 << endl;

    //If there are more than one special Char, get its index and save it
    if (count > 1) {
        for (int i = index1 + 1; i < list.size(); i++) {
            if (list[i] == "<" || list[i] == ">" || list[i] == ">>" || list[i] == "|") {
                index2 = i;
                specialChar2 = list[i];
            }
        }
    }
    //#TEST
    //cout << "Index of second special character: " << index2 << endl;
    //cout << "Second special character: " << specialChar2 << endl;


    if (count == 1 && (specialChar1 == ">" || specialChar1 == "<" || specialChar1 == ">>")) {
        //redirect with 1 operation (list, index1, specialChar1)
        //#TEST
        //cout << "1 special char < > >>" << endl;
        redirectOne(list, index1, specialChar1);


    } else if (count == 1 && specialChar1 == "|") {
        //#TEST
        //pipeit (list, index1)
        // cout << "1 special char |" << endl;
        pipeIt(list, index1);

    } else if (count == 2 && (specialChar1 == ">" || specialChar1 == "<" || specialChar1 == ">>") &&
               (specialChar2 == ">" || specialChar2 == "<" || specialChar2 == ">>")) {
        //#TEST
        //redirect with 2 operations (list, index1, index 2, special character 1, special character 2)
        //cout << "2 special char < > >>" << endl;
        redirect2(list, index1, index2, specialChar1, specialChar2);

    } else {
        cout << "This operation is not supported" << endl;
    }


}

//------Built-in Functions-------//


void clear() {
    cout << "\e[2J\e[H";
}

void echo(vector <string> str) {

    //Checks for background execution
    if (str[0][str[0].size() - 1] == '&') {
        return;
    }
    //Echoes content on prompt
    for (int i = 1; i < str.size(); i++) {
        cout << str[i] << " ";
    }
    //Empty line
    puts("");

}

void dir(string &directory) {

    //Create a DIR pointer to opened directory
    DIR *dirPointer = opendir((char *) directory.c_str());

    if (dirPointer) {
        struct dirent *dp = NULL;

        //Reads drectories content
        while ((dp = readdir(dirPointer)) != NULL) {
            string file = dp->d_name;

            //If hidden file dont show
            if (file[0] == '.') {    // skip these
                continue;
            }

            //#TEST
            //Prints content
            cout << file << endl;

        }
        //Closes directory
        closedir(dirPointer);
    }
}

void cd(vector <string> list) {

    //Redirects to HOME if cd is empty
    if (list[1].empty()) {
        char *home = getenv("HOME");
        chdir(home);

    } else {
        chdir((char *) (list[1].c_str()));
    }

    char *w = (char *) list[0].c_str();

    //Checks for background execution
    if (list[0][list[0].size() - 1] != '&') {
        cout << getcwd(w, 200) << endl;
    }


}

void envi(char **environ) {

    for (int i = 0; environ[i] != NULL; i++) {
        cout << environ[i] << endl;
    }

}

void myPause(vector <string> list) {

    if (list[0][list[0].size() - 1] != '&') {
        cout << "Press Return to Continue..." << endl;
    }
    while (getchar() != '\n');
}


void help(vector <string> list, string projectPath, string &currentPath) {
    //#TEST
    //cout << projectPath << endl;
    vector <string> project;
    project.push_back("cd");
    project.push_back(projectPath);


    vector <string> cwd;
    cwd.push_back("cd");
    cwd.push_back(currentPath);

    //#TEST Print cwd vector
    //cout << cwd[0] << endl;
    //cout << cwd[1] << endl;

    //Get to project folder
    cd(project);

    vector <string> helP;
    helP.push_back("more");
    helP.push_back("readme");

    //#TEST Print helP vector
    //cout << helP[0] << endl;
    //cout << helP[1] << endl;

    execHandler(helP);


    //Goes back to previous directory
    cd(cwd);


}

string trim(const string &str) {

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
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

vector <string> split(const string &str, const string &delim) {

    vector <string> tokens;

    size_t prev = 0, pos = 0;

    do {
        pos = str.find(delim, prev);
        if (pos == string::npos) {
            pos = str.length();
        }
        string token = str.substr(prev, pos - prev);

        if (!token.empty()) {
            tokens.push_back(token);
        }

        prev = pos + delim.length();

    } while (pos < str.length() && prev < str.length());

    //#TEST Print tokens vector
    //for(int i = 0; i < tokens.size(); i++){
    //  cout << tokens[i] << endl;
    //


    return tokens;
}

void redirectOne(vector <string> list, int index1, string sc) {

    //Initiate variables
    char *filename;
    int fd;

    //Populate left side vector
    vector <string> left;
    for (int i = 0; i < index1; i++) {
        left.push_back(list[i]);
        //#TEST to see if it's getting populated
        //cout << left[i] << endl;
    }

    //Populate right side vector
    vector <string> right;
    for (int j = index1 + 1; j < list.size(); j++) {
        right.push_back(list[j]);
    }

    filename = (char *) right[0].c_str();

    //#TEST
    //cout << filename << endl;

    if (sc == "<") {
        if ((fd = open(filename, O_RDONLY)) == -1) {
            cout << "File not found" << endl;
        }
        int readHold = dup(0);
        dup2(fd, STDIN_FILENO);
        execHandler(left);
        dup2(readHold, 0);
        close(fd);


    } else if (sc == ">") {
        if ((fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU)) == -1) {
            cout << "File not found" << endl;
            exit(1);
        }
        int holder = dup(1);
        dup2(fd, 1);
        execHandler(left);
        dup2(holder, 1);
        close(fd);

    } else if (sc == ">>") {
        if ((fd = open(filename, O_CREAT | O_WRONLY | O_APPEND, S_IRWXU)) == -1) {
            cout << "File not found" << endl;
            exit(1);
        }
        int holder2 = dup(1);
        dup2(fd, 1);
        execHandler(left);
        dup2(holder2, 1);
        close(fd);
    }

}

void pipeIt(vector <string> list, int index1) {

    int fd[2];


    //Populate left side vector
    vector <string> left;
    for (int i = 0; i < index1; i++) {
        left.push_back(list[i]);
        //#TEST
        //cout << left[i] << endl;
    }

    //Populate right side vector
    vector <string> right;
    for (int j = index1 + 1; j < list.size(); j++) {
        right.push_back(list[j]);
    }

    if (pipe(fd) == 0) {
        //If child
        if (fork() == 0) {
            //close out stdout end
            close(1);
            //connects stdout to write end of the pipe
            dup2(fd[1], 1);
            //closes stdin
            close(fd[0]);
            //executes the left side of input
            execHandler(left);
            //kill child
            exit(1);

        } else {
            //If parent
            //stores read into hold
            int hold = dup(0);
            //closes stdin
            close(0);
            //connecrs stdin to read end of pipe
            dup2(fd[0], 0);
            close(fd[1]);
            //executes right part of input
            execHandler(right);
            //closes the pipe
            close(fd[0]);
            //Reloads old value of stdin
            dup2(hold, 0);

            //Wait for child to finish
            wait(NULL);
        }
    }


}

void redirect2(vector <string> list, int index1, int index2, string specialChar1, string specialChar2) {
    //cout << "REDIRECT2" << endl;
    int fd1;
    int fd2;
    char *filewrite;
    char *fileread;

    //3 vectors for the 3 different parts of the input
    vector <string> instruc;
    vector <string> writer;
    vector <string> reader;

    instruc.push_back(list[0]);

    if (list[index1] == ">") {
        for (int i = index1 + 1; i < index2; i++) {
            writer.push_back(list[i]);
        }
        for (int j = index2 + 1; j < list.size(); j++) {
            reader.push_back(list[j]);
        }

    } else if (list[index1] == "<") {
        for (int i = index1 + 1; i < index2; i++) {
            reader.push_back(list[i]);
        }
        for (int j = index2 + 1; j < list.size(); j++) {
            writer.push_back(list[j]);
        }
    }

    filewrite = (char *) writer[0].c_str();
    fileread = (char *) reader[0].c_str();

    //#TEST
    if ((fd1 = open(filewrite, O_CREAT | O_WRONLY | O_APPEND, S_IRWXU)) == -1) {
        cout << "File not found" << endl;
        exit(1);
    }
    //stores stdout fd
    int writeHold = dup(1);
    dup2(fd1, 1);

    //#TEST
    if ((fd2 = open(fileread, O_RDONLY)) == -1) {
        cout << "File not found" << endl;
        exit(1);
    }
    //stores stdin fd
    int readHold = dup(0);
    dup2(fd2, 0);

    execHandler(instruc);

    dup2(readHold, 0);
    dup2(writeHold, 1);

    close(fd1);
    close(fd2);


}

void batchExec(char *file, string projectPath, string &currentWD) {


    ifstream input(file);

//Shell Prompt
    cout << "Nick's Shell> ";

    for (string line; getline(input, line);) {
        if (!line.empty()) {
            line = trim(line);

            //returns a vector with input separated in
            vector <string> list = split(line, " ");

            pid_t id = -1;
            //if background execution
            if (list[0][list[0].size() - 1] == '&') {
                //create new process
                id = fork();
                //if child
                if (id == 0) {
                    //remove &
                    list[0].pop_back();
                }
                if (list[0] == "exit") {
                    cout << "Exiting..." << endl;
                }
            }
            //execute background exexution
            if (id == 0) {
                commandhandler(list, projectPath, currentWD);
            }
            //kill child
            if (id == 0) {
                pid_t current = getpid();
                kill(current, SIGTERM);
            }


            //Takes care of empty entries and makes sure "exit" is not sent to command handler

            if (id != 0) {
                if (!line.empty() && line != "exit") {
                    commandhandler(list, projectPath, currentWD);
                }
            }
            //Clear list for next loop
            list.clear();
        }

    }
}
