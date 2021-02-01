/*******************************************************************************
 * CS344 - Fall 2020
 * Program name: smallsh - assignment 03
 * Author: Felipe Groberio <teixeirf@oregonstate.edu>
 * Date: November 3rd, 2020
 ******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* this struct is used to store a linked list of strings. Each node
 * is a word, and the whole linked list represents one command.
 */
struct userCommand {
    char *argmt;
    struct userCommand *next;
};

struct userCommand *getUserInput();

void appendPid(char *uI);

int getListSize(struct userCommand *head);

struct userCommand *getListTail(struct userCommand *head);

void freeCommandLL(struct userCommand *list);
