#include "shell.c"
#include "shell.h"

int _main(int argc, const char* argv[]) {
    char input[MAX_LINE_SIZE + 1];	// Creates character array of maximum line size named input to hold user input

    while (true) {
        printf("%s", SHELL_PROMPT);	// Prompts user with slsh$
        fgets(input, MAX_LINE_SIZE, stdin);	// Reads string from stdin of maximum line size and stores it in input
        input[strcspn(input, "\n")] = '\0';  // Removes newline from user input
        command* cmd = parse(input);	// Creates command called cmd using parse function on input

        if (cmd->argc > 0) {		// If command is not empty
            if (execute(cmd) == ERROR)	// Check if executing command results in error
                fprintf(stderr, "%s command failed\n", input);	// If so, print command failed
        }

        cleanup(cmd);	// Deallocate memory allocated by creating the command
    }

    return EXIT_SUCCESS;
}


int main(int argc, const char** argv) { return _main(argc, argv); }

