/*******************************************************************************
 * CS344 - Fall 2020
 * Program name: smallsh - assignment 03
 * Author: Felipe Groberio <teixeirf@oregonstate.edu>
 * Date: November 3rd, 2020
 ******************************************************************************/
#include "commandLL.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

int foregroundFlag;
pid_t pidArray[2048];
int status;

void checkBackgroundProcess();

void installSignals();

void handle_SIGINT(int signo);

void handle_SIGTSTP(int signo);

void built_in_status();

void built_in_cd(struct userCommand *head);

void built_in_exit(struct userCommand *head);

void forkExec(struct userCommand *head);
