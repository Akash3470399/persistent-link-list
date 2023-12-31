#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "disk.h"
#include "bitsarr.h"
#include "bitmap.h"
#include "helper.h"

FILE *diskfp;
long t = 18;        // disk size in bits
int m = 54;     // main link list node size
int d = 68;     // data link list node size

unsigned int data_bitmap_size, main_bitmap_size, sblk;
long data_list_base, main_list_base;
int ptr_size;
long x, y;
bitmap *main_bitmap = NULL, *data_bitmap = NULL;



int disk_config(char *diskname)
{
    
    int filesize;
    diskfp = fopen(diskname, "rb+");
    if(diskfp == NULL)
    {
        printf("Please check you have provied right disk name.\n");
        return 0;
    }
    
    filesize = get_filesize(diskname);  

    t = filesize * 8;   // disk size in bits
        
    ptr_size = ceil(log(t/m)/log(2));

    data_bitmap_size = t/d;
    main_bitmap_size = t/m;

    data_list_base = ptr_size + data_bitmap_size;
    main_list_base = t - (ptr_size + main_bitmap_size) - m; // as reverse   

    data_bitmap = bitmap_init(data_bitmap_size, 0);
    main_bitmap = bitmap_init(main_bitmap_size, 0);

    sblk = data_bitmap_size -1;

    disk_rd((unsigned char*)&x, ptr_size, 0);
    disk_rd((unsigned char*)&y, ptr_size, t-ptr_size);

    disk_rd(data_bitmap->arr, data_bitmap_size, ptr_size);
    disk_rd(main_bitmap->arr, main_bitmap_size, main_list_base + m);

    /*
    fprintf(stdout, "t %ld , ptr size %d, data ll bitmap size %d, main ll bitmap size %d\n", t, ptr_size, data_bitmap_size, main_bitmap_size);
    fprintf(stdout, "data list base : %ld, main_list_base %ld\n", data_list_base, main_list_base);
    fprintf(stdout, "data bitmap bast %d, main bitmap base %ld\n", ptr_size, main_list_base+m);
    */
    return 1;
}

// create and initilize the disk
int disk_init(char *diskname)
{
    int preoccupied_space, preoccupied_data_nodes, preoccupied_main_nodes, ones;
    bitmap *bm;

    disk_config(diskname);
    preoccupied_space = (ptr_size * 2) + data_bitmap_size + main_bitmap_size;

    preoccupied_data_nodes = mceil(preoccupied_space, d);
    preoccupied_main_nodes = mceil(preoccupied_space, m);

    x = data_bitmap_size - preoccupied_data_nodes;   
    y = main_bitmap_size - preoccupied_main_nodes;

    disk_wr((unsigned char*)&x, ptr_size, 0);
    disk_wr((unsigned char*)&y, ptr_size, t-ptr_size);

    bm = bitmap_init(preoccupied_data_nodes, 0xFF);
    disk_wr(bm->arr, preoccupied_data_nodes, data_list_base-preoccupied_data_nodes);
    destroy_bitmap(bm);

    bm = bitmap_init(preoccupied_main_nodes, 0xFF);
    disk_wr(bm->arr, preoccupied_main_nodes, t - ptr_size - preoccupied_main_nodes);
    destroy_bitmap(bm);
}

void disk_update_config()
{

    disk_wr((unsigned char*)&x, ptr_size, 0);
    disk_wr((unsigned char*)&y, ptr_size, t-ptr_size);
    
    disk_wr(main_bitmap->arr, main_bitmap->len, main_list_base + m);
    disk_wr(data_bitmap->arr, data_bitmap->len, ptr_size);

}


    // ********* datalen config *********
    // e.g filepos = 7 & datalen = 3
    // as to store data we only 3 bits & 3 bits can be store in one byte
    // but as in startbyte byte we only have 1 bit available so we need fetch
    // one byte extra to store remaining 2 bytes
    
    // startbyte : byte number in file where filepos numbered bit is present
    // startpos : position of filepos bit in startbyte byte

// write datalen number of bits from the bitsarr into the disk file
// starting from filepos bit position in the file
// 8 represent the byte size
int disk_wr(unsigned char *bitsarr, int datalen, int filepos)
{

    int buflen = 1 + mceil(datalen-(8- filepos%8), 8);

    unsigned char buffer[buflen+1];
    int startbyte = filepos/8, startpos = filepos % 8;

    memset(buffer, 0, buflen+1);
    fseek(diskfp, startbyte, SEEK_SET);
    fread(buffer, 1, buflen, diskfp);                   // it will move fp forward
    
    bits_put(buffer, startpos, bitsarr, datalen);

    fseek(diskfp, buflen * (-1), SEEK_CUR);             // get back to buffers position
    fwrite(buffer, 1, buflen, diskfp);
    fflush(diskfp);
}


// read datalen number of bits into bitsarr from the file starting from filepos bit number
int disk_rd(unsigned char *bitsarr, int datalen, int filepos)
{
    int buflen = 1 + mceil(datalen-(8- filepos%8), 8);
    unsigned char buffer[buflen+1];
    int startbyte = filepos/8, startpos = filepos % 8, bitscnt, rdbits = 0;

    memset(buffer, 0, buflen);
    fseek(diskfp, startbyte, SEEK_SET);
    fread(buffer, 1, buflen, diskfp);

    int i = 0;
    while(rdbits < datalen)
    {
        bitscnt = ((datalen - rdbits) < 8)? (datalen-rdbits): 8;
        bitsarr[i++] = bits_get(buffer, startpos+rdbits ,bitscnt);
        rdbits += bitscnt;
    }
    return rdbits;
}

