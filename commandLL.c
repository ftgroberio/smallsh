/*******************************************************************************
 * CS344 - Fall 2020
 * Program name: smallsh - assignment 03
 * Author: Felipe Groberio <teixeirf@oregonstate.edu>
 * Date: November 3rd, 2020
 ******************************************************************************/
#include "commandLL.h"

/* getUserInput() process user input, parses the data and creates a linked
 * list of commands. Each string separated by a space is an individual node
 * in the linked list.
 */
struct userCommand *getUserInput() {
    struct userCommand *head = NULL;
    struct userCommand *tail = NULL;
    size_t bufferSize = 2048;
    char *userInput = calloc(bufferSize, sizeof(char *));

    // Capture user input
    printf(": ");
    fflush(stdout);
    int numChars = getline(&userInput, &bufferSize, stdin);

    // If user input is interruped by a signal, disregard user input
    if (numChars == -1) {
        // reset stdin status
        clearerr(stdin);
        struct userCommand *head = malloc(sizeof(struct userCommand));
        head->argmt = calloc(3, sizeof(char));
        strcpy(head->argmt, "#");
        head->next = NULL;
        // Free user input memory and return one empty node
        free(userInput);
        return head;
    }
    strtok(userInput, "\n");

    // If "$$" exists in the string, change it to the process pid
    appendPid(userInput);

    // Check if the provided input is a comment
    if (userInput[0] == '#') {
        head = malloc(sizeof(struct userCommand));
        head->argmt = calloc(strlen(userInput) + 1, sizeof(char));
        head->next = NULL;
        strcpy(head->argmt, userInput);
        // Free user input memory and return the comment string node
        free(userInput);
        return head;
    }

    /* This portion of the function creates a linked list for each string
     * separated by a space in the given input.
     */
    char *ptr;
    char *arg;
    for (arg = strtok_r(userInput, " ", &ptr); arg != NULL;
         arg = strtok_r(NULL, " ", &ptr)) {

        // Create a new node
        struct userCommand *temp = malloc(sizeof(struct userCommand));
        temp->argmt = calloc(strlen(arg) + 1, sizeof(char));
        strcpy(temp->argmt, arg);
        // If no nodes exist, create head and tail
        if (head == NULL && tail == NULL) {
            head = temp;
            tail = temp;
            head->next = NULL;
        }
        // If nodes exist, attach new node to linked list
        else {
            tail->next = temp;
            tail = temp;
            tail->next = NULL;
        }
    }
    // Clear user input memory and return the head node of command linked list
    free(userInput);
    return head;
}

/* appendPid(char *) will search a given string for "$$" and replaces it
 * by the process pid
 */
void appendPid(char *uI) {
    // Save the process pid to string
    char pid[16];
    sprintf(pid, "%d", getpid());

    char *temp = calloc(2048, sizeof(char *));
    char *final = calloc(2048, sizeof(char *));
    int n = 0, i, start = 0;

    // Transverse through the characters of the given string
    for (i = 0; uI[i] != 0; i++) {
        if (uI[i] == '$') {
            n++;
        } else {
            n = 0;
        }

        /* If two '$' characters are found consecutively, append previous
         * characters to a temporary string, then append the pid, and continue
         * processing
         */
        if (n == 2) {
            strcpy(temp, uI);
            temp[i - 1] = '\0';
            strcat(final, &temp[start]);
            strcat(final, pid);
            n = 0;
            start = i + 1;
        }
    }

    /* Copy any remaining characters after the last set of '$$' to the final
     * string
     */
    strcpy(temp, uI);
    temp[i] = '\0';
    strcat(final, &temp[start]);
    strcpy(uI, final);

    // Free temporary strings
    free(temp);
    free(final);
}

/* This helper function transverses through a given userCommand linked list
 * and returns an integer representing the number of nodes in that list
 */
int getListSize(struct userCommand *head) {
    struct userCommand *temp = head;
    int structSize = 0;
    while (temp) {
        structSize++;
        temp = temp->next;
    }
    return structSize;
}

/* This is a helper function that returns the last node of a given
 * userCommand linked list
 */
struct userCommand *getListTail(struct userCommand *head) {
    struct userCommand *temp = head;
    struct userCommand *tail = head;
    while (temp) {
        tail = temp;
        temp = temp->next;
    }
    return tail;
}

/* This function frees the memory associated to a given userCommand list
 */
void freeCommandLL(struct userCommand *list) {
    struct userCommand *head = list;
    while (head) {
        struct userCommand *temp = head;
        head = temp->next;
        free(temp->argmt);
        free(temp);
    }
}
