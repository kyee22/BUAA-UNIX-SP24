#include<unistd.h>

int main() {


    char* argument_list[] = {"echo", "\'", NULL}; // NULL terminated array of char* strings
    
    // Ok! Will execute the command "ls -l"
    execvp("echo", argument_list);
    char* argument_list2[] = {"echo", "\""}; // NULL terminated array of char* strings
    execvp("echo", argument_list);

    return 0;
}