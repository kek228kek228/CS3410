# Lab 9 : Introduction to GDB

## What is GDB?
GDB is the GNU project debugger for C/C++. Some of its functionalities include the following:
1. Starting your program, specifying anything that might affect its behavior
2. Making your program stop on specified conditions
3. Examining what has happened when your program has stopped
4. Changing things in your program, so you can experiment with correcting the effects of one bug and go on to learn about another.

## Why GDB?
GDB is an incredibly useful tool that helps you look inside your code and truly step through it from the eyes of the computer. GDB will be incredibly helpful when you start `P4 > Buffer Overflow` and have to look through RISC-V assembly code. GDB supports many different languages and similar debuggers exist for your favorite languages too: `OCaml, Java, JS, etc.`

Furthermore, unlike most modern languages, C code is particularly fragile. There is no garbage collection, no built in error and exception handling, no nothing! C is great because it is efficient and C compilation is extremely optimized, however, it is really easy to make mistakes. Runtime exceptions in C manifest themselves as **segmentation faults** (now referred to as segfaults). Segfaults are not particularly useful errors; they could mean anything from array out of bounds error to accessing kernel memory space. 

_Probably something that happened somewhere_
```
TA: "You should use GDB to solve this issue that you've been unable to fix for the last 20 years..."
students: "But we're smart and we can simply step through our codes ourselves!"
TA: *sighs*
```

GDB helps you make sense of your code and your execution at runtime. You can check the values of important variables, skip over trivial code or slowly step through questionable segments.

Ultimately, GDB is often seen as a daunting task, but it really isn't hard to use. 
**Read further if you are ready to learn!**

## Getting Started
### How do we compile our code?
Let's assume that we have .c file called `test.c`. Let us compile this program so that we can use GDB. We compile the code as follows:

```console
$ gcc -std=c99 -g -o test test.c
$ ls -l
-rwxr-xr-x 1 awb93 pug-awb93 9840 Mar 22 16:33 test
-rw-r--r-- 1 awb93 pug-awb93  175 Mar 22 16:33 test.c
```

**Breakdown**:
We compile our C program with the following flags:
 - `std=c99`:  compile with C language standard C99
 - `-g`:       debugging flag, executable `test` now has debugging symbols allowing you to step through the original C source with GDB
 - `-o`:       write output to file `test`, if no file given then write to `a.out`
The `gcc` compilation command compiles our C code and outputs the executable binary into a file called `test`. We notice that the `test` executable has the `-x` privilege flag. This means that it is an executable. `-r` and `-w` mean that a file has read and write privileges.  

_There are a ton of other compilation flags that might be useful, type `man gcc` in your terminal to read more_
 
### How do we run our code?
Now that we have created a binary executable file called `test`, we can run it as follows:

```console
$ gdb ./test
GNU gdb (Ubuntu 7.11.1-0ubuntu1~16.5) 7.11.1
Copyright (C) 2016 Free Software Foundation, Inc.
...
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./test...done.
(gdb)
```

## Useful GDB Commands
 - `r`:         executes the entire program
 - `b`:         set a breakpoint in the code
 - `s` <arg>:   step one line in the execution of code (or <arg> lines if provided)
 - `n` <arg>:   step one line in code without diving into function calls (or <arg> lines if provided)
 - `c`:         continue execution
 - `p`:         display the value in a variable (you can cast the variable to change how it gets printed too!)
 - `l`:         lists program with line numbers
 - `backtrace`/`bt`: returns a trace of steps leading to the segfault
 - `clear`:     clears breakpoints
 - `disable`:   disables a breakpoint
 - `enable`:    enables a breakpoint
 - `q`:         quit gdb
 - `<ctrl-C>`:  pauses the execution of program 
 - <enter>:     runs the previous entered command again
 - and more...
 
### Example `test.c`

```c
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
  if (argc < 2)
    exit(1);
    
  char buff[10];
  char start = 'a' - 1;
  for (int i = 0; i < 10; i++) {
    buff[i] = ++start;
  }
  
  printf("Look %s! I made a string: %s\n", argv[1], buff);
  return 0;
}
```

**Executing code with GDB**
```console
$ gcc -std=c99 -g -o test test.c
$ gdb ./test
GNU gdb (Ubuntu 7.11.1-0ubuntu1~16.5) 7.11.1
...
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./test...done.
(gdb) b 1
Breakpoint 1 at 0x4005e5: file test.c, line 1.
(gdb) r you
Starting program: /home/awb93/test

Breakpoint 1, main (argc=1, argv=0x7fffffffe928) at test.c:4
4	int main(int argc, char** argv) {
(gdb) s 3
10	  for (int i = 0; i < 10; i++) {
(gdb) p argv[1]
$3 = 0x7fffffffeb6f "you"
(gdb) c
Hey you! I made a string: abcdefghij@
[Inferior 1 (process 34147) exited normally]
(gdb) q
```

**Discussion**:
In the above example we simply play around with some GDB commands. You'll notice that line numbers appear to the left code segments, that you can display elements in an array, step by more than one line by using `s <number>` and set breakpoints at specific lines. Lastly, we also notice that we can run our code with arguments; for example we write `r you`.

### Example `seg_test.c`

```c
#include <stdio.h>
#include <signal.h>

int foo() {
  raise(SIGSEGV);
  return 3410;
}

int main() {
  int course_number = foo();
  printf("CS%d is the best CS course!\n", course_number);
  return 0;
}
```

**Executing code in GDB**
```console
$ gcc -std=c99 -g -o segfault seg_test.c
$ gdb ./seg
GNU gdb (Ubuntu 7.11.1-0ubuntu1~16.5) 7.11.1
...
Reading symbols from ./seg...done.
(gdb) r
Starting program: /home/awb93/seg_test

Program received signal SIGSEGV, Segmentation fault.
0x00007ffff7a42428 in __GI_raise (sig=11) at ../sysdeps/unix/sysv/linux/raise.c:54
54	../sysdeps/unix/sysv/linux/raise.c: No such file or directory.
(gdb) backtrace
#0  0x00007ffff7a42428 in __GI_raise (sig=11) at ../sysdeps/unix/sysv/linux/raise.c:54
#1  0x0000000000400574 in foo () at seg_test.c:5
#2  0x000000000040058d in main () at seg_test.c:10
(gdb) b foo
Breakpoint 2 at 0x40056a: file seg_test.c, line 5.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/awb93/seg

Breakpoint 1, foo () at seg_test.c:5
5		raise(SIGSEGV);
(gdb) q
```

**Discussion**:
In the above example we induce a segfault by executing the command `raise(SIGSEGV)`. Hopefully you don't do that for fun in your own code! Regardless, we use the `backtrace` command to locate the source of the error. **#0** is where the segfault was generated. Now that we found out where to look for the source of the error we can simply set a breakpoint and step through that area.

## Valgrind and Other Tools
From the website: "The Valgrind tool suite provides a number of debugging and profiling tools that help you make your programs faster and more correct. The most popular of these tools is called Memcheck. It can detect many memory-related errors that are common in C and C++ programs and that can lead to crashes and unpredictable behaviour."

For your upcoming projects such `P5 $ Cache` and `P6 * Malloc` you will be writing a lot of code in C. It might be useful to acquaint yourself with debugging tools such as GDB and Valgrind. Although the examples we show in lab are not likely to require such tools, you might find it exceedingly helpful to use them as your exploration into C continues.

_All of these tools are available on the VM and the SSH. You may download them onto your own computer, but most testing should be done on course servers_

## The Terminal User Interface (TUI)
Our version of GDB is compiled with TUI support!  This means you can have a graphical display of the code you are running, as you are running it.  
Once GDB starts up, simply enter the command `layout src` and you'll see the source in a separate window, if the target was compiled with debugging information.
```
  +--hello.c----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
   |1       #include <stdlib.h>                                                                                                                                                          |
   |2       #include <stdio.h>                                                                                                                                                           |
   |3       int main(){                                                                                                                                                                  |
B+>|4         printf("hello world!\n");                                                                                                                                                  |
   |5         return 0;                                                                                                                                                                  |
   |6       }                                                                                                                                                                            |
   |7                                                                                                                                                                                    |
   |8                                                                                                                                                                                    |
   |9                                                                                                                                                                                    |
   |10                                                                                                                                                                                   |
   |11                                                                                                                                                                                   |
   |12                                                                                                                                                                                   |
   |13                                                                                                                                                                                   |
   |14                                                                                                                                                                                   |
   |15                                                                                                                                                                                   |
   |16                                                                                                                                                                                   |
   |17                                                                                                                                                                                   |
   |18                                                                                                                                                                                   |
   |19                                                                                                                                                                                   |
   |20                                                                                                                                                                                   |
   |21                                                                                                                                                                                   |
   |22                                                                                                                                                                                   |
   |23                                                                                                                                                                                   |
   |24                                                                                                                                                                                   |
   |25                                                                                                                                                                                   |
   |26                                                                                                                                                                                   |
   |27                                                                                                                                                                                   |
   +-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
native process 29388 In: main                                                                                                                                          L4    PC: 0x40052a 
(gdb) run
Starting program: /home/mz282/hello
hello world!
(gdb) b 4 1 (process 29387) exited normally]
Breakpoint 1 at 0x40052a: file hello.c, line 4.
(gdb) run
Starting program: /home/mz282/hello

Breakpoint 1, main () at hello.c:4
(gdb) 
```
The display will show the current line you are on (highlighted) as well as the location of any breakpoints you have and if they have ever been hit.  There are a couple of other layouts that allow you to inspect registers and assembly as well, but for now source is the most useful.  You can exit TUI mode by entering the command `tui disable` or the keybinding `Ctrl-x a` (`control-x` at the same time, then push `a` by itself).  
While in TUI mode, you can scroll the code by using the up and down arrow keys.  However, this means you can't use those keys for command history inside GDB.  Instead, you can use the emacs-style readline bindings `Ctrl-p` and `Ctrl-n` to go up and down, respectively.  You can also use `Ctrl-x o` to switch the active window that the arrow keys work on.
