#include <stdio.h>
#include <string.h>
#include "bitmap.h"
#include "helper.h"
#include "disk.h"

void init_bitmaps()
{
    int BUFLEN = 1024;
    unsigned char buffer[BUFLEN]; 
    int bufsize = (BUFLEN * 8), wrbits = 0, calc;
    long base;

    // initilizing data bitmap to all 0's
    calc = (data_bitmap.size / bufsize);

    memset(buffer, 0, BUFLEN);
    for(int i = 0; i < calc; i++)
    {
        disk_wr(buffer, bufsize, data_bitmap.base + wrbits);
        wrbits += bufsize;
    }

    calc = (data_bitmap.size % bufsize); 
    if(calc > 0)
        disk_wr(buffer, calc, data_bitmap.base + wrbits);

    
    wrbits = 0;
    // initilizeing main bitmap to all 0's
    calc = (main_bitmap.size / bufsize);

    for(int i = 0; i < calc; i++)
    {
        disk_wr(buffer, bufsize, main_bitmap.base + wrbits);
        wrbits += bufsize;
    }

    calc = (main_bitmap.size % bufsize); 
    if(calc > 0)
        disk_wr(buffer, calc, main_bitmap.base + wrbits);


    // marking preoccupied blocks as taken
    
    // for data bitmap
    wrbits = 0;
    memset(buffer, 0xFF, BUFLEN);
    calc = (data_bitmap.size - x) / bufsize;
    base = datalist_base - (data_bitmap.size - x);

    for(int i = 0; i < calc; i++)
    {
        disk_wr(buffer, bufsize, base + wrbits);
        wrbits += bufsize;
    }

    calc = (data_bitmap.size - x) % bufsize;
    if(calc > 0)
        disk_wr(buffer, calc, base + wrbits);
    
    // for main bitmap
    wrbits = 0;
    calc = (main_bitmap.size - y) / bufsize;
    base = t - ((3*ptrsize) +(main_bitmap.size - y));

    for(int i = 0; i < calc; i++)
    {
        disk_wr(buffer, bufsize, base + wrbits);
        wrbits += bufsize;
    }

    calc = (main_bitmap.size - y) % bufsize;
    if(calc > 0)
        disk_wr(buffer, calc, base + wrbits);
    
}
