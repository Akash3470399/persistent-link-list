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

int bitmap_isset(bitmap *bm, int pos)
{
	int byteno = pos/8, bitpos=pos % 8;
	if(bm != NULL)
		return ((bm->arr[byteno] & 1<<bitpos)>>bitpos);
}


void bitmap_print(bitmap *bm)
{
	FILE *fp = fopen("bitmap", "wb+");
	
	if(fp == NULL)
	{
		printf("Can't get bitmap at this moment.\n");
		return;
	}
	fprintf(fp, "bitmap len %d\nbitmap:\n%8x ", bm->len, 0);
	int arrlen = mceil(bm->len, 8);
	for(int i = 0; i < arrlen; i++)
	{
		
		for(int j = 7; j >= 0; j--)
			fprintf(fp, "%d", (bm->arr[i] & (1<<j))>>j);
		
		fprintf(fp, " ");

		if((i+1) % 8 == 0)
			fprintf(fp, "\n%8x ", i+1);
	}
	fclose(fp);
}

void destroy_bitmap(bitmap *bm)
{
	free(bm->arr);
	free(bm);
}


