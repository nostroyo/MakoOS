#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include "./idt/idt.h"
#include "./IO/io.h"
#include "mem/heap/kheap.h"
#include "mem/paging/paging.h"
#include "disk/disk.h"
#include "string/string.h"


uint16_t* video_mem = 0;
uint16_t term_row = 0;
uint16_t term_col = 0;


uint16_t terminal_make_char(char c, char color){
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color) {
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_writechar(char c, char color) {

    if(c == '\n') {
        term_col = 0;
        term_row ++;
        return;
    }

    terminal_putchar(term_col, term_row, c, color);
    term_col ++;
    
    if (term_col>= VGA_WIDTH) {
        term_col = 0;
        term_row ++;
    }
}

void terminal_init() {
    video_mem = (uint16_t*)(0xB8000);
    for (int y = 0; y < VGA_HEIGTH; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}
void print(const char* str) {
    size_t len = strlen(str);

    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
    
}


static paging_four_GB_chunck* chunk_dir = 0;
void kernel_main() {
    terminal_init();
    print("Hello, world! \ntest");


    kheap_init();

    disk_search_and_init();

    idt_init();
    chunk_dir =  paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(paging_get_directory(chunk_dir));

    enable_paging();
    

    enable_int();
 
}