# Producer-Consumer System using pipes

## Overview

The programs demonstrates a **Producer-Consumer** problem using **Pthreads** and **pipes**. It features:
- **Producers** generating random characters.
- **Consumers** consuming characters produced by the producers.
- A shared buffer implemented using a **pipe** between producers and consumers.
- The producers write characters to a pipe, and the consumers read and count vowels (`a`, `e`, `i`, `o`, `u`) from the pipe.
- The producers and consumers periodically write their activity (produced characters and vowel counts) to text files every 60 seconds.
- After 5 minutes, the producers terminate, and the consumers write the final vowel count to their respective files.

## Key Components

- **Producer Threads:**
  - Each producer generates random lowercase alphabet characters.
  - They write the generated characters to a pipe.
  - Every 60 seconds, each producer logs the characters produced to a file (`prod_<id>.txt`).
  - After 5 minutes, the producers stop generating characters and finalize their log files.

- **Consumer Threads:**
  - Each consumer reads characters from the pipe and checks if it's a vowel.
  - They count the number of vowels consumed.
  - Every 60 seconds, each consumer logs the vowel count to a file (`cons_<id>.txt`).
  - Once the producers stop, consumers write their final vowel count to their respective files.

## Files Generated

- **Producer Logs (`prod_<id>.txt`)**: Each producer logs the characters they generate.
- **Consumer Logs (`cons_<id>.txt`)**: Each consumer logs the count of vowels they consume.

## Code Breakdown

q_1.c : Consists of 1 Producer and 1 Consumer Thread.

q_2.c : Consists of 1 Producer and 5 Consumer Thread.

q_3.c : Consists of 5 Producer and 5 Consumer Thread.

1. **Producer Logic (`producer` function)**:
   - Randomly generates characters.
   - Writes the characters to the pipe.
   - Writes characters to a log file every 60 seconds.
   - After 5 minutes, stops and finalizes the log file.

2. **Consumer Logic (`consumer` function)**:
   - Reads characters from the pipe.
   - Checks if a character is a vowel.
   - Increments the vowel count if a vowel is encountered.
   - Writes vowel count to a log file every 60 seconds.
   - After producers stop, writes the final vowel count to the log file.

3. **Synchronization**:
   - **Pthread Mutex** is used in q_3.c to allow only one thread to write to the pipe at once.
   - Mutexes are not used using `read()` as pipe reads are atomic.
   
4. **Pipe**:
   - A unidirectional communication channel used between the producers and consumers.

Note: the `read()` instruction in pipes blocks the calling thread if the pipe is empty. If it does not receive anything even after sometime, then it returns a negative value. Hence it is used as a condition in the while loop for the consumers.

## How to Run

1. **Compile**: 
    -  `make all` compiles all the codes : q_1.c, q_2.c, q_3.c, bonus.c
    - `make q_subprobno` compiles q_subprobno.c
    - `make clean` removes all the executables.

2. **Run**:
    - run `./q_subprobno` for a particular executable.
   
## Bonus Part Explanation

The bonus part is implemented in the file bonus.c.

1. **Implementation** : The implementation only differs slightly from q_3:
    - The `pid = fork()` is used to create a child process, which is then moved to a new pid namespace using `unshare(CLONE_NEWPID)`
    - Since the child process is now the init process for the new namespace, it cannot spawn threads.
    - Hence, we `fork()` again to create another child process which will spawn the threads.

2. **Communication** :
The processes communicate through UNIX domain sockets, which work similar to pipes.

3. **Compile** : Comiple using `make bonus`

4. **Run** : To create a new namespace, you must use root privileges. Therefore, run using `sudo ./bonus`