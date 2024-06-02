#include "memory.h"
#include "system.h"

#define MAX_PAGE_ALIGNED_ALLOCS 32

// Global variables to initially manage memory
uint32_t last_alloc = 0;
uint32_t heap_end = 0;
uint32_t heap_begin = 0;
uint32_t pheap_begin = 0;
uint32_t pheap_end = 0;
uint8_t *pheap_desc = 0;
uint32_t memory_used = 0;

// Initialize the kernel memory manager
void init_kernel_memory(uint32_t* kernel_end)
{
    last_alloc = kernel_end + 0x1000; // Set last allocation address after kernel end
    heap_begin = last_alloc; // Set heap start address after last allocation
    pheap_end = 0x400000; // Set end of page-aligned heap
    pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096); // Calculate start of page-aligned heap
    heap_end = pheap_begin; // heap ends where pheap begins
    memset((char *)heap_begin, 0, heap_end - heap_begin); // Clear the heap
    pheap_desc = (uint8_t *)malloc(MAX_PAGE_ALIGNED_ALLOCS);  // Allocate memory for page descriptors
    printf("Kernel heap starts at 0x%x\n", last_alloc); // Print adress of start of the heap
}

// Print the current memory layout
void print_memory_layout()
{
    printf("Memory used: %d bytes\n", memory_used); // Print the amount of memory used
    printf("Memory free: %d bytes\n", heap_end - heap_begin - memory_used); // Print the amount of free memory 
    printf("Heap size: %d bytes\n", heap_end - heap_begin); // Print the total heap size
    printf("Heap start: 0x%x\n", heap_begin); // Print the start of the heap
    printf("Heap end: 0x%x\n", heap_end); // Print the end of the heap
    printf("PHeap start: 0x%x\nPHeap end: 0x%x\n", pheap_begin, pheap_end); // Print the start and end of the page-aligned heap
}

// Free a block of memory
void free(void *mem)
{
    alloc_t *alloc = (mem - sizeof(alloc_t));  // Get the allocation header
    memory_used -= alloc->size + sizeof(alloc_t); 
    alloc->status = 0; // Mark the block as free
}

// Free a block of page-aligned memory
void pfree(void *mem)
{
    if(mem < pheap_begin || mem > pheap_end) return; // Validate the memory address

    // Determine the page ID
    uint32_t ad = (uint32_t)mem;
    ad -= pheap_begin;
    ad /= 4096;

    // Set the page descriptor to free
    pheap_desc[ad] = 0;
}

// Allocate a block of page-aligned memory
char* pmalloc(size_t size)
{
    // Loop through the available list
    for(int i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++)
    {
        if(pheap_desc[i]) continue; // Skip if the page is already allocated
        pheap_desc[i] = 1; // Mark the page as allocated
        printf("PAllocated from 0x%x to 0x%x\n", pheap_begin + i*4096, pheap_begin + (i+1)*4096); // Print allocation info
        return (char *)(pheap_begin + i*4096); // Return the allocated memory address
    }
    printf("pmalloc: FATAL: failure!\n"); // Print error if allocation fails
    return 0;
}


// Allocate a block of memory
void* malloc(size_t size)
{
    if(!size) return 0; // Return null if size is zero

    // Loop through blocks to find an available block with enough size
    uint8_t *mem = (uint8_t *)heap_begin;
    while((uint32_t)mem < last_alloc)
    {
        alloc_t *a = (alloc_t *)mem; // Get the allocation header
        printf("mem=0x%x a={.status=%d, .size=%d}\n", mem, a->status, a->size);

        if(!a->size)
            goto nalloc;
        if(a->status) {
            mem += a->size; // Move to the next block
            mem += sizeof(alloc_t);
            mem += 4; // Skip padding
            continue;
        }
        // If the block is not allocated and its size is big enough,
        // adjust its size, set the status, and return the location.
        if(a->size >= size)
        {
            a->status = 1;
            printf("RE:Allocated %d bytes from 0x%x to 0x%x\n", size, mem + sizeof(alloc_t), mem + sizeof(alloc_t) + size);
            memset(mem + sizeof(alloc_t), 0, size);
            memory_used += size + sizeof(alloc_t);
            return (char *)(mem + sizeof(alloc_t));
        }
        // If the block is not allocated and its size is not big enough,
        // add its size and the sizeof(alloc_t) to the pointer and continue.
        mem += a->size;
        mem += sizeof(alloc_t);
        mem += 4;
    }

    nalloc:;
    if(last_alloc + size + sizeof(alloc_t) >= heap_end)
    {
        panic("Cannot allocate bytes! Out of memory.\n"); // Panic if out of memory
    }
    alloc_t *alloc = (alloc_t *)last_alloc; // Get the next allocation block
    alloc->status = 1; // Mark the block as allocated
    alloc->size = size; // Set the size of the block

    last_alloc += size; // Update the last allocation address
    last_alloc += sizeof(alloc_t); // Include the allocation header size
    last_alloc += 4; 
    printf("Allocated %d bytes from 0x%x to 0x%x\n", size, (uint32_t)alloc + sizeof(alloc_t), last_alloc); // Print allocation info
    memory_used += size + 4 + sizeof(alloc_t); // Update the used memory count
    memset((char *)((uint32_t)alloc + sizeof(alloc_t)), 0, size); // Clear the allocated memory
    return (char *)((uint32_t)alloc + sizeof(alloc_t)); // Return the allocated memory address
}