
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

struct termbuf {
  char ascii;
  char color;
};


int x = 0;
int y = 0;

void putc(char c) {
  struct termbuf *vram = (struct termbuf*)0xB8000;
  int index = y * width + x;

  vram[index].ascii = c;
  vram[index].color = 7;
  x++;

  if (x >= width) {
      x = 0;
      y++;
   }

}


void main() {

    unsigned short *vram = (unsigned short*)0xb8000; // Base address of video mem
    const unsigned char color = 7; // gray text on black background

    putc('n');
    putc('i');
    putc('c');
    putc('k');


    while(1) {
        uint8_t status = inb(0x64);

        if(status & 1) {
            uint8_t scancode = inb(0x60);
        }
    }
}
