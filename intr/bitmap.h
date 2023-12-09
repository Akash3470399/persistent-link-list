#ifndef BITMAP_H 
#define BITMAP_H

typedef struct bitmap_summary bitmap_summary;

struct bitmap_summary
{
    int size, setcount;
    long base;
};


void init_bitmaps();
void load_bitmap_summary();

#endif
