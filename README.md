# Large Integer Multiplication Using Pipes

This C program demonstrates inter-process communication using pipes to perform large integer multiplication. The program breaks down the multiplication of two 4-digit numbers using a parent-child process model, where the child process performs the basic multiplications and the parent process coordinates the overall calculation.

## How It Works

The program implements the following multiplication algorithm:
- For two 4-digit numbers (e.g., 1234 * 5678), the calculation is broken down as:
  - X = (12 * 56) * 10^4
  - Y = (12 * 78 + 34 * 56) * 10^2
  - Z = (34 * 78) * 10^0
  - Final result = X + Y + Z

The calculation is distributed between a parent and child process:
1. Parent process breaks down the numbers and sends pairs to the child
2. Child process performs the multiplication and sends results back
3. Parent process combines the results to produce the final answer

## Prerequisites

- GCC compiler
- Unix-like operating system (Linux, macOS, etc.)

## Compilation

To compile the program:
```bash
gcc -o assignment-2 assignment-2.c
```

## Usage

Run the program with two 4-digit integers as command-line arguments:
```bash
./assignment-2 1234 5678
```

### Input Requirements
- Exactly two command-line arguments
- Each number must be between 1000 and 9999 (4 digits)

### Example Output
```
Your integers are 1234 5678
Parent (PID 26580): created child (PID 26581)

###
# Calculating X
###
Parent (PID 26580): Sending 12 to child
Parent (PID 26580): Sending 56 to child
Child (PID 26581): Received 12 from parent
Child (PID 26581): Received 56 from parent
Child (PID 26581): Sending 672 to parent
Parent (PID 26580): Received 672 from child

[... additional calculation steps ...]

1234*5678 == 6720000 + 284000 + 2652 == 7006652
```

## Error Handling

The program includes error checking for:
- Invalid number of arguments
- Numbers outside the valid range (1000-9999)
- Pipe creation failures
- Fork operation failures
