#include "libc/stdio.h"
#include "libc/string.h"
#include "libc/stdarg.h"

uint16_t* video_memory_offset = (uint16_t*)0xB8000;

enum vga_colors {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GRAY = 7,
    VGA_COLOR_DARK_GRAY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

uint16_t vga_rich_char(
    unsigned char u_char, 
    unsigned char char_color, 
    unsigned char bgcolor
    ) {
    return (uint16_t) u_char | (uint16_t) char_color << 8 | (uint16_t) bgcolor << 12;
}
const int lineLength = 80;
static int cursor [2] = {0, 0};

const int default_text_color = VGA_COLOR_WHITE;
const int default_bg_color = VGA_COLOR_BLACK;

void goNextLine() {
    cursor[0] = 0;
    cursor[1]++;
}
int putchar(int myChar) {
    uint16_t entry = vga_rich_char(myChar, default_text_color, default_bg_color);
    if (myChar == '\n') {
        cursor[1]++;
        cursor[0] = 0;
    } else {
        video_memory_offset[cursor[1] * lineLength + cursor[0]] = entry;
        cursor[0]++;
        if (cursor[0] >= lineLength) {
            goNextLine();
        }
    }

}

int printf(const char* __restrict__ format_string, ...) {
    va_list args;
    va_start(args, format_string);
    int printed = 0;
    for (const char* char_ptr = format_string; *char_ptr != '\0'; char_ptr++) {

        if (*char_ptr != '%') {
            putchar(*char_ptr);
            printed++;

        } else {
            //not implemented
        }
    va_end(args);

    }
}
