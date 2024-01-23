# Usage

#### Compile:

`make lib_hider`

#### Run:
`LD_PRELOAD=$PWD/lib_hider.so <program> <arguments>`

# The Research 

## Pre-Research:
Before tackling the issue at hand, I first explored how programs like ps retrieve the arguments of a specific process. 
To answer this question, I used tools such as `strace` and `ltrace` during a ps run.

The analysis revealed that the arguments were obtained from a virtual file named `/proc/<pid>/cmdline`, created by the operating system to store the command line that invoked the running process.

### How does `/proc/<pid>/cmdline` functions:
I entertained a couple of ideas about how cmdline functions:
1.	The kernel stores the execution string in `/proc/<pid>/cmdline` and doesn't update it when the arguments in argv change. 
In other words, modifying the arguments in argv won't alter the data in `/proc/<pid>/cmdline`.

2.	The kernel monitors the argument pointers at the stack and updates the `/proc/<pid>/cmdline` file as they change.

To verify these theories, I used gdb-peda to inspect a simple C program. 

Setting a breakpoint at `main`, I observed the stack and found argv addresses.
By changing the values of the strings of argv using gdb, I confirmed that it affected `/proc/<pid>/cmdline`.

## The Solution:

To make the arguments disappear, I realized I needed to run in the program's context to access its stack and nullify the arguments. 
However, I still wanted the program to run with its original arguments. 
Therefore, I devised a plan to hook the main function, copy and nullify the original argv memory, and then call `main` with the copied arguments.

I decided to allocate new pointers on the heap, copy the data from argv 
to them, and set them as the argv pointers.

The next challenge was hooking main. 
I chose to use LD_PRELOAD to load a shared library I wrote, which hooked __libc_start_main using dlsym to call the actual __libc_start_main when I finished.

I created a wrapped main function within the shared library that hides the arguments, calls the real main function, and frees the arguments on the heap at the end of the run.

 I hope this helps! Let me know if you have any further questions or if there's anything specific you'd like to adjust. 


## What Next

By using the research results, we can also run a program with fake arguments in the execution string. 
Then by hooking main, we can allocate new arguments on the heap, reading them from ENV variable for example, then the user will see other arguments if he'll use ps.