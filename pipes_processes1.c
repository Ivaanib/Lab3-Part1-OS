// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  



int main() {
    int fd1[2];  // Pipe 1: P1 -> P2
    int fd2[2];  // Pipe 2: P2 -> P1

    char fixed_str1[] = "howard.edu";  // String to concatenate in P2
    char fixed_str2[] = ""; // Empty since we don't need to add "gobison.org" twice
    char input_str[100]; 
    pid_t p;

    if (pipe(fd1) == -1) {
        fprintf(stderr, "Pipe 1 Failed\n");
        return 1;
    }
    if (pipe(fd2) == -1) {
        fprintf(stderr, "Pipe 2 Failed\n");
        return 1;
    }

    // Taking input in Parent P1
    printf("Input : ");
    scanf("%s", input_str);
    p = fork();

    if (p < 0) {
        fprintf(stderr, "fork Failed\n");
        return 1;
    }

    // Parent process (P1)
    else if (p > 0) {
        char concat_str[300];

        close(fd1[0]);  // Close reading end of pipe 1
        close(fd2[1]);  // Close writing end of pipe 2

        // Send input string to P2 via pipe 1
        write(fd1[1], input_str, strlen(input_str) + 1);
        close(fd1[1]);  // Close writing end after sending

        // Wait for child process to finish processing
        wait(NULL);

        // Read the concatenated string from P2 via pipe 2
        read(fd2[0], concat_str, sizeof(concat_str));
        close(fd2[0]);  // Close reading end after receiving

        // Concatenate fixed_str2 (empty or any other needed string) to the string received from P2
        strcat(concat_str, fixed_str2);
        printf("Output : %s\n", concat_str);
    }

    // Child process (P2)
    else {
        char concat_str[200];
        char second_input[100];

        close(fd1[1]);  // Close writing end of pipe 1
        close(fd2[0]);  // Close reading end of pipe 2

        // Read string from P1 via pipe 1
        read(fd1[0], concat_str, sizeof(concat_str));
        close(fd1[0]);  // Close reading end after receiving

        // Concatenate "howard.edu" to the string received from P1
        strcat(concat_str, fixed_str1);
        printf("Output : %s\n", concat_str);

        // Prompt user for another input in P2
        printf("Input : ");
        scanf("%s", second_input);

        // Concatenate the second input to concat_str
        strcat(concat_str, second_input);

        // Send the concatenated string back to P1 via pipe 2
        write(fd2[1], concat_str, strlen(concat_str) + 1);
        close(fd2[1]);  // Close writing end after sending

        exit(0);
    }

    return 0;
}