# Assignment 3 - SymTable
This repository contains the implementation files for Assignment 3 of Princeton COS 217. It is a dual implementation of a Symbol Table ADT using both a Linked List and a Resizing Hash Table.

## The Christmas Day Bug
This assignment officially broke me. The most painful part of this project was a single logic error in symtablehash.c that blew up my entire Christmas day.

The Nightmare
I wrote these two lines in the wrong order inside the resize loop:

```c
curr = curr->next;
q->next = p;
```
By the time I tried to move curr forward, I had already overwritten q->next (the same memory location). This created an infinite loop that hung the CPU and leaked every block of memory in the table.

The Fix
I spent the entire day hunting for a bug that was solved by simply reversing the order of those two lines:

```c
curr = curr->next;
q->next = p;
```
Honestly i feel so dumb after all this.

Anyway,

Results
Status: 100% Correct. Passes all test cases.

Memory: All heap blocks were freed.

Valgrind: 0 errors from 0 contexts.

Building and Testing
To compile the Hash Table implementation:


```bash
gcc217 -O3 testsymtable.c symtablehash.c -o testsymtablehash
```
To run memory checks:


```bash
valgrind --leak-check=full ./testsymtablehash
```
