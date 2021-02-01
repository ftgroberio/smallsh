/*******************************************************************************
 * CS344 - Fall 2020
 * Program name: smallsh - assignment 03
 * Author: Felipe Groberio <teixeirf@oregonstate.edu>
 * Date: November 3rd, 2020
 ******************************************************************************/
#include "shellCommands.h"

int main() {
    // Install handlers for SIGINT and SIGTSTP
    installSignals();
    while (1) {
        // Check if any of the background processes have ended
        checkBackgroundProcess();
        // Capture user input and create command linked list
        struct userCommand *head = getUserInput();

        // Process a comment command
        if (head->argmt[0] == '#' || head->argmt[0] == '\n' ||
            head->argmt == NULL) {
        }
        // Process a change of directory built in command
        else if (strcmp(head->argmt, "cd") == 0) {
            built_in_cd(head);
        }
        // Process the built in status command and report exit
        // status of last process
        else if (strcmp(head->argmt, "status") == 0) {
            built_in_status();
        }
        // Process built in exit command, terminate all child processes and
        // terminate smallsh
        else if (strcmp(head->argmt, "exit") == 0) {
            built_in_exit(head);
        }
        // If not a built in command, attempt to execute command
        else {
            forkExec(head);
        }
        // Free any used memory
        freeCommandLL(head);
    }
}
