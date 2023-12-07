#include <stdio.h>
#include "bitmap.h"

#ifndef DISK_H
#define DISK_H

extern FILE *diskfp;
extern long t;     // disk size in bits
extern int m ;      // main link list node size
extern int d ;  // data link list node size

extern unsigned int data_bitmap_size, main_bitmap_size, sblk;
extern long data_list_base, main_list_base;
extern int ptrsize;
extern long x, y;
extern bitmap *main_bitmap, *data_bitmap;

int disk_config(char *diskname);
int disk_init(char *diskname);
void disk_update_config();

int disk_wr(unsigned char *, int datalen, int filepos);
int disk_rd(unsigned char *, int datalen, int filepos);

// internal methods
int _calc_ptrsize(long filesize);
#endif
