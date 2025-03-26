#include "shell.c"
#include "shell.h"

int _main(int argc, const char* argv[]) {
    char input[MAX_LINE_SIZE + 1];

    while (true) {
        printf("%s", SHELL_PROMPT);
        fgets(input, MAX_LINE_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0';  // Remove newline from user input
        command* cmd = parse(input);

        if (cmd->argc > 0) {
            if (execute(cmd) == ERROR)
                fprintf(stderr, "%s command failed\n", input);
        }

        cleanup(cmd);
    }

    return EXIT_SUCCESS;
}


int main(int argc, const char** argv) { return _main(argc, argv); }

