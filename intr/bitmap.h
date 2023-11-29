#ifndef BITMAP_H 
#define BITMAP_H

typedef struct bitmap bitmap;

struct bitmap
{
	unsigned char *arr;
	int len;
};


bitmap *bitsarr_tobitmap(unsigned char *bitsarr, int len);
void bitmap_tobitsarr(bitmap *bm, unsigned char *bitsarr);
bitmap *bitmap_init(int len, int val);
void bitmap_set(bitmap *b, int pos);
void bitmap_reset(bitmap *bm, int pos);
int bitmap_isset(bitmap *bm, int pos);
void bitmap_print(bitmap *bm);
void destroy_bitmap(bitmap *bm);

#endif
