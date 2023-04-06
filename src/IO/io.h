#ifndef IO_H
#define _H

unsigned short insw(unsigned short port);
void outw(unsigned short port, unsigned short value);

unsigned char insb(unsigned short port);
void outb(unsigned short port, unsigned char value);


#endif