#include <sys/types.h>   // for fork, wait
#include <sys/wait.h>    // for wait
#include <unistd.h>      // for fork, pipe, dup, close
#include <stdio.h>       // for NULL, perror
#include <stdlib.h>      // for exit
#include <iostream>      // for cout
using namespace std;

int main(int argc, char** argv) {
    int fds[2][2];
    int pid;

    // create a pipe using fds[0] for child <-> grandChild
    if (pipe(fds[0]) < 0)
    {
        perror("Pipe Failed");
    }
    // crate a pipe using fds[1] for grandChild <-> greatGrandChild
    if (pipe(fds[1]) < 0)
    {
        perror("Pipe Failed");
    }

    // make sure there is an arguement passed
    if (argc != 2) {
        cerr << "Usage: processes command" << endl;
        exit(-1);
    }

    // fork a child
    pid = fork();
    if (pid < 0)
    {
        perror("Fork Failed");
    }

    // if I'm a child
    else if (pid == 0) {
        // fork a grand-child
        pid = fork();
        if (pid < 0)
        {
            perror("Fork Failed");
        }
        // if I'm a grand-child
        else if (pid == 0)
        {
            // fork a great-grand-child
            pid = fork();
            if (pid < 0)
            {
                perror("Fork Failed");
            }
            // if I'm a great-grand-child
            else if (pid == 0)
            {
                // close grandChild <-> greatGrandChild pipe's read and write
                // close child <-> grandChild pipe's read
                // wrie to child <-> grandChild pipe's write
                close(fds[1][0]);
                close(fds[1][1]);
                close(fds[0][0]);
                dup2(fds[0][1], STDOUT_FILENO);


                // execute "ps"
                execlp("ps", "ps", "-A", NULL);
            }
            // else if I'm a grand-child
            else if (pid > 0)
            {
                // close child <-> grandChild pipe's write
                // read from child <-> grandChild pipe's read
                close(fds[0][1]);
                dup2(fds[0][0], STDIN_FILENO);

                // close grandChild <-> greatGrandChild pipe's read
                // write to grandChild <-> greatGrandChild pipe's write
                close(fds[1][0]);
                dup2(fds[1][1], STDOUT_FILENO);

                // execute "grep"
                execlp("grep", "grep", argv[1], NULL);
            }
        }
        // else if I'm a child
        else if (pid > 0)
        {
            // close child <-> grandChild pipe's read
            // close child <-> grandChild write
            // close grandChild <-> greatGrandChild pipes' write
            // read from child <-> grandChild pipe's read
            close(fds[0][0]);
            close(fds[0][1]);
            close(fds[1][1]);
            dup2(fds[1][0], STDIN_FILENO);

            // execute "wc"
            execlp("wc", "wc", "-l", NULL);
        }
    }
    else
    {
        // I'm a parent

        // close every pipe
        close(fds[0][0]);
        close(fds[0][1]);
        close(fds[1][0]);
        close(fds[1][1]);


        wait(NULL);
        cout << "commands completed" << endl;
    }
}