# Memory-Allocator

# Memory Allocator Project

This project implements a custom memory allocator using different allocation strategies (Best-Fit, Worst-Fit, First-Fit, and Next-Fit) and free list management techniques (Unordered List and Address-Ordered List).

## Files

1. **__mymalloc.c__**: This file contains the implementation of the custom memory allocator functions, including `mymalloc`, `myfree`, and various helper functions for managing the heap and free list.

2. **__mymalloc.h__**: This header file declares the data structures and function prototypes used in the memory allocator implementation.

## Data Structures

1. **__Tag__**: This structure represents the header of each block in the heap. It contains the size of the block (in terms of 16-byte blocks) and a flag indicating whether the block is free or allocated.

2. **__Block__**: This structure represents a block in the heap. It contains pointers to the next and previous blocks in the free list, a `Tag` structure with size and free status information, and the actual data payload.

## Functions

1. **__mymalloc(size_t size)__**: This function allocates a block of memory with the specified size. It uses the current allocation strategy (Best-Fit, Worst-Fit, First-Fit, or Next-Fit) to find a suitable free block and allocates memory from it. If no suitable free block is found, it extends the heap using `sbrk`.

2. **__myfree(void *p)__**: This function frees the memory block pointed to by `p`. It marks the block as free, coalesces adjacent free blocks if possible, and updates the free list accordingly.

3. **__split_block(Block *b, size_t size)__**: This helper function splits a free block `b` into two blocks: one of the requested size and another with the remaining size. It updates the free list accordingly.

4. **__left_coalesce(Block *b)__** and **__right_coalesce(Block *b)__**: These helper functions coalesce the block `b` with its left or right neighboring free block, respectively.

5. **__next_block_in_freelist(Block *b)__** and **__prev_block_in_freelist(Block *b)__**: These helper functions return the next or previous block in the free list, respectively.

6. **__next_block_in_addr(Block *b)__** and **__prev_block_in_addr(Block *b)__**: These helper functions return the next or previous block in memory address order, respectively.

7. **__numberof16blocks(size_t size_inbytes)__**: This helper function calculates the number of 16-byte blocks required to store the given size in bytes.

8. **__printheap()__**: This function prints the current state of the heap, including the size and free status of each block.

9. **__getlisttype()__** and **__setlisttype(ListType lt)__**: These functions get and set the type of the free list (Unordered List or Address-Ordered List), respectively.

10. **__getstrategy()__** and **__setstrategy(Strategy s)__**: These functions get and set the allocation strategy (Best-Fit, Worst-Fit, First-Fit, or Next-Fit), respectively.

11. **__first_fit(size_t size)__**, **__best_fit(size_t size)__**, **__worst_fit(size_t size)__**, and **__next_fit(size_t size, Block *last)__**: These functions implement the respective allocation strategies for finding a suitable free block.

## Usage

1. Include the `mymalloc.h` header file in your program.
2. Use `mymalloc(size_t size)` to allocate memory and `myfree(void *p)` to free memory.
3. Optionally, use `setlisttype(ListType lt)` and `setstrategy(Strategy s)` to set the desired free list type and allocation strategy, respectively.
4. Use `getlisttype()` and `getstrategy()` to retrieve the current free list type and allocation strategy, respectively.
5. Use `printheap()` to print the current state of the heap for debugging purposes.

Note: This implementation assumes a 64-bit system and uses 16-byte alignment for blocks.
