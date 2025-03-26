#include "shell.h"
#define SPACE 32

command* create_command(int argc) {
    command* rv = (command*)calloc(1, sizeof(command));

    rv->argc = argc;
    rv->argv = (char**)calloc((argc + 1), sizeof(char*));

    for (int i = 0; i < argc; i++) {
        rv->argv[i] = (char*)calloc(MAX_ARG_LEN, sizeof(char));
    }

    return rv;
}

command* parse(char* line) {
    // finding the number of arguments before initializing command
    char* linecpy_temp = strdup(line);
    int argc = 0;
    char* init_parsed = strtok(linecpy_temp, " ");

    while (init_parsed && strlen(init_parsed) > 0) {
        argc++;
        init_parsed = strtok(NULL, " ");
    }

    free(linecpy_temp);

    // creating a command to be returned with the parsed arguments
    command* rv = create_command(argc);

    char* linecpy = strdup(line);
    char* parsed = strtok(linecpy, " ");

    for (int i = 0; i < argc && parsed; i++) {
        strcpy(rv->argv[i], parsed);
        parsed = strtok(NULL, " ");
    }

    free(linecpy);
    return rv;
}

bool find_full_path(command* cmd) {
    // duplicating the current path, storing it in a character pointer called
    // env_path
    //    char* env_path = strdup(getenv("PATH"));
    char env_path[MAX_ENV_VAR_LEN + 1];
    strcpy(env_path, getenv("PATH"));
    // creating a char pointer to the first segment of the path (before the
    // first :)
    char* path_sgmt = strtok(env_path, ":");

    // creating a struct of type stat to store the file info of the current
    // segment
    struct stat file_info;

    // creating a character array to hold temporary concatenations of the
    // first argument and each path segment

    while (path_sgmt) {
        // copying the segment in the current iteration to the curr_path string
        int len = strlen(path_sgmt) + strlen(cmd->argv[0]) + 1;
        ;
        char curr_path[len + 1];
        strcpy(curr_path, path_sgmt);

        // concatenating the first argument to the end of the current path
        // segment
        strcat(curr_path, "/");
        strcat(curr_path, cmd->argv[0]);

        // attempting to store the statistics of the current path segment in the
        // stat struct. if it works without error, check if it is a regular
        // file.
        if (stat(curr_path, &file_info) != -1 && S_ISREG(file_info.st_mode)) {
            // assigning a duplicate of the current path to the first argument
            //    cmd->argv[0] = strdup(curr_path);
            strcpy(cmd->argv[0], curr_path);
            return true;
        }
        // if the current segment doesn't meet the criteria above, we move on to
        // the next segment and repeat the process
        path_sgmt = strtok(NULL, ":");
    }

    // if all segments of the path have been concatenated and searched and the
    // file has not been found, return false.
    return false;
}

int execute(command* cmd) {
    // checking if command is build in
    if (is_builtin(cmd)) {
        // if so, execute this command
        do_builtin(cmd);
    }

    // else, if command is not built in:
    else {
        // declaring integer to store status of child process
        int command_process_status;

        // storing the full command path in argv[0]
        if (!find_full_path(cmd)) {
            printf("Command %s not found!\n", cmd->argv[0]);
            return ERROR;
        }

        // forking the current process; storing the returned pid
        pid_t pid = fork();

        // if the pid is 0:
        if (pid == 0) {
            // these instructions will only be executed by the child process
            execv(cmd->argv[0], cmd->argv);
            // if execv returns, there was an error
            return ERROR;
        } else {
            // waiting for the child process to terminate
            wait(&command_process_status);
        }
    }

    return SUCCESS;
}

void cleanup(command* cmd) {
    for (int i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
        cmd->argv[i] = NULL;
    }
    free(cmd->argv);
    cmd->argv = NULL;
    free(cmd);
}

bool is_builtin(command* cmd) {
    // Do not modify
    char* executable = cmd->argv[0];
    if (strcmp(executable, "cd") == 0 || strcmp(executable, "exit") == 0)
        return true;
    return false;
}

int do_builtin(command* cmd) {
    // Do not modify
    if (strcmp(cmd->argv[0], "exit") == 0) exit(SUCCESS);

    // cd
    if (cmd->argc == 1)
        return chdir(getenv("HOME"));  // cd with no arguments
    else if (cmd->argc == 2)
        return chdir(cmd->argv[1]);  // cd with 1 arg
    else {
        fprintf(stderr, "cd: Too many arguments\n");
        return ERROR;
    }
}
