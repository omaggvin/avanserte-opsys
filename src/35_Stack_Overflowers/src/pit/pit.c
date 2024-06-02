#include "pit.h"
#include "interrupts.h"
#include "common.h"

static uint32_t ticks = 0; // Global variable to keep track of ticks

// Define the IRQ handler function for the Programmable Interval Timer (PIT)
void pit_irq_handler(registers_t* regs, void* context) {
    ticks++;
}

// Initialize the PIT
void init_pit() {
    // Register the PIT IRQ handler to handle IRQ0 interrupts
    register_irq_handler(IRQ0, pit_irq_handler, NULL);

    // Send the command byte to the PIT command port to set the mode
    outb(PIT_CMD_PORT, 0x36);

    // Split up the divisor into upper and lower bytes
    uint8_t l_divisor = (uint8_t)(PIT_BASE_FREQUENCY / TARGET_FREQUENCY);
    uint8_t h_divisor = (uint8_t)((PIT_BASE_FREQUENCY / TARGET_FREQUENCY) >> 8);

    // Send the frequency divisor
    outb(PIT_CHANNEL0_PORT, l_divisor);
    outb(PIT_CHANNEL0_PORT, h_divisor);
}

// Put the CPU to sleep for a specified number of milliseconds using interrupts.
void sleep_interrupt(uint32_t milliseconds){
    uint32_t current_tick = ticks; // current tick count.
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS; // Calculate the number of ticks to wait.
    uint32_t end_ticks = current_tick + ticks_to_wait; // Calculate the end tick count.

    while (current_tick < end_ticks) { // Loop until the current tick count reaches the end tick count.
          // Enable interrupts to allow the CPU to handle interrupt requests.
        asm volatile("sti");
        // Halt the CPU until the next interrupt (hlt)
        asm volatile("hlt");
        current_tick = ticks; // Update the current tick count.
    }
}

void sleep_busy(uint32_t milliseconds){ // Put the CPU to sleep for a specified number of milliseconds using busy-waiting.
    uint32_t start_tick = ticks;  // Capture the start tick count.
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS; // Calculate the number of ticks to wait.
    uint32_t elapsed_ticks = 0; // Initialize the elapsed tick count.
    
      // Loop until the elapsed tick count reaches the ticks to wait.
    while (elapsed_ticks < ticks_to_wait) {
        // Busy-wait until the tick count advances by one.
        while (ticks == start_tick + elapsed_ticks) {};
        elapsed_ticks++; // Increment the elapsed tick count.
    }    
}