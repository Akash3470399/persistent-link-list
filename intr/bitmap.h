#ifndef BITMAP_H 
#define BITMAP_H

typedef struct bitmap_summary bitmap_summary;

struct bitmap_summary
{
    int size, nextfreeblk, lastptr;
    long base;
};

void set(bitmap_summary bitmap, int pos);
void reset(bitmap_summary bitmap, int pos);
void init_bitmaps();
void load_bitmap_summary(bitmap_summary *);
#endif
