#include <stdio.h>
#include "bitmap.h"

#ifndef DISK_H
#define DISK_H

extern FILE *diskfp;
extern long t;     // disk size in bits
extern int m ;      // main link list node size
extern int d ;  // data link list node size

extern unsigned int nullblk;

// base of lists (bit position in file)
extern long datalist_base, mainlist_base;  

// list heads
extern int mainlist_head, datafreelist, mainfreelist;

// main link list length
extern long mainlist_length;

// pointer size of pointer used in file
extern int ptrsize;                                            

// store the information related to the bitmap
extern bitmap_summary main_bitmap, data_bitmap;


int disk_config(char *diskname);
int disk_init(char *diskname);
void disk_update_config();

int disk_wr(unsigned char *, int datalen, int filepos);
int disk_rd(unsigned char *, int datalen, int filepos);

// internal methods
int _calc_ptrsize(long filesize);
#endif
