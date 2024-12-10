#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

/**
 * Executes the command "cat scores | grep <argument> | sort".
 *
 * This program creates three processes connected by two pipes to mimic the shell pipeline.
 */

int main(int argc, char **argv)
{
    int pipefd1[2]; // Pipe between cat and grep
    int pipefd2[2]; // Pipe between grep and sort
    pid_t pid1, pid2;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s search_term\n", argv[0]);
        exit(1);
    }

    // Create first pipe
    if (pipe(pipefd1) == -1)
    {
        perror("pipe");
        exit(1);
    }

    // Fork first child (P2) to execute grep
    pid1 = fork();

    if (pid1 == -1)
    {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0)
    {
        // Inside first child process (P2)

        // Create second pipe
        if (pipe(pipefd2) == -1)
        {
            perror("pipe");
            exit(1);
        }

        // Fork second child (P3) to execute sort
        pid2 = fork();

        if (pid2 == -1)
        {
            perror("fork");
            exit(1);
        }

        if (pid2 == 0)
        {
            // Inside second child process (P3)
            // Redirect stdin to read from pipefd2[0]
            dup2(pipefd2[0], STDIN_FILENO);

            // Close unused file descriptors
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);

            // Execute sort
            execlp("sort", "sort", NULL);
            perror("execlp sort");
            exit(1);
        }
        else
        {
            // Back in first child process (P2)
            // Redirect stdin to read from pipefd1[0]
            dup2(pipefd1[0], STDIN_FILENO);
            // Redirect stdout to write to pipefd2[1]
            dup2(pipefd2[1], STDOUT_FILENO);

            // Close unused file descriptors
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);

            // Build grep arguments
            char *grep_args[] = {"grep", argv[1], NULL};

            // Execute grep
            execvp("grep", grep_args);
            perror("execvp grep");
            exit(1);
        }
    }
    else
    {
        // Back in parent process (P1)
        // Redirect stdout to write to pipefd1[1]
        dup2(pipefd1[1], STDOUT_FILENO);

        // Close unused file descriptors
        close(pipefd1[0]);
        close(pipefd1[1]);

        // Execute cat
        execlp("cat", "cat", "scores", NULL);
        perror("execlp cat");
        exit(1);
    }

    return 0;
}
