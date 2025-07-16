#ifndef IO_H
#define IO_H

// Read byte from given port
unsigned char insb(unsigned short port);

// Read word from given port
unsigned short insw(unsigned short port);

// output byte to given port
void outb(unsigned short port, unsigned char val);

// output a word to given port
void outw(unsigned short port, unsigned char val);
#endif