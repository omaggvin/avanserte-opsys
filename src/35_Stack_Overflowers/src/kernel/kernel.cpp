
extern "C"{
    #include "system.h"
    //#include "memory/memory.h"
    #include "common.h"
    #include "interrupts.h"
    #include "input.h"
    //#include "song/song.h"
    #include "string.h"
    #include "pit.h"
}


// Existing global operator new overloads
//void* operator new(size_t size) {
  //  return malloc(size);
//}

//void* operator new[](size_t size) {
  //  return malloc(size);
//}

// Existing global operator delete overloads
//void operator delete(void* ptr) noexcept {
  //  free(ptr);
//}

//void operator delete[](void* ptr) noexcept {
  //  free(ptr);
//}

// Add sized-deallocation functions
//void operator delete(void* ptr, size_t size) noexcept {
   // (void)size; // Size parameter is unused, added to match required signature
  //  free(ptr);
//}

//void operator delete[](void* ptr, size_t size) noexcept {
    //(void)size; // Size parameter is unused, added to match required signature
    //free(ptr);
//}

extern "C" int kernel_main(void);
int kernel_main(){

    // Set up interrupt handlers
    register_interrupt_handler(3, [](registers_t* regs, void* context) {
        printf("Interrupt 3 - OK\n");
    }, NULL);

    register_interrupt_handler(4, [](registers_t* regs, void* context) {
        printf("Interrupt 4 - OK\n");
    }, NULL);
    
    // Trigger interrupts to test handlers
    asm volatile ("int $0x3");
    printf("%f");
    sleep_interrupt(1000);
    asm volatile ("int $0x4");

    // Enable interrupts
    asm volatile("sti");

    return 0;
}