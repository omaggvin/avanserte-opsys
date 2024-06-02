#include "song.h"
#include "pit.h"
#include "common.h"
#include "stdio.h"

void enable_speaker(){
    // Read the current state of the PC speaker control register
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    /*
    Bit 0: Speaker gate
            0: Speaker disabled
            1: Speaker enabled
    Bit 1: Speaker data
            0: Data is not passed to the speaker
            1: Data is passed to the speaker
    */
    // Check if bits 0 and 1 are not set (0 means that the speaker is disabled)
    if (speaker_state != (speaker_state | 3)) {
        // If bits 0 and 1 are not set, enable the speaker by setting bits 0 and 1 to 1
        outb(PC_SPEAKER_PORT, speaker_state | 3);
    }
}

// disable the PC speaker
void disable_speaker() {
    // Turn off the PC speaker
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, speaker_state & 0xFC);
}


// play a sound of a given frequency
void play_sound(uint32_t frequency) {
    if (frequency == 0) {
        return; // Do nothing if frequency is 0
    }

    // Calculate the divisor needed to set the desired frequency
    uint16_t divisor = (uint16_t)(PIT_BASE_FREQUENCY / frequency);

    // Set up the PIT
    outb(PIT_CMD_PORT, 0b10110110); 
    outb(PIT_CHANNEL2_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL2_PORT, (uint8_t)(divisor >> 8));

    // Enable the speaker by setting bits 0 and 1
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, speaker_state | 0x03);
}

// Function to stop the sound
void stop_sound(){
    // Stop the sound by disabling the gate to the speaker
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, speaker_state & ~0x03); // Clear bits 0 and 1
}

// Function to play a song by iterating through its notes
void play_song_impl(Song *song) {
    enable_speaker(); // Enable the speaker before playing the song
    for (uint32_t i = 0; i < song->length; i++) {
        Note* note = &song->notes[i]; // Get the current note
        printf("Note: %d, Freq=%d, Sleep=%d\n", i, note->frequency, note->duration);
        play_sound(note->frequency); // Play the current note
        sleep_interrupt(note->duration); // Wait for the duration of the note
        stop_sound(); // Stop the sound before playing the next note
    }
    disable_speaker(); // Disable the speaker after the song is played
}

// function to play a song
void play_song(Song *song) {
    play_song_impl(song); // Call the internal implementation to play the song
}



