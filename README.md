# Reader-Writer-Problem
This repository contains a solution to the reader writer problem written in C.
The writers will generate a random integer value greater than or equal to zero
and append the generated value to the end of the output file.  The readers will
read a random line from the file and report the value at that line.

## Rules
1. There may exist any amount of readers greater than zero.
2. There may exist and amount of writers greater than zero.
3. Any time a writer is waiting to write no reader may enter a critical section.
4. Multiple readers can read at the same time.
5. Multiple writers may not write at the same time.
6. Readers cannot block each other, but may block writer.

## Solution Pseudo-Code
The solution is pseudo-code is outlined below.  It assumes that the reads and
writes to the integer variables are atomic.  To properly solve the problem using
the solution outlined below, the aforementioned operations must be atomic.

```c
reader() {
  reader_count++

  if reader_count == 1 {
    wait( lock )
  }

  // read

  reader_count--

  if reader_count == 0 {
    signal( lock )
  }
}

writer() {
  wait( lock )

  // write ...

  wait( lock )
}
```

## Compiling and Running the Code

```
git clone https://github.com/FrankDattalo/Reader-Writer-Problem.git
cd Reader-Writer-Problem
gcc main.c -lpthread -lrt -o rw
./rw <Main Thread Sleep Time> <Writer Thread Count> <Reader Thread Count>
```
