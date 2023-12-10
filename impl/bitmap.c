#include <stdio.h>
#include <string.h>

#include "consts.h"
#include "bitmap.h"
#include "helper.h"
#include "disk.h"


void load_lastptr(bitmap_summary *bitmap_ptr)
{
    unsigned char buffer[BITMAP_BUFLEN];
    int bufsize = (BITMAP_BUFLEN * 8), rdbits = 0;
    long base, count;

    count = bitmap_ptr->lastptr / 8;
    
    while(count > 0)
    {
        disk_rd(buffer, bufsize, bitmap_ptr->base + rdbits);
    }

}


// initilize the bitmaps to 0 & only setting the preoccupied blocks to 1
void init_bitmaps()
{
    unsigned char buffer[BITMAP_BUFLEN]; 
    int bufsize = (BITMAP_BUFLEN * 8), wrbits = 0;
    // bufsize is how many bits can be store in buffer 
 
    long base, preoccupied_blks, count;
    // bitmaps can be very huge & we can't have that huge buffer array 
    // so initiliseing the bitmaps with small buffer in parts

    // initilizing data bitmap to all 0's
    count = (data_bitmap.size / bufsize);

    memset(buffer, 0, BITMAP_BUFLEN);
    for(int i = 0; i < count; i++)
    {
        disk_wr(buffer, bufsize, data_bitmap.base + wrbits);
        wrbits += bufsize;
    }

    count = (data_bitmap.size % bufsize); 
    if(count > 0)
        disk_wr(buffer, count, data_bitmap.base + wrbits);

    
    wrbits = 0;
    // initilizeing main bitmap to all 0's
    count = (main_bitmap.size / bufsize);

    for(int i = 0; i < count; i++)
    {
        disk_wr(buffer, bufsize, main_bitmap.base + wrbits);
        wrbits += bufsize;
    }

    count = (main_bitmap.size % bufsize); 
    if(count > 0)
        disk_wr(buffer, count, main_bitmap.base + wrbits);


    // marking preoccupied blocks as taken
    
    // for data bitmap
    wrbits = 0;
    memset(buffer, 0xFF, BITMAP_BUFLEN);
    preoccupied_blks = (data_bitmap.size - data_bitmap.lastptr); 
    count = preoccupied_blks / bufsize;
    base = datalist_base - preoccupied_blks;

    for(int i = 0; i < count; i++)
    {
        disk_wr(buffer, bufsize, base + wrbits);
        wrbits += bufsize;
    }
    
    count = preoccupied_blks % bufsize;
    if(count > 0)
        disk_wr(buffer, count, base + wrbits);
    
    // for main bitmap
    wrbits = 0;
    preoccupied_blks = (main_bitmap.size - main_bitmap.lastptr);
    count = preoccupied_blks / bufsize;
    base = t - ((3*ptrsize) + preoccupied_blks);

    for(int i = 0; i < count; i++)
    {
        disk_wr(buffer, bufsize, base + wrbits);
        wrbits += bufsize;
    }

    count = preoccupied_blks % bufsize;
    if(count > 0)
        disk_wr(buffer, count, base + wrbits);
}
