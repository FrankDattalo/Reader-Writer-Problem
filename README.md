# Reader-Writer-Problem
This repository contains a solution to the reader writer problem written in C

## Rules
1. Any time a writer is waiting to write no reader may enter a critical section.
2. Multiple readers can read at the same time.
3. Multiple writers may not write at the same time.
4. Readers cannot block each other, but may block writer.
