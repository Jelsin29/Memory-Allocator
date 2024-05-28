/*Autor:
Farah Alhasan
Jelsin Stiben Sanchez Almanza
*/
#include "mymalloc.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>


Block *first_fit(size_t size);
Block *best_fit(size_t size);
Block *worst_fit(size_t size);
Block *next_fit(size_t size, Block *last);

void *mymalloc(size_t size) {
    // Round up size to nearest multiple of 16
    size = (size + 15) & ~0x0F;

    // First call to mymalloc
    if (!heap_start) {
        heap_start = sbrk(0);    
        heap_end = heap_start;
        if (sbrk(HEAP_SIZE) == (void *)-1)
            return NULL; // sbrk failed
  
        // Initialize the free list with the entire heap
        heap_start->info.size = numberof16blocks(HEAP_SIZE) - 1;
        heap_start->info.isfree = 1;
        free_list = heap_start;
    }

    // Find a free block based on the strategy
    Block *current = NULL;
    Block *prev = NULL;
    switch (strategy) {
        case BEST_FIT:
            current = best_fit(size);
            break;
        case WORST_FIT:
            current = worst_fit(size);
            break;
        case FIRST_FIT:
            current = first_fit(size);
            break;
        case NEXT_FIT:
            current = next_fit(size, last_freed);
            break;
    }

    if (current) {
        // Split block if needed
        if (current->info.size >= size + numberof16blocks(sizeof(Block))) {
            current = split_block(current, size);
        }

        // Mark block as used and remove it from the free list
        current->info.isfree = 0;
        if (prev) {
            prev->next = current->next;
        } else {
            free_list = current->next;
        }
        if (current->next) {
            current->next->prev = prev;
        }

        last_freed = current; // Update last_freed for next_fit
        // Print allocation message
        printf("Allocated %zu bytes for this request\n", size);
        return current->data;
    }

    // No free block found, extend heap
    Block *new_block = heap_end;
    if (sbrk(size + sizeof(Block)) == (void *)-1)
        return NULL; // sbrk failed

    new_block->info.size = size;
    new_block->info.isfree = 0;
    heap_end += size + sizeof(Block);
    return new_block->data;
}

void myfree(void *p) {
    if (p == NULL) {
        return; // Avoid freeing a NULL pointer
    }
    Block *block = (Block *)((char *)p - sizeof(Tag));
    block->info.isfree = 1;

    // Coalesce with next block if possible
    Block *next = next_block_in_addr(block);
    if (next && next->info.isfree) {
        block = right_coalesce(block);
    }

    // Coalesce with previous block if possible
    Block *prev = prev_block_in_addr(block);
    if (prev && prev->info.isfree) {
        block = left_coalesce(prev);
    } else {
        // Add block to free list
        if (listtype == UNORDERED_LIST) {
            block->next = free_list;
            block->prev = NULL;
            if (free_list) {
                free_list->prev = block;
            }
            free_list = block;
        } else {
            // Address-ordered list
            Block *current = free_list;
            Block *prev_free = NULL;
            while (current && (char *)current < (char *)block) {
                prev_free = current;
                current = current->next;
            }
            block->next = current;
            block->prev = prev_free;
            if (prev_free) {
                prev_free->next = block;
            } else {
                free_list = block;
            }
            if (current) {
                current->prev = block;
            }
        }
    }
    last_freed = block; // Update last_freed
}

Block *split_block(Block *b, size_t size) {
    Block *new_block = (Block *)((char *)b + size + sizeof(Block));
    new_block->info.size = b->info.size - size - numberof16blocks(sizeof(Block));
    new_block->info.isfree = 1;
    new_block->next = NULL; // Initialize next pointer to NULL
    new_block->prev = NULL; // Initialize prev pointer to NULL
    b->info.size = size;

    // Update free list
    new_block->next = b->next;
    new_block->prev = b;
    if (b->next) {
        b->next->prev = new_block;
    }
    b->next = new_block;

    if (listtype == ADDR_ORDERED_LIST) {
        // Maintain address-ordered list
        Block *current = free_list;
        Block *prev_free = NULL;
        while (current && (char *)current < (char *)new_block) {
            prev_free = current;
            current = current->next;
        }
        new_block->next = current;
        new_block->prev = prev_free;
        if (prev_free) {
            prev_free->next = new_block;
        } else {
            free_list = new_block;
        }
        if (current) {
            current->prev = new_block;
        }
    }

    return new_block;
}

Block *left_coalesce(Block *b) {
    Block *prev = prev_block_in_addr(b);
    if (prev == NULL) {
        return b; // b is already the first block, cannot coalesce
    }
    prev->info.size += b->info.size + numberof16blocks(sizeof(Block));
    prev->next = b->next;
    if (b->next) {
        b->next->prev = prev;
    }
    return prev;
}

Block *right_coalesce(Block *b) {
    Block *next = next_block_in_addr(b);
    if (next == NULL || next == heap_end) {
        return b; // next is NULL or at the end of the heap, cannot coalesce
    }
    b->info.size += next->info.size + numberof16blocks(sizeof(Block));
    b->next = next->next;
    if (next->next) {
        next->next->prev = b;
    }
    return b;
}

Block *next_block_in_freelist(Block *b) {
    return b->next;
}

Block *prev_block_in_freelist(Block *b) {
    return b->prev;
}

Block *next_block_in_addr(Block *b) {
    if ((char *)b + b->info.size * 16 + sizeof(Block) < (char *)heap_end) {
        return (Block *)((char *)b + b->info.size * 16 + sizeof(Block));
    }
    return NULL;
}

Block *prev_block_in_addr(Block *b) {
    if (b == heap_start) return NULL;
    Block *current = heap_start;
    while (current) {
        Block *next = next_block_in_addr(current);
        if (next == b) return current;
        if (next == NULL) break;
        current = next;
    }
    return NULL;
}

uint64_t numberof16blocks(size_t size_inbytes) {
    return (size_inbytes + 15) / 16;
}

void printheap() {
    Block *current = heap_start;
    while (current < heap_end) {
        printf("--------\n");
        printf("Size: %llu\n", current->info.size * 16);
        printf("Free: %d\n", current->info.isfree);
        printf("--------\n");
        current = next_block_in_addr(current);
    }
}


ListType getlisttype() {
    return listtype;
}

int setlisttype(ListType lt) {
    listtype = lt;
    return 0;
}

Strategy getstrategy() {
    return strategy;
    }
int setstrategy(Strategy s) {
strategy = s;
return 0;
}
// Allocation strategies
Block *first_fit(size_t size) {
    Block *current = free_list;
    while (current) {
        if (current->info.isfree && current->info.size >= size)
            return current;
        current = current->next;
    }
    return NULL; // No suitable free block found
}

Block *best_fit(size_t size) {
    Block *current = free_list;
    Block *best_block = NULL;
    uint64_t min_size = 0; // Initialize with 0 instead of UINT64_MAX

    while (current) {
        if (current->info.isfree && current->info.size >= size && current->info.size < min_size) {
            best_block = current;
            min_size = current->info.size;
        }
        current = current->next;
    }

    return best_block;
}

Block *worst_fit(size_t size) {
    Block *current = free_list;
    Block *worst_block = NULL;
    uint64_t max_size = 0;

    while (current) {
        if (current->info.isfree && current->info.size >= size) {
            if (current->info.size > max_size) {
                worst_block = current;
                max_size = current->info.size;
            }
        }
        current = current->next;
    }

    return worst_block;
}

Block *next_fit(size_t size, Block *last) {
    Block *current = (last) ? last->next : free_list;
    Block *start = current; // Save the starting point

    while (current) {
        if (current->info.isfree && current->info.size >= size)
            return current;
        current = current->next;

        // Wrap around to the start of the list
        if (current == NULL)
            current = free_list;

        // If we've reached the starting point, break the loop
        if (current == start)
            break;
    }

    return NULL; // No suitable free block found
}

int main() {
    // Example usage...
    char *ptr1 = mymalloc(32);
    char *ptr2 = mymalloc(64);

    // Print heap information after allocations
    printf("Heap after allocations:\n");
    printheap();

    myfree(ptr1);
    myfree(ptr2);

    // Print heap information after deallocation
    printf("Heap after deallocations:\n");
    printheap();

    // Set list type and strategy (if needed)
    setlisttype(ADDR_ORDERED_LIST);
    setstrategy(BEST_FIT);

    // Get list type and strategy (if needed)
    ListType lt = getlisttype();
    Strategy st = getstrategy();
    printf("List type: %d, Strategy: %d\n", lt, st);

    return 0;
}
