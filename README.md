# Reader-Writer-Problem
This repository contains a solution to the reader writer problem written in C.
The writers will generate a random integer value greater than or equal to zero
and append the generated value to the end of the output file.  The readers will
read a random line from the file and report the value at that line.

## Rules
1. Any time a writer is waiting to write no reader may enter a critical section.
2. Multiple readers can read at the same time.
3. Multiple writers may not write at the same time.
4. Readers cannot block each other, but may block writer.
