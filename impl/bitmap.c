#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "helper.h"


bitmap *bitsarr_tobitmap(unsigned char *bitsarr, int len)
{
	bitmap *bm = (bitmap*)malloc(sizeof(bitmap));
	bm->arr = (unsigned char *)malloc(sizeof(unsigned char) * mceil(len, 8));

	for(int i = 0; i < mceil(len, 8); i++)
		bm->arr[i] = bitsarr[i];	
	bm->len = len;
	return bm;		
}

void bitmap_tobitsarr(bitmap *bm, unsigned char *bitsarr)
{
	for(int i = 0; i < mceil(bm->len, 8); i++)
		bitsarr[i] = bm->arr[i];
}	

bitmap *bitmap_init(int len, int val)
{
	bitmap *bm = (bitmap*)malloc(sizeof(bitmap));

	bm->arr = (unsigned char*)malloc(sizeof(unsigned char) * mceil(len, 8));
	bm->len = len;
	for(int i = 0; i < mceil(len, 8); i++)
		bm->arr[i] = val;
	return bm;
}

// set pos bit in bitmap b
void bitmap_set(bitmap *bm, int pos)
{
	int byteno = pos/8, bitpos = pos % 8;
	if(bm != NULL)
		bm->arr[byteno] |= 1<<bitpos;	
}

void bitmap_reset(bitmap *bm, int pos)
{
	int byteno = pos/8, bitpos=pos % 8;

	if(bm != NULL)
		bm->arr[byteno] &= ~(1<<bitpos);
}
void destroy_bitmap(bitmap *bm)
{
	free(bm->arr);
	free(bm);
}
