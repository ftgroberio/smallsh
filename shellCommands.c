/*******************************************************************************
 * CS344 - Fall 2020
 * Program name: smallsh - assignment 03
 * Author: Felipe Groberio <teixeirf@oregonstate.edu>
 * Date: November 3rd, 2020
 ******************************************************************************/
#include "shellCommands.h"

// Manages the status of the last terminated process
int status = 0;
// Controls how smallsh manages background or foreground only processes
int foregroundFlag = 0;
// An array to store pids
pid_t pidArray[2048];

/* This function checks if any background pid has terminated. If a pid
 * is ready, announce that a background pid has terminated print out the
 * exit status of the background process
 */
void checkBackgroundProcess() {
    int backgroundPid = waitpid(-1, &status, WNOHANG);
    // check for multiple background pids until waitpid returns -1
    while (backgroundPid > 0) {
        printf("background pid %d is done: ", backgroundPid);
        built_in_status();
        backgroundPid = waitpid(-1, &status, WNOHANG);
    }
}

/* When SIGINT is raised, this function will wait for a background pid if
 * any unfinished pids exists
 */
void handle_SIGINT(int signo) {
    write(STDOUT_FILENO, "\n", 1);
    int sigpid = waitpid(-1, &status, WNOHANG);
    // If any process was caught, return it's exit status
    if (sigpid > 0) {
        built_in_status();
    }
    // If any pids are still runnig, wait for it to finish
    else if (sigpid == 0) {
        waitpid(-1, &status, 0);
        // return process exit status
        built_in_status();
    }
}

/* This is a handle for the SIGTSTP flag that changes how the smallsh
 * handles foreground and background processes. By raising a SIGTSTP,
 * it toggles the foregroundFlag and switches how the program hadles
 * the background character '&'.
 */
void handle_SIGTSTP(int signo) {
    // wait for any process to end
    wait(NULL);
    // toggle foregroundFlag to true
    if (foregroundFlag) {
        write(STDOUT_FILENO, "\nExiting foreground-only mode\n", 30);
        foregroundFlag = 0;
    }
    // toggle foregroundFlag to false
    else {
        write(STDOUT_FILENO,
              "\nEntering foreground-only mode (& is now ignored)\n", 50);
        foregroundFlag = 1;
    }
}

/* smallsh has custom behavior for SIGINT and SIGTSTP. This function install
 * the custom signals in the main execution loop
 */
void installSignals() {
    struct sigaction SIGTSTP_action = {{0}};
    SIGTSTP_action.sa_handler = handle_SIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = 0;
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    struct sigaction SIGINT_action = {{0}};
    SIGINT_action.sa_handler = handle_SIGINT;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);
}

/* This is a built in command function that reports the exit status of
 * the last process. By default, if no proces has been called it, the
 * exit status is 0
 */
void built_in_status() {
    if (WIFEXITED(status)) {
        printf("exit value %d\n", WEXITSTATUS(status));
        fflush(stdout);
    } else {
        printf("terminated by signal %d\n", WTERMSIG(status));
        fflush(stdout);
    }
}

/* This function handles a built in command function that allows the
 * user to change the working directory of smallsh
 */
void built_in_cd(struct userCommand *head) {
    char *path = NULL;
    int ch;
    // If a path exists, change directory
    if (head->next) {
        path = head->next->argmt;
        ch = chdir(path);

    }
    /* If no given path is provided, get the "HOME" environment variable
     * and change the working directory to the home path
     */
    else {
        ch = chdir(getenv("HOME"));
    }
    // Check if the provided path is a valid path
    if (ch < 0) {
        printf("%s: no such file or directory\n", path);
    }
}

/* This function therminates all the child processes before exiting the
 * parent processing and terminating smallsh
 */
void built_in_exit(struct userCommand *head) {
    for (int i = 0; pidArray[i] != 0; i++) {
        kill(pidArray[i], SIGKILL);
    }
    // free any memory associated with userCommand linked list
    freeCommandLL(head);
    exit(0);
}

void forkExec(struct userCommand *head) {
    pid_t pid;
    pid = fork();

    // Append new child pid to global pidArray
    int i = 0;
    for (; pidArray[i] != 0; i++) {
    }
    pidArray[i] = pid;

    // Terminate smallsh in case fork is unsuccessful
    if (pid == -1) {
        printf("Unable to fork, smallsh terminating..\n");
        exit(EXIT_FAILURE);
    }
    // Child processing
    else if (pid == 0) {
        // Ignore SIGTSTP during child execution
        signal(SIGTSTP, SIG_IGN);
        // Allocate memory for the argument array
        char **arguments = malloc((getListSize(head) + 1) * sizeof(char *));
        struct userCommand *temp = head;
        // Assign flags for to track if a file descriptor is unsuccessful
        bool badFileFlag = false;
        int inputFile = -1;
        int outputFile = -1;
        // i is the number of arguments in the argument array
        int i = 0;
        // Transverse through command line
        while (temp) {
            // Redirect input
            if (strcmp(temp->argmt, "<") == 0) {
                inputFile = open(temp->next->argmt, O_RDONLY);
                if (inputFile == -1) {
                    printf("cannot open %s for input\n", temp->next->argmt);
                    badFileFlag = true;
                } else {
                    dup2(inputFile, 0);
                    close(inputFile);
                }
                temp = temp->next;
            }
            // Redirect output
            else if (strcmp(temp->argmt, ">") == 0) {
                outputFile = open(temp->next->argmt, O_RDWR | O_CREAT, 0640);
                if (outputFile == -1) {
                    printf("cannot open %s for output\n", temp->next->argmt);
                    badFileFlag = true;
                } else {
                    dup2(outputFile, 1);
                    dup2(outputFile, 2);
                    close(outputFile);
                }
                temp = temp->next;
            }
            // Change the standard output if the child is a background process
            else if (strcmp(temp->argmt, "&") == 0 &&
                     temp == getListTail(head)) {
                signal(SIGINT, SIG_IGN);
                if (foregroundFlag == 0) {
                    if (inputFile == -1) {
                        dup2(open("/dev/null", 0), 0);
                    }
                    if (outputFile == -1) {
                        dup2(open("/dev/null", O_WRONLY), 1);
                    }
                }
            }
            // Accumulate arguments
            else {
                arguments[i] = temp->argmt;
                i++;
            }
            temp = temp->next;
        }
        // Terminate the argument array with a NULL element
        arguments[i] = NULL;
        int childReturn = 0;
        if (!badFileFlag) {
            // Attempt to run command
            childReturn = execvp(arguments[0], arguments);
            printf("%s: no such file or directory\n", head->argmt);
        }
        // If child exec() fails, clear the memory and retun the exit status
        free(arguments);
        freeCommandLL(head);
        exit(childReturn);
    }

    /* Announce child is a background process if background character is present
     * and the foregroundFlag only is not true
     */
    if (strcmp(getListTail(head)->argmt, "&") == 0 && foregroundFlag == 0) {
        printf("background pid is %d\n", pid);

    } else {
        // Wait for a foreground child
        waitpid(pid, &status, 0);
    }
}
