
#include <stdint.h>
#include "rprintf.h"

#define width 80
#define height 25

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}


unsigned char keyboard_map[128] =
{
   0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
 '9', '0', '-', '=', '\b',     /* Backspace */
 '\t',                 /* Tab */
 'q', 'w', 'e', 'r',   /* 19 */
 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
   0,                  /* 29   - Control */
 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
'\'', '`',   0,                /* Left shift */
'\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
 'm', ',', '.', '/',   0,                              /* Right shift */
 '*',
   0,  /* Alt */
 ' ',  /* Space bar */
   0,  /* Caps lock */
   0,  /* 59 - F1 key ... > */
   0,   0,   0,   0,   0,   0,   0,   0,  
   0,  /* < ... F10 */
   0,  /* 69 - Num lock*/
   0,  /* Scroll Lock */
   0,  /* Home key */
   0,  /* Up Arrow */
   0,  /* Page Up */
 '-',
   0,  /* Left Arrow */
   0,  
   0,  /* Right Arrow */
 '+',
   0,  /* 79 - End key*/
   0,  /* Down Arrow */
   0,  /* Page Down */
   0,  /* Insert Key */
   0,  /* Delete Key */
   0,   0,   0,  
   0,  /* F11 Key */
   0,  /* F12 Key */
   0,  /* All other keys are undefined */
};




struct termbuf {
  char ascii;
  char color;
};


int x = 0;
int y = 0;

void putc(char c) {
  struct termbuf *vram = (struct termbuf*)0xB8000;
 if (c == '\n') {
      x = 0;
      y++;
 } else {
 int index = y * width + x;
  vram[index].ascii = c;
  vram[index].color = 7;
  x++;

  if (x >= width) {
      x = 0;
      y++;
   }
 }
 if (y >= height) {
   for (int y = 1; y < height; y++) {
      for (int x = 0; x < width; x++) {
          vram[(y - 1) * width + x] = vram[y * width + x];
     }
   }

  for (int x = 0; x < width; x++) {
     vram[(height - 1) * width + x].ascii = ' ';
     vram[(height - 1) * width + x].color = 7;
   }
   y = height - 1;
 }
}

int print_string(void (*pc)(char), char *s) {
  while(*s != 0) {
     pc(*s);
     s++;
   }
}


void main() {

  //  unsigned short *vram = (unsigned short*)0xb8000; // Base address of video mem
  //  const unsigned char color = 7; // gray text on black background
 // Homework 2 Changes
    while(1) {
        uint8_t status = inb(0x64);

        if((status & 1) == 1) {
            uint8_t scancode = inb(0x60);
            if (scancode > 128) {
               continue;
           }
            esp_printf((int (*) (int)) putc, "0x%02x %c\n", scancode, keyboard_map[scancode]);
        }
    }
}
