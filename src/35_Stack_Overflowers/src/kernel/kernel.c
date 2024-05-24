#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "stdio.h"
#include "multiboot2.h"
#include "string.h"
#include "descriptor_tables.h"
#include "interrupts.h"
#include "pit.h"

struct multiboot_info
{
  uint32_t size;
  uint32_t reserved;
  struct multiboot_tag *first;
};

int kernel_main();

int main(uint32_t magic, struct multiboot_info *mb_info_addr)
{
  // Initialize the monitor (screen output)
  // monitor_initialize();

  // Initialize the Global Descriptor Table (GDT).
  init_gdt();

  // Initialize the Interrupt Descriptor Table (IDT).
  init_idt();

  // Initialize the kernel's memory manager using the end address of the kernel.
  // init_kernel_memory(&end); // <------ THIS IS PART OF THE ASSIGNMENT

  // Initialize paging for memory management.
  // init_paging(); // <------ THIS IS PART OF THE ASSIGNMENT

  // Print memory information.
  // print_memory_layout(); // <------ THIS IS PART OF THE ASSIGNMENT

  // Initialize PIT
  init_pit(); // <------ THIS IS PART OF THE ASSIGNMENT

  // Print a hello world message.
  printf("hello world! \n");

  // Call the C++ main function of the kernel.
  return kernel_main();
}