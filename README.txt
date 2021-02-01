/*******************************************************************************
 * Program name: smallsh
 * Author: Felipe Groberio <teixeirf@oregonstate.edu>
 * Date: November 3rd, 2020
 * Description: this program simulates an interactive shell. It implements
                built-in commands for changing the working directory, the
                status of the last child process, and to exit the shells. All
                other commands are managed by the OS.
 ******************************************************************************/
    
    You can compile this program by using the following command:
        
        $ gcc --std=gnu99 -o smallsh main.c commandLL.c shellCommands.c


    To execute the program, use the folowing:
        
        $ ./smallsh
