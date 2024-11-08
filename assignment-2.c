#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ_END 0  // Define the read end of the pipe
#define WRITE_END 1 // Define the write end of the pipe

// Function to handle errors and terminate the program
void error_handler(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}

int main(int argc, char *argv[]) {
    // Check if exactly 2 arguments are provided
    if (argc != 3) {
        error_handler("Please provide two 4-digit integers as command-line arguments.");
    }

    // Convert command-line arguments to integers
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);

    // Validate that both numbers are 4-digit integers (between 1000 and 9999)
    if (num1 < 1000 || num1 > 9999 || num2 < 1000 || num2 > 9999) {
        error_handler("Both numbers must be 4-digit integers (1000-9999).");
    }

    printf("Your integers are %d %d\n", num1, num2);

    // Create pipes for communication between parent and child
    int parent_to_child[2]; // Pipe for parent to child communication
    int child_to_parent[2]; // Pipe for child to parent communication

    // Check if pipe creation is successful
    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        error_handler("Pipe creation failed.");
    }

    // Fork a new process
    pid_t pid = fork();

    // Check if fork is successful
    if (pid == -1) {
        error_handler("Fork failed.");
    }

    if (pid > 0) {  // Parent process
        printf("Parent (PID %d): created child (PID %d)\n\n", getpid(), pid);

        // Close unused pipe ends
        close(parent_to_child[READ_END]);
        close(child_to_parent[WRITE_END]);

        // Decompose numbers into parts
        int a1 = num1 / 100;  // First two digits of num1
        int a2 = num1 % 100;  // Last two digits of num1
        int b1 = num2 / 100;  // First two digits of num2
        int b2 = num2 % 100;  // Last two digits of num2

        // Step 1: Calculate X = (a1 * b1) * 10000
        printf("###\n# Calculating X\n###\n");
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a1);
        write(parent_to_child[WRITE_END], &a1, sizeof(int)); // Send a1 to child
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b1);
        write(parent_to_child[WRITE_END], &b1, sizeof(int)); // Send b1 to child

        int A;  // Result of a1 * b1
        read(child_to_parent[READ_END], &A, sizeof(int));  // Read result from child
        printf("Parent (PID %d): Received %d from child\n", getpid(), A);
        long long X = (long long)A * 10000;  // Multiply result by 10000 for X

        // Step 2: Calculate Y = (a1 * b2 + a2 * b1) * 100
        printf("\n###\n# Calculating Y\n###\n");
        // a1 * b2
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a1);
        write(parent_to_child[WRITE_END], &a1, sizeof(int)); // Send a1 to child
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b2);
        write(parent_to_child[WRITE_END], &b2, sizeof(int)); // Send b2 to child

        int B;  // Result of a1 * b2
        read(child_to_parent[READ_END], &B, sizeof(int));  // Read result from child
        printf("Parent (PID %d): Received %d from child\n", getpid(), B);

        // a2 * b1
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a2);
        write(parent_to_child[WRITE_END], &a2, sizeof(int)); // Send a2 to child
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b1);
        write(parent_to_child[WRITE_END], &b1, sizeof(int)); // Send b1 to child

        int C;  // Result of a2 * b1
        read(child_to_parent[READ_END], &C, sizeof(int));  // Read result from child
        printf("Parent (PID %d): Received %d from child\n", getpid(), C);

        long long Y = (long long)(B + C) * 100;  // Multiply sum of B and C by 100 for Y

        // Step 3: Calculate Z = a2 * b2
        printf("\n###\n# Calculating Z\n###\n");
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a2);
        write(parent_to_child[WRITE_END], &a2, sizeof(int)); // Send a2 to child
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b2);
        write(parent_to_child[WRITE_END], &b2, sizeof(int)); // Send b2 to child

        int D;  // Result of a2 * b2
        read(child_to_parent[READ_END], &D, sizeof(int));  // Read result from child
        printf("Parent (PID %d): Received %d from child\n", getpid(), D);

        long long Z = D;  // Z is the result of a2 * b2

        // Calculate the final result: X + Y + Z
        long long result = X + Y + Z;

        // Print the final result
        printf("\n%d*%d == %lld + %lld + %lld == %lld\n", num1, num2, X, Y, Z, result);

        // Close used pipe ends
        close(parent_to_child[WRITE_END]);
        close(child_to_parent[READ_END]);

        // Wait for child process to finish
        wait(NULL);
    } else {  // Child process
        // Close unused pipe ends
        close(parent_to_child[WRITE_END]);
        close(child_to_parent[READ_END]);

        int x, y, result;
        // Loop to receive input from parent, perform multiplication, and send back result
        while (1) {
            // Read x and y from the parent
            if (read(parent_to_child[READ_END], &x, sizeof(int)) <= 0) break;
            if (read(parent_to_child[READ_END], &y, sizeof(int)) <= 0) break;

            // Print received values
            printf("Child (PID %d): Received %d from parent\n", getpid(), x);
            printf("Child (PID %d): Received %d from parent\n", getpid(), y);

            // Multiply x and y
            result = x * y;
            printf("Child (PID %d): Sending %d to parent\n", getpid(), result);

            // Send result back to the parent
            write(child_to_parent[WRITE_END], &result, sizeof(int));
        }

        // Close used pipe ends
        close(parent_to_child[READ_END]);
        close(child_to_parent[WRITE_END]);
        exit(0);
    }

    return 0;
}
